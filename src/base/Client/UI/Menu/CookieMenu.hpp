#pragma once
#include <ImGui/imgui.h>
#include "SkinChanger.hpp"
#include "Esp.hpp"
#include "Aimbot.hpp"
#include "AntiAim.hpp"
#include "NoSpread.hpp"
#include "Bhop.hpp"
#include "ThirdPerson.hpp"
#include "Triggerbot.hpp"
#include "TextureOverride.hpp"
#include "PaintKits.hpp"
#include "Chams.hpp"
#include "Ragebot.hpp"
#include "LegitBot.hpp"
#include "Movement.hpp"
#include "PlantBomb.hpp"
#include "Gloves.hpp"
#include "PaintKits.hpp"
#include "MenuEffects.hpp"
#include "EspOverlay.hpp"
#include "Tracers.hpp"
#include "WorldFov.hpp"
#include "WorldVisuals.hpp"
#include "WeaponConfig.hpp"
#include "Trace.hpp"
#include "HitboxData.hpp"
#include "LagComp.hpp"
#include "MenuAssets.hpp"
#include "MenuConfig.hpp"
#include <Client/Fonts/EmbeddedFonts.hpp>
#include <CS2/Hook/Hook_DrawObject.hpp>
#include <Client/Settings/CSettingsJson.hpp>
#include <Client/UI/Menu/MenuSettings.hpp>
#include <CS2/Hook/Hook_SetViewModelFov.hpp>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <string>

#include <GameClient/CL_ItemDefinition.hpp>
#include "CookieIcon.hpp"
#include "KeyBind.hpp"

namespace Menu
{
    inline int selectedTab = 0;
    inline int selectedWeaponIdx = 0;
    inline char searchBuffer[128] = "";

    // Weapons list for the combo box
    struct WeaponEntry
    {
        int defIndex;
        const char* name;
    };

    inline WeaponEntry weaponList[] = {
        { 7,  "AK-47" },
        { 8,  "AUG" },
        { 9,  "AWP" },
        { 1,  "Desert Eagle" },
        { 2,  "Dual Berettas" },
        { 10, "FAMAS" },
        { 3,  "Five-SeveN" },
        { 13, "Galil AR" },
        { 11, "G3SG1" },
        { 4,  "Glock-18" },
        { 14, "M249" },
        { 16, "M4A4" },
        { 60, "M4A1-S" },
        { 17, "MAC-10" },
        { 27, "MAG-7" },
        { 33, "MP7" },
        { 34, "MP9" },
        { 23, "MP5-SD" },
        { 28, "Negev" },
        { 35, "Nova" },
        { 19, "P90" },
        { 32, "P2000" },
        { 36, "P250" },
        { 26, "PP-Bizon" },
        { 64, "R8 Revolver" },
        { 29, "Sawed-Off" },
        { 38, "SCAR-20" },
        { 39, "SG 553" },
        { 40, "SSG 08" },
        { 30, "Tec-9" },
        { 24, "UMP-45" },
        { 61, "USP-S" },
        { 25, "XM1014" },
        { 63, "CZ75-Auto" },
        // Knives
        { 500, "Bayonet" },
        { 505, "Flip Knife" },
        { 506, "Gut Knife" },
        { 507, "Karambit" },
        { 508, "M9 Bayonet" },
        { 509, "Huntsman Knife" },
        { 512, "Falchion Knife" },
        { 514, "Bowie Knife" },
        { 515, "Butterfly Knife" },
        { 516, "Shadow Daggers" },
        { 517, "Paracord Knife" },
        { 519, "Ursus Knife" },
        { 520, "Navaja Knife" },
        { 521, "Nomad Knife" },
        { 522, "Stiletto Knife" },
        { 523, "Talon Knife" },
        { 525, "Skeleton Knife" },
        { 526, "Kukri Knife" },
    };
    inline constexpr int weaponListCount = sizeof(weaponList) / sizeof(weaponList[0]);

    inline SkinChanger::SkinConfig& GetSkinConfig(int defIndex)
    {
        return SkinChanger::weaponSkins[defIndex];
    }
}

#include <d3d11.h>

namespace Menu
{
    inline void ApplyGrayTheme()
    {
        ImGuiStyle& style = ImGui::GetStyle();
        style.Colors[ImGuiCol_Text]                  = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
        style.Colors[ImGuiCol_TextDisabled]          = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.12f, 0.12f, 0.12f, 0.94f);
        style.Colors[ImGuiCol_ChildBg]               = ImVec4(0.15f, 0.15f, 0.15f, 0.40f);
        style.Colors[ImGuiCol_PopupBg]               = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
        style.Colors[ImGuiCol_Border]                = ImVec4(0.30f, 0.30f, 0.30f, 0.50f);
        style.Colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        style.Colors[ImGuiCol_FrameBg]               = ImVec4(0.20f, 0.20f, 0.20f, 0.54f);
        style.Colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.30f, 0.30f, 0.30f, 0.40f);
        style.Colors[ImGuiCol_FrameBgActive]         = ImVec4(0.40f, 0.40f, 0.40f, 0.67f);
        style.Colors[ImGuiCol_TitleBg]               = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
        style.Colors[ImGuiCol_TitleBgActive]         = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
        style.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
        style.Colors[ImGuiCol_MenuBarBg]             = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
        style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
        style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
        style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
        style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        style.Colors[ImGuiCol_Button]                = ImVec4(0.25f, 0.25f, 0.25f, 0.40f);
        style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
        style.Colors[ImGuiCol_ButtonActive]          = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
        style.Colors[ImGuiCol_Header]                = ImVec4(0.30f, 0.30f, 0.30f, 0.31f);
        style.Colors[ImGuiCol_HeaderHovered]         = ImVec4(0.40f, 0.40f, 0.40f, 0.80f);
        style.Colors[ImGuiCol_HeaderActive]          = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        style.Colors[ImGuiCol_Separator]             = ImVec4(0.35f, 0.35f, 0.35f, 0.50f);
        style.Colors[ImGuiCol_SeparatorHovered]      = ImVec4(0.45f, 0.45f, 0.45f, 0.78f);
        style.Colors[ImGuiCol_SeparatorActive]       = ImVec4(0.55f, 0.55f, 0.55f, 1.00f);
        style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(0.20f, 0.20f, 0.20f, 0.20f);
        style.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.30f, 0.30f, 0.30f, 0.67f);
        style.Colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.40f, 0.40f, 0.40f, 0.95f);
        style.Colors[ImGuiCol_Tab]                   = ImVec4(0.18f, 0.18f, 0.18f, 0.86f);
        style.Colors[ImGuiCol_TabHovered]            = ImVec4(0.28f, 0.28f, 0.28f, 0.80f);
        style.Colors[ImGuiCol_TabActive]             = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
        style.Colors[ImGuiCol_TabUnfocused]          = ImVec4(0.11f, 0.11f, 0.11f, 0.97f);
        style.Colors[ImGuiCol_TabUnfocusedActive]    = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);

        style.WindowRounding = 6.f;
        style.ChildRounding = 4.f;
        style.FrameRounding = 4.f;
        style.PopupRounding = 4.f;
        style.ScrollbarRounding = 4.f;
        style.GrabRounding = 3.f;
        style.TabRounding = 4.f;

        const ImVec4 accent(0.92f, 0.72f, 0.18f, 1.f);
        const ImVec4 accentDim(0.45f, 0.36f, 0.10f, 1.f);
        style.Colors[ImGuiCol_TabActive] = accentDim;
        style.Colors[ImGuiCol_TabHovered] = ImVec4(0.35f, 0.30f, 0.12f, 0.90f);
        style.Colors[ImGuiCol_CheckMark] = accent;
        style.Colors[ImGuiCol_SliderGrab] = accent;
        style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(1.f, 0.82f, 0.35f, 1.f);
        style.Colors[ImGuiCol_Header] = ImVec4(0.28f, 0.24f, 0.10f, 0.55f);
        style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.38f, 0.32f, 0.12f, 0.80f);
        style.Colors[ImGuiCol_HeaderActive] = accentDim;
        style.ItemSpacing = ImVec2(8.f, 6.f);
        style.FramePadding = ImVec2(6.f, 4.f);
    }

    inline void DrawRageWeaponPanel()
    {
        ImGui::Separator();
        ImGui::Text("Per-Weapon Rage");
        ImGui::Checkbox("Use Per-Weapon Settings##Rage", &Ragebot::config.usePerWeapon);
        if ( !Ragebot::config.usePerWeapon )
            return;

        ImGui::Combo(
            "Weapon Class##Rage" ,
            &Ragebot::config.perWeaponClass ,
            WeaponConfig::kWeaponClassNames ,
            WeaponConfig::kWeaponClassCount );
        WeaponConfig::RageSettings& ws = WeaponConfig::GetRageSettings(
            static_cast<WeaponConfig::WeaponClass>( Ragebot::config.perWeaponClass ) );

        ImGui::SliderInt("Class Min Damage", &ws.minDamage, 1, 100);
        ImGui::SliderInt("Class Hitchance", &ws.hitchance, 0, 100, "%d%%");
        const char* classTargetModes[] = { "Highest Damage", "Lowest FOV", "Lowest Distance" };
        ImGui::Combo("Class Target Select", &ws.targetSelect, classTargetModes, IM_ARRAYSIZE(classTargetModes));
        ImGui::Checkbox("Class Auto Stop", &ws.autoStop);
        ImGui::Checkbox("Class Early Auto Stop", &ws.earlyAutoStop);
        ImGui::Checkbox("Class Penetration", &ws.penetration);
        ImGui::Checkbox("Class Safe Point", &ws.safePoint);
        ImGui::Checkbox("Class Rapid Fire", &ws.rapidFire);
        ImGui::Checkbox("Class Auto Scope", &ws.autoScope);
        ImGui::Checkbox("Class Remove Spread", &ws.removeSpread);
        ImGui::Checkbox("Head##RageClassHB", &ws.hitboxHead);
        ImGui::SameLine();
        ImGui::Checkbox("Neck##RageClassHB", &ws.hitboxNeck);
        ImGui::Checkbox("Chest##RageClassHB", &ws.hitboxChest);
        ImGui::SameLine();
        ImGui::Checkbox("Upper Chest##RageClassHB", &ws.hitboxUpperChest);
        ImGui::Checkbox("Pelvis##RageClassHB", &ws.hitboxPelvis);
        ImGui::SameLine();
        ImGui::Checkbox("Stomach##RageClassHB", &ws.hitboxStomach);
        ImGui::Checkbox("Arms##RageClassHB", &ws.hitboxArms);
        ImGui::SameLine();
        ImGui::Checkbox("Legs##RageClassHB", &ws.hitboxLegs);
        ImGui::SameLine();
        ImGui::Checkbox("Feet##RageClassHB", &ws.hitboxFeet);
    }
}

namespace CookieUI
{
    extern ID3D11Device* pDevice;

    inline void RenderMenu()
    {
        using namespace Menu;
        using namespace SkinChanger;

        ApplyGrayTheme();

        ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_Always);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        
        ImGui::Begin("##Cookie", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

        ImVec2 winPos = ImGui::GetWindowPos();
        ImVec2 winSize = ImGui::GetWindowSize();
        ImDrawList* drawList = ImGui::GetWindowDrawList();

        const float titleBarHeight = ImGui::GetFrameHeight();
        const float iconSize = 16.0f;
        const ImU32 titleColor = ImGui::GetColorU32(ImGuiCol_Text);
        const ImVec2 iconPos(
            winPos.x + 10.0f,
            winPos.y + (titleBarHeight - iconSize) * 0.5f);
        CookieIcon::DrawLucideCookie(drawList, iconPos, iconSize, titleColor, 1.5f);
        if ( ImFont* lexend = EmbeddedFonts::GetLexendBold() )
            ImGui::PushFont( lexend );
        drawList->AddText(
            ImVec2(iconPos.x + iconSize + 6.0f, winPos.y + (titleBarHeight - ImGui::GetFontSize()) * 0.5f),
            titleColor,
            "Cookie v1.4");
        if ( EmbeddedFonts::GetLexendBold() )
            ImGui::PopFont();

        ImGui::SetCursorPos(ImVec2(15, 30));
        
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
        ImGui::BeginChild("MainArea", ImVec2(winSize.x - 30, winSize.y - 45), false);

        MenuEffects::RenderMenuBackground( winSize );

        if (ImGui::BeginTabBar("MainTabs", ImGuiTabBarFlags_NoTooltip))
        {
            if (ImGui::BeginTabItem("Rage"))
            {
                ImGui::Spacing();
                if ( ImGui::Checkbox( "Enable Ragebot" , &Ragebot::config.enabled ) )
                {
                    if ( Ragebot::config.enabled )
                    {
                        Aimbot::config.enabled = false;
                        Triggerbot::config.enabled = false;
                        LegitBot::config.enabled = false;
                    }
                }

                const char* hitboxModes[] = { "V1 Bones" , "V2 Native (HitboxToWorldTransforms)" };
                ImGui::Combo( "Hitbox API" , reinterpret_cast<int*>( &HitboxData::config.mode ) , hitboxModes , IM_ARRAYSIZE( hitboxModes ) );
                if ( ImGui::IsItemHovered() )
                    ImGui::SetTooltip( "V1: bone/skeleton path (default).\nV2: native GetHitboxFlags / GetHitboxParent / HitboxToWorldTransforms." );
                if ( HitboxData::config.mode == HitboxData::Mode::V2_Native && !HitboxData::IsNativeReady() )
                    ImGui::TextColored( ImVec4( 1.f , 0.45f , 0.2f , 1.f ) , "V2 native patterns missing — using V1 fallback." );
                else if ( HitboxData::GetActiveMode() == HitboxData::Mode::V2_Native )
                    ImGui::TextDisabled( "Active: V2 Native" );

                if (Ragebot::config.enabled)
                {
                    ImGui::SliderInt("Min Damage", &Ragebot::config.minDamage, 1, 100);
                    ImGui::SliderInt("Hitchance", &Ragebot::config.hitchance, 0, 100, "%d%%");
                    ImGui::SliderInt("Multipoint Scale", &Ragebot::config.multipointScale, 0, 100, "%d%%");
                    ImGui::Checkbox("Auto Stop", &Ragebot::config.autoStop);
                    ImGui::Checkbox("Early Auto Stop", &Ragebot::config.earlyAutoStop);
                    ImGui::Checkbox("Penetration", &Ragebot::config.penetration);
                    ImGui::Checkbox("Safe Point", &Ragebot::config.safePoint);
                    ImGui::Checkbox("Adaptive Weapon", &Ragebot::config.adaptiveWeapon);
                    ImGui::Checkbox("Auto Shoot##Rage", &Ragebot::config.autoShoot);
                    ImGui::Checkbox("Silent Aim##Rage", &Ragebot::config.silentAim);
                    if ( ImGui::IsItemHovered() )
                        ImGui::SetTooltip("Off = server-visible aim (online). On = input-history only (local/practice).");
                    ImGui::Checkbox("No Spread##Rage", &NoSpread::config.enabled);
                    if ( ImGui::IsItemHovered() )
                        ImGui::SetTooltip( "Compensate spread/inaccuracy while moving." );
                    ImGui::Checkbox("Rapid Fire", &Ragebot::config.rapidFire);
                    ImGui::Checkbox("Auto Scope", &Ragebot::config.autoScope);
                    ImGui::Checkbox("Delay Aim", &Ragebot::config.delayAim);
                    if ( Ragebot::config.delayAim )
                        ImGui::SliderInt("Delay Aim (ms)", &Ragebot::config.delayAimMs, 0, 500);
                    ImGui::Checkbox("Backtrack##Rage", &Ragebot::config.backtrack);
                    ImGui::Checkbox("Backtrack Debug", &LagComp::debugConfig.drawBacktrack);
                    if ( LagComp::debugConfig.drawBacktrack )
                        ImGui::ColorEdit4("Backtrack Color", LagComp::debugConfig.color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
                    ImGui::Checkbox("Team Check##Rage", &Ragebot::config.teamCheck);

                    const char* hitscanModes[] = { "Normal", "Lethal", "Lethal+", "Prefer Lethal" };
                    ImGui::Combo("Hitscan Mode", reinterpret_cast<int*>(&Ragebot::config.hitscanMode), hitscanModes, IM_ARRAYSIZE(hitscanModes));

                    const char* scanModes[] = { "Single Point", "Adaptive Multipoint" };
                    ImGui::Combo("Scan Mode", reinterpret_cast<int*>(&Ragebot::config.scanMode), scanModes, IM_ARRAYSIZE(scanModes));

                    const char* stopModes[] = { "Slow (Counter-Strafe)", "Early (Hard Stop)" };
                    ImGui::Combo("Stop Mode", reinterpret_cast<int*>(&Ragebot::config.stopMode), stopModes, IM_ARRAYSIZE(stopModes));

                    const char* targetModes[] = { "Highest Damage", "Lowest FOV", "Lowest Distance" };
                    ImGui::Combo("Target Select", reinterpret_cast<int*>(&Ragebot::config.targetSelect), targetModes, IM_ARRAYSIZE(targetModes));

                    ImGui::Checkbox("Head", &Ragebot::config.hitboxHead);
                    ImGui::SameLine();
                    ImGui::Checkbox("Neck", &Ragebot::config.hitboxNeck);
                    ImGui::Checkbox("Chest", &Ragebot::config.hitboxChest);
                    ImGui::SameLine();
                    ImGui::Checkbox("Upper Chest", &Ragebot::config.hitboxUpperChest);
                    ImGui::Checkbox("Pelvis", &Ragebot::config.hitboxPelvis);
                    ImGui::SameLine();
                    ImGui::Checkbox("Stomach", &Ragebot::config.hitboxStomach);
                    ImGui::Checkbox("Arms", &Ragebot::config.hitboxArms);
                    ImGui::SameLine();
                    ImGui::Checkbox("Legs", &Ragebot::config.hitboxLegs);
                    ImGui::SameLine();
                    ImGui::Checkbox("Feet", &Ragebot::config.hitboxFeet);

                    DrawRageWeaponPanel();
                }

                ImGui::Separator();
                ImGui::Spacing();
                if ( Ragebot::config.enabled )
                    ImGui::TextDisabled( "Legit Aimbot / Triggerbot disabled while Ragebot is active." );

                ImGui::BeginDisabled( Ragebot::config.enabled );
                ImGui::Columns(2, "rageAimbotLayout", false);
                
                // Left Column
                ImGui::BeginChild("AimbotLeft", ImVec2(0, 0), true);
                ImGui::Text("Main Aimbot");
                ImGui::Separator();
                ImGui::Spacing();
                
                if ( ImGui::Checkbox( "Enable Aimbot" , &Aimbot::config.enabled ) && Aimbot::config.enabled )
                    Ragebot::config.enabled = false;
                LegitBot::SyncFromAimbot();

                KeyBindWidget( "Aim Key" , &Aimbot::config.aimKey );
                
                ImGui::Checkbox("Auto Shoot", &Aimbot::config.autoShoot);
                if ( ImGui::IsItemDeactivatedAfterEdit() )
                    Aimbot::menuBlockFrames = 24;

                ImGui::Checkbox("Silent Aim", &Aimbot::config.silentAim);
                ImGui::Checkbox("No Spread", &NoSpread::config.enabled);
                if ( ImGui::IsItemHovered() )
                    ImGui::SetTooltip( "Compensate spread/inaccuracy while moving. Keep Silent Aim on." );
                ImGui::Checkbox("Recoil Control", &Aimbot::config.recoilControl);
                if ( ImGui::IsItemHovered() )
                    ImGui::SetTooltip( "Compensate aim punch (2x) while spraying." );
                ImGui::SliderInt("Smooth", &Aimbot::config.smooth, 0, 20);
                ImGui::Checkbox("Punch Randomization", &Aimbot::config.punchRandomization);
                if ( Aimbot::config.punchRandomization )
                {
                    ImGui::SliderFloat("Punch Rand X", &Aimbot::config.punchRandomX, 0.f, 1.f, "%.2f");
                    ImGui::SliderFloat("Punch Rand Y", &Aimbot::config.punchRandomY, 0.f, 1.f, "%.2f");
                }
                ImGui::Checkbox("FOV Visualize", &Aimbot::config.fovVisualize);
                if ( Aimbot::config.fovVisualize )
                    ImGui::ColorEdit4("FOV Color", Aimbot::config.fovVisualizeColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
                const char* targetModes[] = { "FOV + Distance", "Crosshair", "Distance" };
                ImGui::Combo("Target Select", &Aimbot::config.targetSelection, targetModes, IM_ARRAYSIZE(targetModes));
                ImGui::Checkbox("Head##LegitHB", &Aimbot::config.hitboxHead);
                ImGui::SameLine();
                ImGui::Checkbox("Neck##LegitHB", &Aimbot::config.hitboxNeck);
                ImGui::Checkbox("Chest##LegitHB", &Aimbot::config.hitboxChest);
                ImGui::SameLine();
                ImGui::Checkbox("Upper Chest##LegitHB", &Aimbot::config.hitboxUpperChest);
                ImGui::Checkbox("Stomach##LegitHB", &Aimbot::config.hitboxStomach);
                ImGui::SameLine();
                ImGui::Checkbox("Pelvis##LegitHB", &Aimbot::config.hitboxPelvis);
                ImGui::Checkbox("Legs##LegitHB", &Aimbot::config.hitboxLegs);
                ImGui::SameLine();
                ImGui::Checkbox("Arms##LegitHB", &Aimbot::config.hitboxArms);
                ImGui::Checkbox("No Scope Silent", &LegitBot::config.noScope);
                ImGui::Checkbox("Delay Aim", &LegitBot::config.delayAim);
                if ( LegitBot::config.delayAim )
                    ImGui::SliderInt("Delay Aim (ms)", &LegitBot::config.delayAimMs, 0, 500);
                unsigned int conditions = LegitBot::config.conditions;
                bool inAir = ( conditions & static_cast<unsigned int>( LegitBot::Condition::InAir ) ) != 0;
                bool flashed = ( conditions & static_cast<unsigned int>( LegitBot::Condition::Flashed ) ) != 0;
                bool inSmoke = ( conditions & static_cast<unsigned int>( LegitBot::Condition::InSmoke ) ) != 0;
                bool delayShot = ( conditions & static_cast<unsigned int>( LegitBot::Condition::DelayShot ) ) != 0;
                if ( ImGui::Checkbox( "Only On Ground##LegitCond" , &inAir ) )
                {
                    conditions = inAir
                        ? ( conditions | static_cast<unsigned int>( LegitBot::Condition::InAir ) )
                        : ( conditions & ~static_cast<unsigned int>( LegitBot::Condition::InAir ) );
                }
                if ( ImGui::Checkbox( "Not Flashed##LegitCond" , &flashed ) )
                {
                    conditions = flashed
                        ? ( conditions | static_cast<unsigned int>( LegitBot::Condition::Flashed ) )
                        : ( conditions & ~static_cast<unsigned int>( LegitBot::Condition::Flashed ) );
                }
                if ( ImGui::Checkbox( "Not In Smoke##LegitCond" , &inSmoke ) )
                {
                    conditions = inSmoke
                        ? ( conditions | static_cast<unsigned int>( LegitBot::Condition::InSmoke ) )
                        : ( conditions & ~static_cast<unsigned int>( LegitBot::Condition::InSmoke ) );
                }
                if ( ImGui::Checkbox( "Delay After Kill##LegitCond" , &delayShot ) )
                {
                    conditions = delayShot
                        ? ( conditions | static_cast<unsigned int>( LegitBot::Condition::DelayShot ) )
                        : ( conditions & ~static_cast<unsigned int>( LegitBot::Condition::DelayShot ) );
                }
                LegitBot::config.conditions = conditions;
                ImGui::Checkbox("Team Check", &Aimbot::config.teamCheck);
                ImGui::Checkbox("Visible Only (TraceShape)", &Aimbot::config.visCheck);
                ImGui::Checkbox("Penetration (AutoWall)", &Aimbot::config.penetration);
                if ( ImGui::IsItemDeactivatedAfterEdit() )
                    Aimbot::menuBlockFrames = 24;
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Line-of-sight via client TraceShape. No m_bSpotted / map parsing.");

                ImGui::EndChild();

                ImGui::NextColumn();
                
                // Right Column
                ImGui::BeginChild("AimbotRight", ImVec2(0, 0), true);
                ImGui::Text("Configuration");
                ImGui::Separator();
                ImGui::Spacing();
                
                const char* fovTypes[] = { "Angle (Degrees)", "Screen (Pixels)" };
                ImGui::Combo("FOV Mode", &Aimbot::config.fovType, fovTypes, IM_ARRAYSIZE(fovTypes));
                if (Aimbot::config.fovType == 0)
                    ImGui::SliderFloat("FOV", &Aimbot::config.fov, 1.0f, 180.0f, "%.1f deg");
                else
                    ImGui::SliderFloat("FOV", &Aimbot::config.screenFov, 10.0f, 1000.0f, "%.0f px");
                
                const char* bones[] = { "Head", "Neck", "Chest", "Pelvis" };
                if ( ImGui::Combo( "Hitbox" , &Aimbot::config.targetHitbox , bones , IM_ARRAYSIZE( bones ) ) )
                {
                }
                
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Text("Triggerbot");
                ImGui::Spacing();
                
                ImGui::Checkbox("Enable Triggerbot", &Triggerbot::config.enabled);

                KeyBindWidget( "Trigger Key" , &Triggerbot::config.key , &Triggerbot::config.useKey );

                ImGui::Checkbox("Team Check (Trigger)", &Triggerbot::config.teamCheck);
                ImGui::Checkbox("Visible Only (Trigger)", &Triggerbot::config.visCheck);
                ImGui::SliderInt("Trigger Hitchance", &Triggerbot::config.hitchance, 0, 100, "%d%%");
                ImGui::SliderInt("Delay (ms)", &Triggerbot::config.delayMs, 0, 200);

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Text("Per-Weapon Legit");
                ImGui::Checkbox("Use Per-Weapon Settings", &LegitBot::config.usePerWeapon);
                if ( LegitBot::config.usePerWeapon )
                {
                    ImGui::Combo(
                        "Weapon Class" ,
                        &LegitBot::config.perWeaponClass ,
                        WeaponConfig::kWeaponClassNames ,
                        WeaponConfig::kWeaponClassCount );
                    WeaponConfig::LegitSettings& ws = WeaponConfig::GetLegitSettings(
                        static_cast<WeaponConfig::WeaponClass>( LegitBot::config.perWeaponClass ) );
                    ImGui::SliderInt("Class FOV", &ws.fov, 1, 30);
                    ImGui::SliderInt("Class Smooth", &ws.smooth, 0, 20);
                    const char* classTargetModes[] = { "FOV + Distance", "Crosshair", "Distance" };
                    ImGui::Combo("Class Target Select", &ws.targetSelection, classTargetModes, IM_ARRAYSIZE(classTargetModes));
                    ImGui::Checkbox("Class RCS", &ws.rcs);
                    ImGui::Checkbox("Class Punch Random", &ws.punchRandomization);
                    if ( ws.punchRandomization )
                    {
                        ImGui::SliderFloat("Class Punch X", &ws.punchRandomX, 0.f, 1.f, "%.2f");
                        ImGui::SliderFloat("Class Punch Y", &ws.punchRandomY, 0.f, 1.f, "%.2f");
                    }
                    ImGui::SliderFloat("Class RCS Smooth X", &ws.rcsSmoothX, 0.f, 10.f, "%.1f");
                    ImGui::SliderFloat("Class RCS Smooth Y", &ws.rcsSmoothY, 0.f, 10.f, "%.1f");
                    ImGui::Checkbox("Class Trigger Override", &ws.triggerEnabled);
                    if ( ws.triggerEnabled )
                        ImGui::SliderInt("Class Trigger HC", &ws.triggerHitchance, 0, 100, "%d%%");
                    ImGui::Checkbox("Head##ClassHB", &ws.hitboxHead);
                    ImGui::SameLine();
                    ImGui::Checkbox("Neck##ClassHB", &ws.hitboxNeck);
                    ImGui::Checkbox("Chest##ClassHB", &ws.hitboxChest);
                    ImGui::SameLine();
                    ImGui::Checkbox("Upper Chest##ClassHB", &ws.hitboxUpperChest);
                    ImGui::Checkbox("Stomach##ClassHB", &ws.hitboxStomach);
                    ImGui::SameLine();
                    ImGui::Checkbox("Pelvis##ClassHB", &ws.hitboxPelvis);
                    ImGui::Checkbox("Legs##ClassHB", &ws.hitboxLegs);
                    ImGui::SameLine();
                    ImGui::Checkbox("Arms##ClassHB", &ws.hitboxArms);
                }

                ImGui::EndChild();
                
                ImGui::Columns(1);
                ImGui::EndDisabled();
                LegitBot::SyncFromAimbot();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Visuals"))
            {
                ImGui::Spacing();
                ImGui::Columns(2, "VisualsLayout", false);

                ImGui::BeginChild("VisualsMain", ImVec2(0, 0), true);
                ImGui::Text("ESP");
                ImGui::Separator();
                ImGui::Spacing();
                
                ImGui::Checkbox("Enable ESP", &ESP::config.enabled);
                if (ESP::config.enabled)
                {
                    ImGui::Checkbox("Box", &ESP::config.bBox);
                    if (ESP::config.bBox)
                    {
                        const char* modes[] = { "Normal", "Corners" };
                        ImGui::Combo("Box Style", &ESP::config.boxMode, modes, IM_ARRAYSIZE(modes));
                    }
                    ImGui::ColorEdit4("Box Color", ESP::config.boxColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
                    ImGui::Checkbox("Skeleton", &ESP::config.bSkeleton);
                    ImGui::ColorEdit4("Skeleton Color", ESP::config.skeletonColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
                    ImGui::Checkbox("Health Bar", &ESP::config.bHealthBar);
                    ImGui::Checkbox("Name", &ESP::config.bName);
                    ImGui::Checkbox("Distance", &ESP::config.bDistance);
                    ImGui::Checkbox("Team Check", &ESP::config.teamCheck);
                    ImGui::SliderFloat("Max Distance", &ESP::config.maxDistance, 100.f, 10000.f, "%.0f");
                    ImGui::Spacing();
                }

                ImGui::Separator();
                ImGui::Text("ESP Overlay");
                ImGui::Checkbox("Enable Overlay", &EspOverlay::config.enabled);
                if (EspOverlay::config.enabled)
                {
                    ImGui::Checkbox("Use Components", &EspOverlay::config.useComponents);
                    ImGui::Checkbox("Box", &EspOverlay::config.showBox);
                    ImGui::Checkbox("Health Bar", &EspOverlay::config.showHealthBar);
                    ImGui::Checkbox("Name", &EspOverlay::config.showName);
                    ImGui::Checkbox("Distance", &EspOverlay::config.showDistance);
                    ImGui::Checkbox("Weapon Icon", &EspOverlay::config.showWeaponIcon);
                    ImGui::Checkbox("Ammo Bar", &EspOverlay::config.showAmmoBar);
                    ImGui::Checkbox("Helmet Flag (HK)", &EspOverlay::config.showHelmetFlag);
                    ImGui::Checkbox("Defuser Flag (KIT)", &EspOverlay::config.showKitFlag);
                    ImGui::ColorEdit4("Box Color", EspOverlay::config.boxColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
                    ImGui::ColorEdit4("Health Bar Color", EspOverlay::config.healthBarColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
                    ImGui::ColorEdit4("Ammo Bar Color", EspOverlay::config.ammoBarColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
                    ImGui::ColorEdit4("Text Color", EspOverlay::config.textColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
                    ImGui::SliderFloat("Box Thickness", &EspOverlay::config.boxThickness, 1.f, 4.f, "%.1f");
                    ImGui::SliderFloat("Bar Thickness", &EspOverlay::config.barThickness, 1.f, 6.f, "%.1f");
                    ImGui::Spacing();
                }
                ImGui::Separator();
                ImGui::Text("Movement & Misc");
                ImGui::Checkbox("Plant Bomb Anywhere", &PlantBomb::config.enabled);
                if ( PlantBomb::config.enabled )
                    ImGui::TextDisabled("Client-side only; server may still reject off-site plants.");
                ImGui::Checkbox("Bunny Hop", &Bhop::config.enabled);
                ImGui::Checkbox("Edge Jump", &Bhop::config.edgeJump);
                ImGui::Checkbox("Jump Bug", &Bhop::config.jumpBug);
                ImGui::Checkbox("Hold Space (Bhop)", &Bhop::config.requireSpace);
                ImGui::Checkbox("Auto Forward", &Bhop::config.autoForward);
                ImGui::SliderInt("Hop Chance", &Bhop::config.hopChance, 0, 100, "%d%%");
                if ( ( Bhop::NeedsTrace() || Movement::config.edgeBug ) && !Trace::ready )
                    ImGui::TextColored( ImVec4( 1.f , 0.45f , 0.2f , 1.f ) , "Trace unavailable: Edge Jump / Jump Bug / Edge Bug disabled." );
                ImGui::Separator();
                ImGui::Text("World Visuals");
                ImGui::Checkbox("Night Mode", &WorldVisuals::config.nightMode);
                if ( WorldVisuals::config.nightMode )
                    ImGui::SliderFloat("Ambient Boost", &WorldVisuals::config.nightAmbient, 0.1f, 1.5f, "%.2f");
                ImGui::Checkbox("World Modulate (Fog)", &WorldVisuals::config.worldModulate);
                if ( WorldVisuals::config.worldModulate )
                    ImGui::ColorEdit4("Modulate Tint", WorldVisuals::config.modulateColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
                ImGui::Checkbox("No Shadows", &WorldVisuals::config.noShadow);
                ImGui::Separator();
                ImGui::Checkbox("World FOV Override", &WorldFov::config.enabled);
                if ( WorldFov::config.enabled )
                    ImGui::SliderFloat("World FOV", &WorldFov::config.amount, 60.f, 140.f, "%.0f");
                ImGui::Checkbox("ViewModel FOV Override", &g_viewModelFovConfig.enabled);
                if ( g_viewModelFovConfig.enabled )
                    ImGui::SliderFloat("ViewModel FOV", &g_viewModelFovConfig.amount, 54.f, 90.f, "%.0f");
                ImGui::Checkbox("Bullet Tracers", &BulletTracer::config.enabled);
                if ( BulletTracer::config.enabled )
                {
                    ImGui::SliderFloat("Trail Life", &BulletTracer::config.trailLife, 0.5f, 5.f, "%.1fs");
                    ImGui::SliderFloat("Bullet Speed", &BulletTracer::config.bulletSpeed, 1000.f, 15000.f, "%.0f");
                    ImGui::SliderFloat("Thickness", &BulletTracer::config.thickness, 1.f, 6.f, "%.1f");
                    ImGui::SliderFloat("Ray Length", &BulletTracer::config.rayLength, 1000.f, 15000.f, "%.0f");
                }
                ImGui::Checkbox("Third Person Camera", &ThirdPerson::config.enabled);
                if ( ThirdPerson::config.enabled )
                {
                    ImGui::Checkbox("Smooth Camera", &ThirdPerson::config.smoothCamera);
                    if ( ThirdPerson::config.smoothCamera )
                    {
                        ImGui::SliderFloat( "Smooth Speed" , &ThirdPerson::config.smoothSpeed , 1.f , 20.f , "%.1f" );
                        if ( ImGui::Button( "Reset##ThirdPersonSmooth" ) )
                            ThirdPerson::config.smoothSpeed = ThirdPerson::Config::kSmoothSpeedDefault;
                    }
                    ImGui::SliderFloat( "Third Person Distance" , &ThirdPerson::config.distance , 40.f , 400.f , "%.0f" );
                    ImGui::SameLine();
                    if ( ImGui::Button( "Reset##ThirdPersonDist" ) )
                        ThirdPerson::config.distance = ThirdPerson::Config::kDistanceDefault;

                    ImGui::SliderInt( "Third Person FOV" , &ThirdPerson::config.fov , 60 , 140 , "%d" );
                    ImGui::SameLine();
                    if ( ImGui::Button( "Reset##ThirdPersonFov" ) )
                        ThirdPerson::config.fov = ThirdPerson::Config::kFovDefault;
                }
                ImGui::Checkbox( "Anti-Aim" , &AntiAim::config.enabled );
                if ( AntiAim::config.enabled )
                {
                    const char* aaModes[] = { "Static" , "Spin" };
                    ImGui::Combo( "Mode" , &AntiAim::config.mode , aaModes , IM_ARRAYSIZE( aaModes ) );

                    if ( AntiAim::config.mode == static_cast<int>( AntiAim::Mode::Static ) )
                    {
                        const char* pitchModes[] = { "Custom", "Down", "Up", "Zero" };
                        ImGui::Combo("Pitch Mode", &AntiAim::config.pitchType, pitchModes, IM_ARRAYSIZE(pitchModes));
                        if ( AntiAim::config.pitchType == static_cast<int>( AntiAim::PitchType::None ) )
                        {
                            ImGui::SliderFloat( "Pitch" , &AntiAim::config.pitch , -89.f , 89.f , "%.0f" );
                            ImGui::SameLine();
                            if ( ImGui::Button( "Reset##AAPitchStatic" ) )
                                AntiAim::config.pitch = AntiAim::kPitchDefault;
                        }

                        const char* yawModes[] = { "Custom Offset", "Backwards", "Forwards" };
                        ImGui::Combo("Yaw Mode", &AntiAim::config.yawType, yawModes, IM_ARRAYSIZE(yawModes));
                        if ( AntiAim::config.yawType == static_cast<int>( AntiAim::YawType::None ) )
                        {
                            ImGui::SliderFloat( "Yaw" , &AntiAim::config.yaw , -180.f , 180.f , "%.0f" );
                            ImGui::SameLine();
                            if ( ImGui::Button( "Reset##AAYawStatic" ) )
                                AntiAim::config.yaw = AntiAim::kStaticYawDefault;
                        }
                    }
                    else
                    {
                        ImGui::SliderFloat( "Pitch" , &AntiAim::config.pitch , -89.f , 89.f , "%.0f" );
                        ImGui::SliderFloat( "Spin Speed" , &AntiAim::config.spinSpeed , 1.f , 360.f , "%.0f" );
                        ImGui::SameLine();
                        if ( ImGui::Button( "Reset##AASpinSpeed" ) )
                            AntiAim::config.spinSpeed = AntiAim::kSpinSpeedDefault;
                    }
                }

                ImGui::Separator();
                ImGui::Spacing();
                ImGui::Text("Extras");
                ImGui::Checkbox("Bomb Timer", &ESP::config.bBombTimer);
                ImGui::Checkbox("Spectator List", &ESP::config.bSpectators);
                ImGui::Checkbox("Glow ESP", &ESP::config.bGlow);
                if (ESP::config.bGlow)
                {
                    ImGui::ColorEdit4("Glow Color", ESP::config.glowColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
                }

                ImGui::Separator();
                ImGui::Text("Chams (DrawObject)");
                const bool chamsAvailable = DrawObject_o != nullptr && Chams::IsReady();
                if ( !chamsAvailable )
                {
                    ImGui::TextColored( ImVec4( 1.f , 0.45f , 0.35f , 1.f ) ,
                        "Chams unavailable (DrawObject hook or materials missing)" );
                }
                ImGui::BeginDisabled( !chamsAvailable );
                ImGui::Checkbox("Enable Chams", &Chams::config.enabled);
                ImGui::Checkbox("Enemy Chams", &Chams::config.enemy);
                ImGui::Checkbox("Local Chams", &Chams::config.local);
                ImGui::Checkbox("Teammate Chams", &Chams::config.teammate);
                ImGui::Checkbox("Weapon Chams", &Chams::config.weapon);
                ImGui::Checkbox("Hands / ViewModel", &Chams::config.hands);
                const char* chamsMaterials[] = { "Flat", "Glow", "White", "Default", "Illum" };
                ImGui::Combo("Chams Material", reinterpret_cast<int*>(&Chams::config.materialStyle), chamsMaterials, IM_ARRAYSIZE(chamsMaterials));
                ImGui::Checkbox("Ignore Z", &Chams::config.ignoreZ);
                ImGui::ColorEdit4("Chams Color", Chams::config.color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
                if ( Chams::config.ignoreZ )
                    ImGui::ColorEdit4("Ignore Z Color", Chams::config.ignoreZColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
                ImGui::EndDisabled();

                ImGui::Separator();
                ImGui::Text("Movement Advanced");
                ImGui::Checkbox("Movement Fix", &Movement::config.movementFix);
                ImGui::Checkbox("Movement Correction", &Movement::config.movementCorrection);
                ImGui::Checkbox("Validate Angles", &Movement::config.validateAngles);
                ImGui::Checkbox("Edge Bug", &Movement::config.edgeBug);
                if ( Movement::config.edgeBug )
                {
                    ImGui::Checkbox( "Edge Bug Use Key" , &Movement::config.edgeBugUseKey );
                    if ( Movement::config.edgeBugUseKey )
                        KeyBindWidget( "Edge Bug Key" , &Movement::config.edgeBugKey );
                }
                const char* strafeModes[] = { "Off", "Legit", "Rage" };
                ImGui::Combo("Auto Strafe Mode", reinterpret_cast<int*>(&Movement::config.strafeMode), strafeModes, IM_ARRAYSIZE(strafeModes));
                if ( Movement::config.strafeMode != Movement::StrafeMode::Off )
                {
                    ImGui::SliderFloat( "Strafe Smoothing" , &Movement::config.strafeSmooth , 0.f , 100.f , "%.0f" );
                    ImGui::Checkbox( "Strafe Assist (WASD)" , &Movement::config.strafeAssist );
                }

                ImGui::Separator();
                ImGui::Text("Menu Effects");
                ImGui::Checkbox("Watermark", &MenuEffects::config.watermark);
                ImGui::Checkbox("Particles", &MenuEffects::config.particles);
                ImGui::Checkbox("Menu Background Image", &MenuEffects::config.menuBackgroundImage);
                if ( MenuEffects::config.menuBackgroundImage )
                    ImGui::SliderFloat("Background Alpha", &MenuEffects::config.menuBackgroundAlpha, 0.f, 1.f, "%.2f");
                ImGui::Checkbox("Blur Placeholder", &MenuEffects::config.blurPlaceholder);
                ImGui::SliderInt("Max Particles", &MenuEffects::config.maxParticles, 20, 200);
                ImGui::SliderFloat("Particle Link Distance", &MenuEffects::config.particleLinkDistance, 40.f, 240.f, "%.0f");

                ImGui::EndChild();

                ImGui::NextColumn();
                ImGui::BeginChild("VisualsPreview", ImVec2(0, 0), true);
                ImGui::Text("ESP Preview");
                ImGui::Separator();
                const ImVec2 previewOrigin = ImGui::GetCursorScreenPos();
                const ImVec2 previewSize( ImGui::GetContentRegionAvail().x , ImGui::GetContentRegionAvail().y - 8.f );
                ImGui::Dummy( previewSize );
                if ( MenuAssets::GetEspPreviewTexture() )
                    MenuAssets::RenderEspPreviewPanel( ImGui::GetWindowDrawList() , previewOrigin , previewSize );
                else
                    ImGui::TextDisabled("Preview texture not loaded");
                ImGui::EndChild();

                ImGui::Columns(1);
                
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Skins"))
            {
                ImGui::Spacing();

                ImGui::Text("Gloves");
                ImGui::Checkbox("Enable Gloves", &Gloves::config.enabled);
                if ( Gloves::config.enabled )
                {
                    if ( ImGui::BeginCombo( "Glove Model" ,
                        Gloves::config.modelIndex >= 0 && Gloves::config.modelIndex < static_cast<int>( g_GlovesNames.size() )
                            ? g_GlovesNames[Gloves::config.modelIndex].m_pszName
                            : "Select Glove" ) )
                    {
                        for ( int i = 0; i < static_cast<int>( g_GlovesNames.size() ); ++i )
                        {
                            const bool selected = Gloves::config.modelIndex == i;
                            if ( ImGui::Selectable( g_GlovesNames[i].m_pszName , selected ) )
                                Gloves::config.modelIndex = i;
                            if ( selected )
                                ImGui::SetItemDefaultFocus();
                        }
                        ImGui::EndCombo();
                    }

                    ImGui::InputInt("Glove PaintKit", &Gloves::config.paintKit);
                    ImGui::SliderFloat("Glove Wear", &Gloves::config.wear, 0.0f, 1.0f, "%.4f");
                    ImGui::InputInt("Glove Seed", &Gloves::config.seed);

                    const char* teams[] = { "Terrorist (2)", "Counter-Terrorist (3)" };
                    int teamIdx = Gloves::config.team == 2 ? 0 : 1;
                    if ( ImGui::Combo( "Loadout Team" , &teamIdx , teams , IM_ARRAYSIZE( teams ) ) )
                        Gloves::config.team = teamIdx == 0 ? 2 : 3;

                    if ( ImGui::Button( "Apply Gloves" , ImVec2( -1 , 0 ) ) )
                        Gloves::ApplyFromConfig();
                }
                ImGui::Separator();
                ImGui::Spacing();
                
                // --- Knife Model Changer ---
                ImGui::Checkbox("Knife Changer", &SkinChanger::knifeChangerEnabled);
                if (SkinChanger::knifeChangerEnabled)
                {
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(200);
                    if (ImGui::BeginCombo("##knifemodel", SkinChanger::knifeModels[SkinChanger::selectedKnifeModel].name))
                    {
                        for (int i = 0; i < SkinChanger::knifeModelCount; i++)
                        {
                            bool selected = (SkinChanger::selectedKnifeModel == i);
                            if (ImGui::Selectable(SkinChanger::knifeModels[i].name, selected))
                            {
                                SkinChanger::selectedKnifeModel = i;
                                SkinChanger::lastAppliedWeapon = 0; // force re-apply
                                SkinChanger::forceUpdate.store(true);
                            }
                            if (selected) ImGui::SetItemDefaultFocus();
                        }
                        ImGui::EndCombo();
                    }
                }
                ImGui::Separator();
                ImGui::Spacing();
                
                ImGui::Columns(2, "skinsLayout", false);
                
                // Left Column
                ImGui::BeginChild("WeaponsLeft", ImVec2(0, 0), true);
                ImGui::Text("Inventory");
                ImGui::Separator();
                ImGui::Spacing();
                
                if (ImGui::Button("Force Update", ImVec2(-1, 0))) {
                    forceUpdate.store(true);
                }
                if (ImGui::Button("Clear All Skins", ImVec2(-1, 0))) {
                    std::lock_guard<std::mutex> lock(configMutex);
                    weaponSkins.clear();
                    forceUpdate.store(true);
                }
                if (ImGui::Button("Randomize All", ImVec2(-1, 0))) {
                    std::lock_guard<std::mutex> lock(configMutex);
                    if (!g_PaintKits.empty()) {
                        srand((unsigned int)time(NULL));
                        for (int i = 0; i < weaponListCount; i++) {
                            int defIdx = weaponList[i].defIndex;
                            SkinConfig& cfg = weaponSkins[defIdx];
                            cfg.enabled = true;
                            int randomIdx = rand() % g_PaintKits.size();
                            cfg.paintKit = g_PaintKits[randomIdx].id;
                            cfg.wear = 0.001f;
                            cfg.seed = 0;
                            cfg.statTrak = -1;
                        }
                        forceUpdate.store(true);
                    }
                }
                
                ImGui::Spacing();
                for (int i = 0; i < weaponListCount; i++)
                {
                    bool hasConfig = weaponSkins.count(weaponList[i].defIndex) > 0 && weaponSkins[weaponList[i].defIndex].enabled;
                    if (hasConfig) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.9f, 0.5f, 1.0f));
                    
                    if (ImGui::Selectable(weaponList[i].name, selectedWeaponIdx == i))
                        selectedWeaponIdx = i;
                        
                    if (hasConfig) ImGui::PopStyleColor();
                }
                ImGui::EndChild();
                
                ImGui::NextColumn();
                
                // Right Column
                ImGui::BeginChild("WeaponsRight", ImVec2(0, 0), true);
                if (selectedWeaponIdx >= 0 && selectedWeaponIdx < weaponListCount)
                {
                    int defIdx = weaponList[selectedWeaponIdx].defIndex;
                    SkinConfig& cfg = GetSkinConfig(defIdx);
                    
                    ImGui::Text("Configure %s", weaponList[selectedWeaponIdx].name);
                    ImGui::Separator();
                    ImGui::Spacing();
                    
                    ImGui::Checkbox("Enabled", &cfg.enabled);
                    
                    // Advanced Skin Selector
                    static int selectedRarity = 0; // 0 = All
                    static char skinSearchBuf[64] = "";
                    const char* rarityNames[] = { "All", "Consumer (Gray)", "Industrial (Light Blue)", "Mil-Spec (Blue)", "Restricted (Purple)", "Classified (Pink)", "Covert (Red)", "Contraband (Gold)" };
                    ImGui::Combo("Filter Rarity", &selectedRarity, rarityNames, IM_ARRAYSIZE(rarityNames));
                    ImGui::InputTextWithHint("##skinSearch", "Search skins...", skinSearchBuf, sizeof(skinSearchBuf));
                    
                    std::string previewName = "Custom ID: " + std::to_string(cfg.paintKit);
                    for (const auto& pk : g_PaintKits) {
                        if (pk.id == cfg.paintKit) {
                            previewName = pk.name;
                            break;
                        }
                    }
                    
                    if (ImGui::BeginCombo("Paint Kit", previewName.c_str())) {
                        bool customSelected = false;
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
                        ImGui::Selectable("--- Custom ID ---", customSelected);
                        ImGui::PopStyleColor();
                        
                        // Convert search to lowercase for case-insensitive matching
                        std::string searchLower = skinSearchBuf;
                        for (auto& c : searchLower) c = (char)tolower(c);
                        
                        for (const auto& pk : g_PaintKits) {
                            if (selectedRarity != 0 && (int)pk.rarity != selectedRarity - 1)
                                continue;
                            
                            // Filter by search text
                            if (searchLower.length() > 0) {
                                std::string nameLower = pk.name;
                                for (auto& c : nameLower) c = (char)tolower(c);
                                if (nameLower.find(searchLower) == std::string::npos)
                                    continue;
                            }
                                
                            bool isSelected = (cfg.paintKit == pk.id);
                            
                            // Show ID next to name for clarity
                            char label[128];
                            snprintf(label, sizeof(label), "[%d] %s", pk.id, pk.name);
                            
                            ImGui::PushStyleColor(ImGuiCol_Text, GetRarityColor(pk.rarity));
                            if (ImGui::Selectable(label, isSelected)) {
                                cfg.paintKit = pk.id;
                            }
                            ImGui::PopStyleColor();
                            if (isSelected) {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    }
                    
                    ImGui::InputInt("Custom Paint Kit ID", &cfg.paintKit);
                    if (cfg.paintKit < 0) cfg.paintKit = 0;
                    
                    ImGui::SliderFloat("Wear", &cfg.wear, 0.0f, 1.0f, "%.4f");
                    ImGui::InputInt("Seed", &cfg.seed);
                    
                    bool useStatTrak = cfg.statTrak >= 0;
                    if (ImGui::Checkbox("StatTrak", &useStatTrak)) cfg.statTrak = useStatTrak ? 0 : -1;
                    if (useStatTrak) {
                        ImGui::SameLine();
                        ImGui::SetNextItemWidth(100);
                        ImGui::InputInt("##statval", &cfg.statTrak);
                    }

                    // --- Custom Texture (Runtime Replacement) ---
                    ImGui::Spacing();
                    ImGui::Separator();
                    ImGui::Spacing();
                    if (ImGui::TreeNode("Custom Texture (Your Image on Weapon)"))
                    {
                        ImGui::Checkbox("Enable Custom Texture", &CustomTexture::enabled);
                        
                        ImGui::InputText("Image Path", CustomTexture::imagePath, sizeof(CustomTexture::imagePath));
                        
                        if (ImGui::Button("Load Image", ImVec2(-1, 0)))
                        {
                            if (pDevice)
                            {
                                if (CustomTexture::LoadCustomImage(pDevice, CustomTexture::imagePath))
                                    printf("[Menu] Custom image loaded!\n");
                                else
                                    printf("[Menu] Failed to load image\n");
                            }
                        }
                        
                        if (CustomTexture::imageLoaded)
                        {
                            ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "Image loaded!");
                        }
                        else
                        {
                            ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "No image loaded");
                        }
                        
                        ImGui::Spacing();
                        ImGui::Text("Texture Filter (0 = any)");
                        ImGui::InputInt("Width##filter", &CustomTexture::filterWidth, 256);
                        ImGui::InputInt("Height##filter", &CustomTexture::filterHeight, 256);
                        if (CustomTexture::filterWidth < 0) CustomTexture::filterWidth = 0;
                        if (CustomTexture::filterHeight < 0) CustomTexture::filterHeight = 0;
                        
                        ImGui::Spacing();
                        ImGui::Separator();
                        ImGui::Spacing();
                        
                        // Browse mode controls
                        if (!CustomTexture::browseMode)
                        {
                            if (ImGui::Button("Start Browse", ImVec2(-1, 0)))
                            {
                                CustomTexture::ResetBrowse();
                                CustomTexture::browseMode = true;
                            }
                        }
                        else
                        {
                            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.3f, 1.0f), 
                                "BROWSE MODE 鈥?Look at your weapon!");
                            ImGui::Text("Candidates found: %d", CustomTexture::candidateCount);
                            ImGui::Text("Current: %d / %d", 
                                CustomTexture::browseIndex + 1, CustomTexture::candidateCount);
                            
                            if (ImGui::Button("< Prev (F10)")) CustomTexture::BrowsePrev();
                            ImGui::SameLine();
                            if (ImGui::Button("Next > (F11)")) CustomTexture::BrowseNext();
                            
                            ImGui::Spacing();
                            if (ImGui::Button("LOCK This Texture (F12)", ImVec2(-1, 0)))
                                CustomTexture::LockCurrent();

                            if (ImGui::Button("Stop Browse", ImVec2(-1, 0)))
                                CustomTexture::browseMode = false;
                        }
                        
                        // Show locked status
                        if (CustomTexture::lockedSRV)
                        {
                            ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.3f, 1.0f), "LOCKED!");
                            if (ImGui::Button("Unlock", ImVec2(-1, 0)))
                                CustomTexture::Unlock();
                        }
                        
                        if (ImGui::Button("Reset All", ImVec2(-1, 0)))
                            CustomTexture::ResetBrowse();
                        
                        ImGui::Spacing();
                        ImGui::Separator();
                        ImGui::Spacing();
                        
                        // Debug info
                        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "Debug Info:");
                        ImGui::Text("Hook calls: %d", CustomTexture::hookCalls);
                        ImGui::Text("Unique SRVs seen: %d", CustomTexture::totalUniqueSRVs);
                        ImGui::Text("Hook active: %s", CustomTexture::hooked ? "YES" : "NO");
                        ImGui::Checkbox("No Filter (accept all)", &CustomTexture::noFilter);
                        ImGui::InputInt("Min Size##min", &CustomTexture::minSize, 32);
                        if (CustomTexture::minSize < 1) CustomTexture::minSize = 1;
                        
                        ImGui::Spacing();
                        ImGui::Separator();
                        ImGui::TextColored(ImVec4(0.7f, 0.85f, 1.0f, 1.0f), "How to use:");
                        ImGui::BulletText("Set image path and click Load");
                        ImGui::BulletText("Enable + click Start Browse");
                        ImGui::BulletText("Use F10/F11 to cycle textures");
                        ImGui::BulletText("When weapon shows your image: F12");
                        ImGui::BulletText("Keys: F10=prev F11=next F12=lock");
                        
                        ImGui::TreePop();
                    }
                }
                ImGui::EndChild();
                
                ImGui::Columns(1);
                ImGui::EndTabItem();
            }

            if ( ImGui::BeginTabItem( "Config" ) )
            {
                ImGui::Spacing();
                static char configNameBuf[128] = "default.json";
                static int selectedConfigIdx = -1;
                static bool configListDirty = true;

                auto* settings = GetSettingsJson();
                if ( configListDirty )
                {
                    settings->UpdateConfigList();
                    configListDirty = false;
                }

                auto& configList = settings->GetConfigList();
                ImGui::InputText( "Config Name" , configNameBuf , sizeof( configNameBuf ) );

                if ( ImGui::Button( "Save Config" , ImVec2( 120 , 0 ) ) )
                {
                    std::string fileName = configNameBuf;
                    if ( fileName.find( ".json" ) == std::string::npos )
                        fileName += ".json";
                    settings->SaveConfig( fileName );
                    configListDirty = true;
                }
                ImGui::SameLine();
                if ( ImGui::Button( "Load Config" , ImVec2( 120 , 0 ) ) )
                {
                    std::string fileName = configNameBuf;
                    if ( fileName.find( ".json" ) == std::string::npos )
                        fileName += ".json";
                    settings->LoadConfig( fileName );
                }
                ImGui::SameLine();
                if ( ImGui::Button( "Delete Config" , ImVec2( 120 , 0 ) ) )
                {
                    std::string fileName = configNameBuf;
                    if ( fileName.find( ".json" ) == std::string::npos )
                        fileName += ".json";
                    settings->DeleteConfig( fileName );
                    configListDirty = true;
                }
                ImGui::SameLine();
                if ( ImGui::Button( "Refresh List" , ImVec2( 120 , 0 ) ) )
                    configListDirty = true;

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Text( "Saved Configs" );
                ImGui::BeginChild( "ConfigList" , ImVec2( 0 , 200 ) , true );
                for ( int i = 0; i < static_cast<int>( configList.size() ); ++i )
                {
                    const bool selected = selectedConfigIdx == i;
                    if ( ImGui::Selectable( configList[i].c_str() , selected ) )
                    {
                        selectedConfigIdx = i;
                        snprintf( configNameBuf , sizeof( configNameBuf ) , "%s" , configList[i].c_str() );
                    }
                }
                ImGui::EndChild();

                ImGui::Spacing();
                ImGui::TextDisabled( "Schema version: %d" , MenuConfig::kSchemaVersion );
                KeyBindWidget( "Menu Toggle Key" , &MenuSettings::menuToggleKey );
                ImGui::Spacing();
                ImGui::TextDisabled( "Configs are saved next to the DLL as .json files." );
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::EndChild();
        ImGui::PopStyleVar();

        ImGui::PopStyleVar();
        ImGui::End();
    }
}
