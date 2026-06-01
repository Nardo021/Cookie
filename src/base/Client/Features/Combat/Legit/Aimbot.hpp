#pragma once
#include <Windows.h>
#include <cstdint>
#include <cmath>
#include <cfloat>
#include <unordered_map>
#include <vector>
#include "Game.hpp"
#include "Offsets.hpp"
#include "Patterns.hpp"
#include "Trace.hpp"
#include "Triggerbot.hpp"
#include "NoSpread.hpp"
#include "AutoWall.hpp"
#include <Client/Utils/KeyBindUtils.hpp>
#include <CS2/SDK/Types/CEntityData.hpp>
#include <CS2/SDK/Math/Vector3.hpp>
#include <CS2/SDK/Types/CEntityData.hpp>
#include <CS2/SDK/Update/CCSGOInput.hpp>
#include <CS2/SDK/Update/CUserCmd.hpp>
#include <GameClient/CL_Bypass.hpp>
#include <Client/Features/Combat/Legit/LegitScan.hpp>
#include <Client/Features/Combat/HitboxData.hpp>
#include <GameClient/CL_Players.hpp>
#include <GameClient/CL_Weapons.hpp>
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
        int   targetHitbox = 0;      // legacy single-hitbox fallback
        int   aimKey     = VK_LBUTTON;
        bool  aimKeyHold = true;
        bool  aimUseKey  = true;
        bool  aimShowInBinds = true;
        bool  autoShoot  = false;
        bool  silentAim  = true;
        bool  teamCheck  = true;
        bool  visCheck   = true;
        bool  penetration = false;
        bool  recoilControl = false;
        int   smooth = 0;
        bool  punchRandomization = false;
        int   targetSelection = 0;
        bool  hitboxHead = true;
        bool  hitboxNeck = false;
        bool  hitboxChest = true;
        bool  hitboxUpperChest = false;
        bool  hitboxPelvis = false;
        bool  hitboxStomach = false;
        bool  hitboxLegs = false;
        bool  hitboxArms = false;
        int   rcsShots = 0;
        bool  rcsShotsEnable = false;
        float rcsSmoothX = 0.f;
        float rcsSmoothY = 0.f;
        float punchRandomX = 0.f;
        float punchRandomY = 0.f;
        bool  fovVisualize = false;
        float fovVisualizeColor[4] = { 1.f , 0.35f , 0.35f , 0.35f };
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
        if ( !config.fovVisualize )
            return;
        
        ImDrawList* draw = ImGui::GetBackgroundDrawList();
        if (!draw) return;
        
        ImVec2 center = ImGui::GetIO().DisplaySize;
        center.x *= 0.5f;
        center.y *= 0.5f;

        const ImU32 fovColor = IM_COL32(
            static_cast<int>( config.fovVisualizeColor[0] * 255.f ) ,
            static_cast<int>( config.fovVisualizeColor[1] * 255.f ) ,
            static_cast<int>( config.fovVisualizeColor[2] * 255.f ) ,
            static_cast<int>( config.fovVisualizeColor[3] * 255.f ) );

        if (config.fovType == 1) // Screen FOV
        {
            draw->AddCircle(center, config.screenFov, fovColor, 64, 1.0f);
        }
        else if (config.fovType == 0) // Angle FOV visualization approximation (for active weapon)
        {
            if (drawFovRadius > 0.0f) {
                draw->AddCircle(center, drawFovRadius, fovColor, 64, 1.0f);
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

        const int shotsFired = pawn->m_iShotsFired();
        if ( shotsFired <= 0 )
            return;

        if ( config.rcsShotsEnable && config.rcsShots > 0 && shotsFired < config.rcsShots )
            return;

        const auto& punchCache = pawn->m_aimPunchCache();
        const int punchCount = punchCache.Count();
        if ( punchCount <= 0 )
            return;

        const ::QAngle& punch = punchCache[punchCount - 1];
        const float smoothX = config.rcsSmoothX > 0.f ? config.rcsSmoothX : 2.f;
        const float smoothY = config.rcsSmoothY > 0.f ? config.rcsSmoothY : 2.f;
        float deltaX = punch.m_x * smoothX;
        float deltaY = punch.m_y * smoothY;

        if ( config.punchRandomization )
        {
            const float randScaleX = config.punchRandomX > 0.f ? config.punchRandomX : 0.3f;
            const float randScaleY = config.punchRandomY > 0.f ? config.punchRandomY : 0.3f;
            deltaX *= randScaleX + ( static_cast<float>( punchCount % 5 ) * 0.1f );
            deltaY *= randScaleY + ( static_cast<float>( ( punchCount + 2 ) % 5 ) * 0.1f );
        }

        aimAngle.pitch -= deltaX;
        aimAngle.yaw -= deltaY;
        ClampAngles( aimAngle );
    }

    inline void CollectLegitHitboxes( std::vector<uint32_t>& out ) noexcept
    {
        LegitScan::HitboxFilter filter{};
        filter.head = config.hitboxHead;
        filter.neck = config.hitboxNeck;
        filter.chest = config.hitboxChest;
        filter.upperChest = config.hitboxUpperChest;
        filter.pelvis = config.hitboxPelvis;
        filter.stomach = config.hitboxStomach;
        filter.legs = config.hitboxLegs;
        filter.arms = config.hitboxArms;
        filter.legacyFallback = config.targetHitbox;
        LegitScan::CollectHitboxes( filter , out );
    }

    inline QAngle PerformSmooth( const QAngle& current , const QAngle& target , int smooth ) noexcept
    {
        if ( smooth <= 0 )
            return target;

        QAngle out = current;
        const float factor = 1.f / static_cast<float>( std::max( 1 , smooth ) );
        out.pitch += ( target.pitch - current.pitch ) * factor;
        out.yaw += ( target.yaw - current.yaw ) * factor;
        ClampAngles( out );
        return out;
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

    inline Game::Vector3 ToGameVector( const ::Vector3& v )
    {
        return { v.m_x , v.m_y , v.m_z };
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
        const ::Vector3 sdkEye( eye.x , eye.y , eye.z );

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

            auto* sdkPawn = reinterpret_cast<C_CSPlayerPawn*>( pawn );
            if ( !LegitScan::IsValidTarget( sdkPawn ) )
                continue;

            enemiesFound++;

            std::vector<uint32_t> hitboxes;
            CollectLegitHitboxes( hitboxes );

            for ( uint32_t studioHitbox : hitboxes )
            {
                const ::Vector3 sdkBonePos = LegitScan::GetHitboxPosition( sdkPawn , studioHitbox );
                if ( sdkBonePos.IsZero() )
                    continue;

                Game::Vector3 bonePos = ToGameVector( sdkBonePos );
                QAngle aimAng = CalcAngle( eye , bonePos );

                float fovVal = FLT_MAX;
                if ( config.fovType == 0 )
                {
                    fovVal = GetFOV( viewAngles , aimAng );
                    if ( fovVal > config.fov )
                        continue;
                }
                else
                {
                    fovVal = GetScreenDistance( bonePos );
                    if ( fovVal > config.screenFov )
                        continue;
                }

                Vector3 delta = bonePos - eye;
                float dist = sqrtf( delta.x * delta.x + delta.y * delta.y + delta.z * delta.z );

                if ( config.visCheck )
                {
                    if ( dist > 8192.0f )
                        continue;

                    if ( LegitScan::IsSmokeBetween( sdkEye , sdkBonePos ) )
                        continue;

                    auto it = visCache.find( pawn );
                    bool visible;
                    if ( it != visCache.end() )
                        visible = it->second;
                    else
                    {
                        visible = Trace::IsVisible( eye , bonePos , localPawn , localHandle , pawn );
                        if ( !visible && config.penetration )
                        {
                            if ( auto* vdata = GetCL_Weapons()->GetLocalWeaponVData() )
                            {
                                const auto pen = AutoWall::CanPenetrate( sdkEye , sdkBonePos , sdkPawn , vdata );
                                visible = pen.canHit && pen.damage >= 1.f;
                            }
                        }
                        visCache[pawn] = visible;
                    }
                    if ( !visible )
                        continue;
                }

                float score = 0.f;
                switch ( config.targetSelection )
                {
                case 1: score = fovVal; break;
                case 2: score = dist; break;
                default: score = fovVal + ( dist * 0.01f ); break;
                }

                if ( score < bestScore )
                {
                    bestScore = score;
                    best.pawn = pawn;
                    best.pos = bonePos;
                    best.angle = aimAng;
                    best.fov = fovVal;
                }
            }
        }

        if ( doDebug ) printf( "[Aim DBG] Enemies: %d, best fov: %.1f\n" ,
            enemiesFound , best.fov == FLT_MAX ? -1.f : best.fov );

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

        QAngle viewAngles = Game::Read<QAngle>( Game::clientBase + Offsets::dwViewAngles );
        ::Vector3 sdkEye = GetLocalEyePosition();
        Game::Vector3 eyePos = ToGameVector( sdkEye );

        if ( config.fovType == 0 && config.fovVisualize )
        {
            const float screenH = ImGui::GetIO().DisplaySize.y;
            drawFovRadius = LegitScan::CalcDrawFovRadius( config.fov , screenH );
        }
        else
        {
            drawFovRadius = 0.f;
        }

        float tracePitch = viewAngles.pitch;
        float traceYaw = viewAngles.yaw;

        if ( config.enabled && localPawn && !eyePos.IsZero() )
        {
            const bool menuBlocking = IsMenuBlockingCombat();

            bool shouldAim = false;
            if ( !menuBlocking )
            {
                static KeyBindUtils::KeyBindSlot s_aimKeySlot{};
                if ( config.autoShoot )
                    shouldAim = true;
                else if ( config.aimUseKey )
                    shouldAim = KeyBindUtils::IsActive(
                        static_cast<unsigned int>( config.aimKey ) ,
                        true ,
                        config.aimKeyHold ,
                        s_aimKeySlot );
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
                    if ( config.smooth > 0 && !config.silentAim )
                        shootAngles = PerformSmooth( viewAngles , shootAngles , config.smooth );
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
                    {
                        auto* activeWeapon = GetCL_Weapons()->GetLocalActiveWeapon();
                        if ( LegitScan::CanShoot( GetCL_Players()->GetLocalPlayerPawn() , activeWeapon ) )
                            GetCL_Bypass()->SetAttack( cmd , true );
                        else
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
        }
        else if ( config.autoShoot )
        {
            GetCL_Bypass()->SetDontAttack( cmd , true );
        }

        if ( localPawn )
        {
            uint32_t localHandle = 0;
            const uintptr_t localCtrl = Game::Read<uintptr_t>( Game::clientBase + Offsets::dwLocalPlayerController );
            if ( localCtrl )
                localHandle = Game::Read<uint32_t>( localCtrl + Offsets::m_hPlayerPawn );

            Triggerbot::Tick( cmd , localPawn , localHandle , viewAngles );
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
