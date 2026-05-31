#pragma once
#include <Windows.h>
#include <cstdint>
#include <cmath>
#include <cfloat>
#include <unordered_map>
#include "Game.hpp"
#include "Offsets.hpp"
#include "Patterns.hpp"
#include "Trace.hpp"
#include "Tracers.hpp"
#include "Triggerbot.hpp"
#include "NoSpread.hpp"
#include <CS2/SDK/Types/CEntityData.hpp>
#include <CS2/SDK/Math/Vector3.hpp>
#include <CS2/SDK/Types/CEntityData.hpp>
#include <CS2/SDK/Update/CCSGOInput.hpp>
#include <CS2/SDK/Update/CUserCmd.hpp>
#include <GameClient/CL_Bypass.hpp>
#include <GameClient/CL_Bones.hpp>
#include <GameClient/CL_Players.hpp>
#include <Client/CCookieGUI.hpp>
#include <ImGui/imgui.h>
#include "SkinChanger.hpp"

namespace Aimbot
{
    using Game::Vector3;
    using Game::QAngle;

    struct AimbotConfig
    {
        bool  enabled    = false;
        int   fovType    = 0;        // 0 = Angle (Degrees), 1 = Screen (Pixels)
        float fov        = 5.0f;     // For Angle FOV
        float screenFov  = 100.0f;   // For Screen FOV
        int   targetHitbox = 0;      // 0=head, 1=neck, 2=chest, 3=pelvis
        int   aimKey     = VK_LBUTTON;
        bool  autoShoot  = false;
        bool  silentAim  = true;
        bool  teamCheck  = true;     // skip same-team (disable for FFA/DM)
        bool  visCheck   = true;     // TraceShape LOS — ragebot skips walled targets
        bool  recoilControl = false;
    };

    inline AimbotConfig config;

    // ===== Debug =====
    inline bool debugEnabled     = false;
    inline int  debugTickCounter = 0;
    inline float drawFovRadius   = 0.0f; // Stores active FOV radius for drawing
    inline int   menuBlockFrames = 0;    // ignore aim/fire briefly after menu input
    inline bool  blockAntiAim    = false; // set when aimbot owns this cmd tick

    inline bool IsMenuBlockingCombat()
    {
        if ( GetCookieGUI() && GetCookieGUI()->IsVisible() )
            return true;

        if ( ImGui::GetCurrentContext() && ImGui::GetIO().WantCaptureMouse )
            return true;

        return menuBlockFrames > 0;
    }

    // ===== Render FOV Circle =====
    inline void RenderFOV()
    {
        if (!config.enabled) return;
        
        ImDrawList* draw = ImGui::GetBackgroundDrawList();
        if (!draw) return;
        
        ImVec2 center = ImGui::GetIO().DisplaySize;
        center.x *= 0.5f;
        center.y *= 0.5f;

        if (config.fovType == 1) // Screen FOV
        {
            draw->AddCircle(center, config.screenFov, IM_COL32(255, 255, 255, 60), 64, 1.0f);
        }
        else if (config.fovType == 0) // Angle FOV visualization approximation (for active weapon)
        {
            if (drawFovRadius > 0.0f) {
                draw->AddCircle(center, drawFovRadius, IM_COL32(255, 255, 255, 60), 64, 1.0f);
            }
        }
    }

    // ===== Constants =====
    constexpr float PI      = 3.14159265358979323846f;
    constexpr float RAD2DEG = 180.0f / PI;

    // ===== Angle Math =====
    inline void ClampAngles(QAngle& a)
    {
        if (a.pitch >  89.f) a.pitch =  89.f;
        if (a.pitch < -89.f) a.pitch = -89.f;
        while (a.yaw >  180.f) a.yaw -= 360.f;
        while (a.yaw < -180.f) a.yaw += 360.f;
        a.roll = 0.f;
    }

    inline void ApplyRecoilControl( QAngle& aimAngle , C_CSPlayerPawn* pawn )
    {
        if ( !config.recoilControl || !pawn )
            return;

        if ( pawn->m_iShotsFired() <= 0 )
            return;

        const auto& punchCache = pawn->m_aimPunchCache();
        const int punchCount = punchCache.Count();
        if ( punchCount <= 0 )
            return;

        const ::QAngle& punch = punchCache[punchCount - 1];
        aimAngle.pitch -= punch.m_x * 2.f;
        aimAngle.yaw -= punch.m_y * 2.f;
        ClampAngles( aimAngle );
    }

    inline QAngle CalcAngle(const Vector3& src, const Vector3& dst)
    {
        Vector3 d = dst - src;
        float hyp = d.Length2D();
        QAngle a;
        a.pitch = -atan2f(d.z, hyp) * RAD2DEG;
        a.yaw   =  atan2f(d.y, d.x) * RAD2DEG;
        a.roll  = 0.f;
        ClampAngles(a);
        return a;
    }

    inline float GetFOV(const QAngle& view, const QAngle& aim)
    {
        float dp = aim.pitch - view.pitch;
        float dy = aim.yaw - view.yaw;
        while (dy >  180.f) dy -= 360.f;
        while (dy < -180.f) dy += 360.f;
        return sqrtf(dp * dp + dy * dy);
    }

    inline float GetScreenDistance(const Vector3& targetPos)
    {
        float outX, outY;
        float pos[3] = { targetPos.x, targetPos.y, targetPos.z };
        ImVec2 size = ImGui::GetIO().DisplaySize;
        if (Game::WorldToScreen(pos, outX, outY, size.x, size.y))
        {
            float dx = outX - (size.x * 0.5f);
            float dy = outY - (size.y * 0.5f);
            return sqrtf(dx * dx + dy * dy);
        }
        return FLT_MAX;
    }

    // ===== Visibility Check (TraceShape) =====

    inline const char* GetHitboxBoneName( int hitbox )
    {
        static const char* const kBoneNames[] = { "head_0" , "neck_0" , "spine_2" , "pelvis" };
        constexpr int kHitboxCount = 4;
        if ( hitbox < 0 || hitbox >= kHitboxCount )
            hitbox = 0;
        return kBoneNames[hitbox];
    }

    inline Game::Vector3 ToGameVector( const ::Vector3& v )
    {
        return { v.m_x , v.m_y , v.m_z };
    }

    inline ::Vector3 GetTargetBonePosition( uintptr_t pawn , int hitbox )
    {
        auto* sdkPawn = reinterpret_cast<C_CSPlayerPawn*>( pawn );
        if ( !sdkPawn )
            return {};

        return GetCL_Bones()->GetBonePositionByName( sdkPawn , GetHitboxBoneName( hitbox ) );
    }

    inline ::Vector3 GetLocalEyePosition()
    {
        return GetCL_Players()->GetLocalEyeOrigin();
    }

    // ===== Target Selection =====
    struct AimTarget
    {
        uintptr_t pawn = 0;
        Vector3   pos;
        QAngle    angle;
        float     fov = FLT_MAX;
    };

    inline AimTarget GetBestTarget(const Vector3& eye, const QAngle& viewAngles, bool doDebug)
    {
        AimTarget best;
        float bestScore = FLT_MAX;

        uintptr_t localCtrl = Game::Read<uintptr_t>(Game::clientBase + Offsets::dwLocalPlayerController);
        if (!localCtrl)
        {
            if (doDebug) printf("[Aim DBG] No local controller\n");
            return best;
        }

        uint32_t localHandle = Game::Read<uint32_t>(localCtrl + Offsets::m_hPlayerPawn);
        uintptr_t localPawn = Game::GetEntityByHandle(localHandle);
        if (!localPawn)
        {
            if (doDebug) printf("[Aim DBG] No local pawn\n");
            return best;
        }
        int localTeam = Game::Read<uint8_t>(localPawn + Offsets::m_iTeamNum);
        if (doDebug) printf("[Aim DBG] LocalPawn=0x%IX team=%d teamCheck=%d visCheck=%d trace=%d\n",
            localPawn, localTeam, config.teamCheck, config.visCheck, Trace::ready);

        if (config.visCheck && !Trace::ready && !Trace::Init())
        {
            if (doDebug) printf("[Aim DBG] TraceShape not ready 鈥?no targets\n");
            return best;
        }

        uintptr_t entList = Game::Read<uintptr_t>(Game::clientBase + Offsets::dwEntityList);
        if (!entList)
        {
            if (doDebug) printf("[Aim DBG] Entity list null\n");
            return best;
        }

        int enemiesFound = 0;
        std::unordered_map<uintptr_t, bool> visCache;

        for (int i = 1; i <= 64; ++i)
        {
            uint32_t chunkIndex = i >> 9;
            uint32_t entryIndex = i & 0x1FF;

            uintptr_t chunkAddr = entList + 0x8 * chunkIndex + 0x10;
            uintptr_t listEntry = Game::Read<uintptr_t>(chunkAddr);
            if (!listEntry) continue;

            uintptr_t ctrl = Game::Read<uintptr_t>(listEntry + Game::ENTITY_IDENTITY_SIZE * entryIndex);
            if (!ctrl) continue;

            uint32_t pH = Game::Read<uint32_t>(ctrl + Offsets::m_hPlayerPawn);
            if (!pH || pH == 0xFFFFFFFF) continue;

            uintptr_t pawn = Game::GetEntityByHandle(pH);
            if (!pawn || pawn == localPawn) continue;

            // Alive check
            int health = Game::Read<int32_t>(pawn + Offsets::m_iHealth);
            uint8_t lifeState = Game::Read<uint8_t>(pawn + Offsets::m_lifeState);
            if (health <= 0 || lifeState != 0) continue;

            // Team check (can be disabled for FFA/DM)
            int team = Game::Read<uint8_t>(pawn + Offsets::m_iTeamNum);
            if (config.teamCheck && team == localTeam) continue;

            enemiesFound++;

            // Get target bone position (CalcWorldSpaceBones + bone name lookup)
            ::Vector3 sdkBonePos = GetTargetBonePosition( pawn , config.targetHitbox );
            if ( sdkBonePos.IsZero() )
                continue;

            Game::Vector3 bonePos = ToGameVector( sdkBonePos );

            QAngle aimAng = CalcAngle(eye, bonePos);
            
            float fovVal = FLT_MAX;
            if (config.fovType == 0) // Angle
            {
                fovVal = GetFOV(viewAngles, aimAng);
                if (fovVal > config.fov) continue;
            }
            else // Screen
            {
                fovVal = GetScreenDistance(bonePos);
                if (fovVal > config.screenFov) continue;
            }

            // Calculate distance
            Vector3 delta = bonePos - eye;
            float dist = sqrtf(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);

            if (config.visCheck)
            {
                if (dist > 8192.0f)
                    continue;

                auto it = visCache.find(pawn);
                bool visible;
                if (it != visCache.end())
                    visible = it->second;
                else
                {
                    visible = Trace::IsVisible(eye, bonePos, localPawn, localHandle, pawn);
                    visCache[pawn] = visible;
                }
                if (!visible)
                    continue;
            }

            if (doDebug && enemiesFound <= 5)
            {
                printf("[Aim DBG] Enemy %d: hp=%d team=%d fov=%.1f dist=%.0f\n",
                    i, health, team, fovVal, dist);
            }

            // Weighted score: lower = better
            // FOV is primary factor, distance is secondary
            float score = fovVal + (dist * 0.01f);

            if (score < bestScore)
            {
                bestScore  = score;
                best.pawn  = pawn;
                best.pos   = bonePos;
                best.angle = aimAng;
                best.fov   = fovVal;
            }
        }

        if (doDebug) printf("[Aim DBG] Enemies: %d, best fov: %.1f\n",
            enemiesFound, best.fov == FLT_MAX ? -1.f : best.fov);

        return best;
    }

    inline void OnCreateMove( CCSGOInput* input , uint32_t slot , CUserCmd* cmd )
    {
        debugTickCounter++;
        blockAntiAim = false;
        const bool doDebug = debugEnabled && ( debugTickCounter % 512 == 1 );

        if ( !Game::clientBase || !input || !cmd )
            return;

        {
            static bool s_prevMenuVisible = false;
            const bool menuVisible = GetCookieGUI() && GetCookieGUI()->IsVisible();

            if ( menuVisible )
                menuBlockFrames = 12;
            else if ( s_prevMenuVisible && !menuVisible )
                menuBlockFrames = 20;
            else if ( menuBlockFrames > 0 )
                menuBlockFrames--;

            s_prevMenuVisible = menuVisible;
        }

        uintptr_t localPawn = Game::GetLocalPlayerPawn();

        if ( localPawn )
            Triggerbot::Tick( localPawn );

        QAngle viewAngles = Game::Read<QAngle>( Game::clientBase + Offsets::dwViewAngles );
        ::Vector3 sdkEye = GetLocalEyePosition();
        Game::Vector3 eyePos = ToGameVector( sdkEye );

        float tracePitch = viewAngles.pitch;
        float traceYaw = viewAngles.yaw;

        if ( config.enabled && localPawn && !eyePos.IsZero() )
        {
            const bool menuBlocking = IsMenuBlockingCombat();

            bool shouldAim = false;
            if ( !menuBlocking )
            {
                if ( config.autoShoot )
                    shouldAim = true;
                else if ( config.aimKey != 0 )
                    shouldAim = ( GetAsyncKeyState( config.aimKey ) & 0x8000 ) != 0;
                else
                    shouldAim = true;
            }

            if ( shouldAim )
            {
                AimTarget target = GetBestTarget( eyePos , viewAngles , doDebug );
                if ( target.pawn != 0 )
                {
                    blockAntiAim = true;
                    ClampAngles( target.angle );

                    if ( auto* sdkPawn = GetCL_Players()->GetLocalPlayerPawn() )
                        ApplyRecoilControl( target.angle , sdkPawn );

                    QAngle shootAngles( target.angle.pitch , target.angle.yaw , 0.f );
                    if ( NoSpread::config.enabled )
                    {
                        ::QAngle compensated( target.angle.pitch , target.angle.yaw , 0.f );
                        ::QAngle desired( target.angle.pitch , target.angle.yaw , 0.f );
                        if ( NoSpread::CompensateAngles( desired , cmd , compensated ) )
                            shootAngles = QAngle( compensated.m_x , compensated.m_y , 0.f );
                    }

                    ::QAngle sdkAngles( shootAngles.pitch , shootAngles.yaw , 0.f );
                    const bool silentOnly = config.silentAim && !config.autoShoot;
                    GetCL_Bypass()->SetViewAngles(
                        &sdkAngles ,
                        input ,
                        cmd ,
                        !config.silentAim ,
                        silentOnly );

                    if ( config.silentAim && config.autoShoot )
                    {
                        GetCL_Bypass()->SetViewAngles(
                            &sdkAngles ,
                            input ,
                            cmd ,
                            false ,
                            true );
                    }

                    tracePitch = shootAngles.pitch;
                    traceYaw = shootAngles.yaw;

                    if ( config.autoShoot )
                        GetCL_Bypass()->SetAttack( cmd , true );
                }
                else if ( config.autoShoot )
                {
                    GetCL_Bypass()->SetDontAttack( cmd , true );
                }
            }
            else if ( config.autoShoot )
            {
                GetCL_Bypass()->SetDontAttack( cmd , true );
            }
        }
        else if ( config.autoShoot )
        {
            GetCL_Bypass()->SetDontAttack( cmd , true );
        }

        if ( BulletTracer::DetectShot() && !eyePos.IsZero() )
        {
            BulletTracer::AddTraceFromAngles(
                eyePos.x , eyePos.y , eyePos.z ,
                tracePitch , traceYaw );
        }
    }

    inline bool Init()
    {
        return true;
    }

    inline void Shutdown()
    {
    }
}
