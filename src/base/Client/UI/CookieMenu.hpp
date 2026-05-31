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
#include <cstdlib>
#include <ctime>

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
        drawList->AddText(
            ImVec2(iconPos.x + iconSize + 6.0f, winPos.y + (titleBarHeight - ImGui::GetFontSize()) * 0.5f),
            titleColor,
            "Cookie v1.4");

        ImGui::SetCursorPos(ImVec2(15, 30));
        
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
        ImGui::BeginChild("MainArea", ImVec2(winSize.x - 30, winSize.y - 45), false);

        if (ImGui::BeginTabBar("MainTabs", ImGuiTabBarFlags_NoTooltip))
        {
            if (ImGui::BeginTabItem("Rage"))
            {
                ImGui::Spacing();
                ImGui::Columns(2, "rageAimbotLayout", false);
                
                // Left Column
                ImGui::BeginChild("AimbotLeft", ImVec2(0, 0), true);
                ImGui::Text("Main Aimbot");
                ImGui::Separator();
                ImGui::Spacing();
                
                ImGui::Checkbox("Enable Aimbot", &Aimbot::config.enabled);

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
                ImGui::Checkbox("Team Check", &Aimbot::config.teamCheck);
                ImGui::Checkbox("Visible Only (TraceShape)", &Aimbot::config.visCheck);
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
                ImGui::SliderInt("Delay (ms)", &Triggerbot::config.delayMs, 0, 200);

                ImGui::EndChild();
                
                ImGui::Columns(1);
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Visuals"))
            {
                ImGui::Spacing();
                
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
                    ImGui::Spacing();
                }
                ImGui::Separator();
                ImGui::Spacing();
                
                ImGui::Text("Other");
                ImGui::Checkbox("Bunny Hop", &Bhop::config.enabled);
                ImGui::Checkbox("Edge Jump", &Bhop::config.edgeJump);
                ImGui::Checkbox("Jump Bug", &Bhop::config.jumpBug);
                ImGui::Checkbox("Hold Space (Bhop)", &Bhop::config.requireSpace);
                ImGui::Checkbox("Auto Forward", &Bhop::config.autoForward);
                ImGui::Checkbox("Auto Strafe", &Bhop::config.autoStrafe);
                ImGui::Checkbox("Bullet Tracers", &BulletTracer::config.enabled);
                ImGui::Checkbox("Third Person Camera", &ThirdPerson::config.enabled);
                if ( ThirdPerson::config.enabled )
                {
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
                        ImGui::SliderFloat( "Pitch" , &AntiAim::config.pitch , -89.f , 89.f , "%.0f" );
                        ImGui::SameLine();
                        if ( ImGui::Button( "Reset##AAPitchStatic" ) )
                            AntiAim::config.pitch = AntiAim::kPitchDefault;

                        ImGui::SliderFloat( "Yaw" , &AntiAim::config.yaw , -180.f , 180.f , "%.0f" );
                        ImGui::SameLine();
                        if ( ImGui::Button( "Reset##AAYawStatic" ) )
                            AntiAim::config.yaw = AntiAim::kStaticYawDefault;
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

                ImGui::EndChild();
                
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Skins"))
            {
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

            ImGui::EndTabBar();
        }

        ImGui::EndChild();
        ImGui::PopStyleVar();

        ImGui::PopStyleVar();
        ImGui::End();
    }
}
