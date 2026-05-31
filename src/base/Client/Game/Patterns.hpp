#pragma once

// Signature patterns — see patterns/pattern.txt (LAST CHECKED: 25/05/2026)

namespace Patterns
{
    // GameOverlayRenderer64.dll (steam)
    constexpr const char* sig_Present = "48 89 5C 24 ? 48 89 6C 24 ? 56 57 41 54 41 56 41 57 48 83 EC ? 41 8B E8";

    // client.dll
    constexpr const char* sig_CreateMove_client = "48 8B C4 4C 89 40 ? 48 89 48 ? 55 53 41 54";
    constexpr const char* sig_OverrideView = "40 57 48 83 EC ? 48 8B FA E8 ? ? ? ? BA";
    constexpr const char* sig_DrawScopeOverlay = "48 8B C4 53 57 48 83 EC ? 48 8B FA";
    constexpr const char* sig_DrawLegs = "40 55 53 56 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? F2 0F 10 42";
    constexpr const char* sig_ValidateInput = "40 53 48 83 EC ? 48 8B D9 E8 ? ? ? ? 33 C0 C6 83 ? ? ? ? 00";
    constexpr const char* sig_GetViewAngles = "4C 8B C1 85 D2 74 ? 48 8D 05";
    constexpr const char* sig_SetViewAngles = "85 D2 75 ? 48 63 81";
    constexpr const char* sig_DrawSmokeVertex = "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 56 41 57 48 83 EC ? 48 8B 9C 24 ? ? ? ? 4D 8B F8";
    constexpr const char* sig_GetChatObject = "48 8B 05 ? ? ? ? C3 ? ? ? ? ? ? ? ? 48 8B 05 ? ? ? ? 48 8D 0D";
    constexpr const char* sig_SendChatMessage = "4C 89 44 24 ? 4C 89 4C 24 ? 53 B8";
    constexpr const char* sig_RegenerateWeaponSkins = "48 83 EC ? E8 ? ? ? ? 48 85 C0 0F 84 ? ? ? ? 48 8B 10";
    constexpr const char* sig_LevelInit = "40 55 56 41 56 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B 0D";
    constexpr const char* sig_LevelShutdown = "48 83 EC ? 48 8B 0D ? ? ? ? 48 8D 15 ? ? ? ? 45 33 C9 45 33 C0 ? ? ? FF 50 ? 48 85 C0 74 ? 48 8B 0D ? ? ? ? 48 8B D0 ? ? ? 41 FF 50 ? 48 83 C4";
    constexpr const char* sig_FlashOverlay = "85 D2 0F 88 ? ? ? ? 48 89 4C 24 ? 55 56";
    constexpr const char* sig_TraceShape = "48 89 5C 24 ? 48 89 4C 24 ? 55 57";
    constexpr const char* sig_TraceManager = "48 8B 0D ? ? ? ? 48 8D 55 ? 66 89 44 24";
    constexpr const char* sig_ChangeModel = "40 53 48 83 EC ? 48 8B D9 4C 8B C2 48 8B 0D ? ? ? ? 48 8D 54 24";
    constexpr const char* sig_FrameStageNotify = "48 89 5C 24 ? 48 89 6C 24 ? 57 48 83 EC ? 48 8B F9 33 ED";
    constexpr const char* sig_OnAddEntity = "48 89 74 24 ? 57 48 83 EC ? 41 B9 ? ? ? ? 41 8B C0 41 23 C1 48 8B F2 41 83 F8 ? 48 8B F9 44 0F 45 C8 41 81 F9 ? ? ? ? 73 ? FF 81";
    constexpr const char* sig_OnRemoveEntity = "48 89 74 24 ? 57 48 83 EC ? 41 B9 ? ? ? ? 41 8B C0 41 23 C1 48 8B F2 41 83 F8 ? 48 8B F9 44 0F 45 C8 41 81 F9 ? ? ? ? 73 ? FF 89";
    constexpr const char* sig_UpdateGlobalVars = "48 8B 0D ? ? ? ? 4C 8D 05 ? ? ? ? 48 85 D2";
    constexpr const char* sig_ForceButtonsDown = "40 53 57 41 56 48 81 EC ? ? ? ? 48 83 79 ? 00";
    constexpr const char* sig_DrawCrosshair = "48 89 5C 24 ? 57 48 83 EC ? 48 8B D9 E8 ? ? ? ? 48 85 C0";
    constexpr const char* sig_UpdatePostProcessing = "48 85 D2 0F 84 ? ? ? ? 48 89 5C 24 ? 57 48 83 EC ? ? ? 00 48 8B DA 48 8B F9 0F 84 ? ? ? ? 48 8D 15";
    constexpr const char* sig_UpdateSkybox = "48 89 5C 24 ? 57 48 83 EC ? 48 8B F9 E8 ? ? ? ? 48 8B 47";
    constexpr const char* sig_CalcViewModel = "40 55 53 56 41 56 41 57 48 8B EC";
    constexpr const char* sig_EquipItemInLoadout = "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 89 54 24 ? 57 41 54 41 55 41 56 41 57 48 83 EC ? 0F B7 FA";
    constexpr const char* sig_ComputeRandomSeed = "48 89 5C 24 ? 57 48 81 EC ? ? ? ? ? ? ? ? 48 8D 8C 24";
    constexpr const char* sig_SetTypeKV3 = "40 53 48 83 EC ? ? ? ? 41 B9 ? ? ? ? 49 83 CA";
    constexpr const char* sig_DrawTeamIntro = "48 83 EC ? ? ? ? ? 44 38 89";
    constexpr const char* sig_ShowMessageBox = "44 88 4C 24 ? 53 41 56";
    constexpr const char* sig_SetPlayerReady = "40 53 48 83 EC ? 48 8B DA 48 8D 15 ? ? ? ? 48 8B CB FF 15";
    constexpr const char* sig_CreateParticleEffect = "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? F3 0F 10 1D ? ? ? ? 41 8B F8 8B DA 4C 8D 05";
    constexpr const char* sig_UnknownParticleFunction = "40 56 48 83 EC ? 41 8B F0";
    constexpr const char* sig_CacheParticleEffect = "4C 8B DC 53 48 81 EC ? ? ? ? F2 0F 10 05";
    constexpr const char* sig_GetParticleManager = "48 8B 05 ? ? ? ? C3 ? ? ? ? ? ? ? ? 48 89 5C 24 ? 57 B8";
    constexpr const char* sig_PopupEventHandle = "40 56 57 41 57 48 83 EC ? 48 8B 3D ? ? ? ? 4D 85 C0";
    constexpr const char* sig_SetMeshGroupMask = "40 53 48 83 EC ? ? ? ? 48 8B D9 4C 39 81";
    constexpr const char* sig_InitTraceData = "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 8D 79 ? 33 F6 C7 47";
    constexpr const char* sig_InitTraceInfo = "40 55 41 55 41 57 48 83 EC";
    constexpr const char* sig_InitFilter = "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 0F B6 41 ? 33 FF 24";
    constexpr const char* sig_CreateTrace = "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 56 41 57 48 83 EC ? ? ? ? ? 4D 8D 71";
    constexpr const char* sig_GetTraceInfo = "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 8B E9 0F 29 74 24";
    constexpr const char* sig_HandleBulletPenetration = "48 8B C4 44 89 48 ? 48 89 50 ? 48 89 48 ? 55 57";
    constexpr const char* sig_GetEntityByIndex = "4C 8D 49 ? 81 FA";
    constexpr const char* sig_GetLocalPawn = "48 83 EC ? 83 F9 ? 75 ? 48 8B 0D ? ? ? ? 48 8D 54 24 ? ? ? ? FF 90 ? ? ? ? ? ? 48 63 C1 4C 8D 05";
    constexpr const char* sig_GetLocalController = "48 83 EC ? 83 F9 ? 75 ? 48 8B 0D ? ? ? ? 48 8D 54 24 ? ? ? ? FF 90 ? ? ? ? ? ? 48 63 C1 48 8D 0D ? ? ? ? ? ? ? ? 48 83 C4 ? C3 ? ? ? ? ? ? ? ? ? ? ? ? ? 48 83 EC ? 83 F9";
    constexpr const char* sig_ReportHit = "40 53 48 83 EC ? 48 8D 05 ? ? ? ? 48 8D 59 ? ? ? ? ? ? ? 74 ? 48 8B CB E8 ? ? ? ? ? ? ? 0F B6 C1 ? ? A8 ? 74 ? 48 8D 59 ? 48 85 DB 74 ? 48 8B CB E8 ? ? ? ? BA ? ? ? ? 48 8B CB 48 83 C4 ? 5B E9 ? ? ? ? 48 83 C4 ? 5B C3 ? ? ? ? ? ? ? ? ? ? 48 89 5C 24 ? 57 48 83 EC ? 48 8D 05 ? ? ? ? 48 8B F9 ? ? ? F6 41 ? ? 74 ? 48 83 C1 ? E8 ? ? ? ? EB ? 48 8B 41 ? 48 83 E0 ? 48 85 C0 75 ? 48 8D 4F ? E8 ? ? ? ? 48 8D 4F ? E8 ? ? ? ? 48 8B 5F ? 0F B6 C3 ? ? A8 ? 74 ? 48 83 C3 ? 74 ? 48 8B CB E8 ? ? ? ? BA ? ? ? ? 48 8B CB E8 ? ? ? ? 48 8B 5C 24 ? 48 83 C4 ? 5F C3 ? ? ? ? ? ? 48 89 5C 24 ? 57 48 83 EC ? 48 8D 05 ? ? ? ? 48 8B D9";
    constexpr const char* sig_RenderDecals = "44 88 4C 24 ? 55 53";
    constexpr const char* sig_SetupMapInfo = "48 8B C4 48 89 58 ? 48 89 68 ? 48 89 70 ? 57 48 81 EC ? ? ? ? 0F 29 70 ? 48 8B EA 0F 29 78 ? 45 33 C0";
    constexpr const char* sig_GetSpread = "48 83 EC ? 48 63 91";
    constexpr const char* sig_GetInaccuracy = "48 89 5C 24 ? 55 56 57 48 81 EC ? ? ? ? 44 0F 29 84 24";
    constexpr const char* sig_MatchFoundHandler = "48 83 EC ? 48 8B 0D ? ? ? ? 48 85 C9 74 ? ? ? ? 48 89 7C 24";
    constexpr const char* sig_PanoramaEvent = "40 56 57 41 57 48 83 EC ? 48 8B 3D ? ? ? ? 4D 85 C0";
    constexpr const char* sig_FindHudElement = "4C 8B DC 53 48 83 EC ? 48 8B 05";
    constexpr const char* sig_EmitPanoramaSound = "40 53 48 81 EC ? ? ? ? ? ? ? 48 8B 05";
    constexpr const char* sig_IsDemoOrHLTV = "48 83 EC ? 48 8B 0D ? ? ? ? ? ? ? FF 90 ? ? ? ? 84 C0 75 ? 38 05";
    constexpr const char* sig_GetMapBspName = "48 8B 0D ? ? ? ? ? ? ? 48 FF A0 ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? 48 89 5C 24 ? 57";
    constexpr const char* sig_GetPlayerModel = "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 8B CA FF 15 ? ? ? ? 48 8B 1D ? ? ? ? 48 8B F0 8B 5B ? 85 DB 74 ? 33 FF 0F 1F 44 00 ? 8B CF E8 ? ? ? ? 48 85 C0 74 ? 8B 88 ? ? ? ? C1 E9 ? F6 C1 ? 74 ? 48 63 C7 EB ? 48 8B C8 E8 ? ? ? ? 48 3B C6 74 ? FF C7 3B FB 75 ? BF ? ? ? ? 8B CF E8 ? ? ? ? 48 8B D8 48 85 C0 74 ? 48 8B C8 E8 ? ? ? ? 48 85 C0 48 0F 45 D8 48 8B CB E8 ? ? ? ? 0F B7 D8 E8 ? ? ? ? 8B D3 45 33 C0 48 8B C8 E8 ? ? ? ? 48 8B D8";
    constexpr const char* sig_GetGameModeName = "48 83 EC ? 48 8B 0D ? ? ? ? ? ? ? FF 90 ? ? ? ? 48 85 C0 74 ? 48 8B 0D ? ? ? ? ? ? ? 4C 8B 42";
    constexpr const char* sig_GetMapName = "48 83 EC ? 48 8B 0D ? ? ? ? ? ? ? FF 90 ? ? ? ? 48 8B C8 48 83 C4";
    constexpr const char* sig_GetServerName = "40 53 48 83 EC ? 48 8B D9 48 8B 0D ? ? ? ? 48 85 C9 74 ? E8 ? ? ? ? 48 85 C0";
    constexpr const char* sig_IsLocalPlayerWatchingOwnDemo = "48 89 5C 24 ? 48 89 6C 24 ? 56 57 41 56 48 83 EC ? 48 8B 0D";
    constexpr const char* sig_IsOverwatch = "48 83 EC ? E8 ? ? ? ? 0F B6 40 ? 48 83 C4 ? C3 ? ? ? ? ? ? ? ? ? ? ? ? ? ? 48 89 5C 24 ? 57";
    constexpr const char* sig_SubmitCommendation = "48 89 74 24 ? 55 57 41 56 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B CA";
    constexpr const char* sig_SubmitPlayerReport = "48 89 5C 24 ? 56 48 83 EC ? 48 8B CA";
    constexpr const char* sig_ShouldShowHudElements = "48 83 EC ? BA ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 85 C0 75 ? 48 8B 05 ? ? ? ? 48 8B 40 ? ? ? 00 74 ? BA";
    constexpr const char* sig_GetPlayerTeamName = "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 8B CA 48 8B EA";
    constexpr const char* sig_IsLatched = "0F B6 81 ? ? ? ? C3 ? ? ? ? ? ? ? ? 48 83 EC ? 33 C9";
    constexpr const char* sig_DrawOverhead = "40 53 48 83 EC ? 48 8B D9 83 FA ? 75";

    // Third person sv_cheats bypass (client.dll) — not in pattern.txt, kept for existing hook
    constexpr const char* sig_ThirdPersonReset = "48 8B 40 08 44 38 20 75 10 44 88 67 01";
    constexpr const char* sig_ThirdPersonBypassAlt = "75 15 44 88 BC 3E ? ? ? ? 44 89 BC 3E";

    // materialsystem2.dll
    constexpr const char* sig_CreateMaterial = "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 41 56 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 8B F2";

    // soundsystem.dll
    constexpr const char* sig_PlayVSound = "48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 55 48 8D 6C 24 ? 48 81 EC ? ? ? ? 33 F6 C7 45 ? ? ? ? ? 4C 8B C1";

    // scenesystem.dll
    constexpr const char* sig_CAnimatableSceneObjectDescRender = "48 8B C4 53 57 41 54";
    constexpr const char* sig_DrawObject = sig_CAnimatableSceneObjectDescRender;
    constexpr const char* sig_DrawLightScene = "? ? ? ? F2 0F 10 42 ? F2 0F 11 41 ? 8B 42 ? 89 41 ? F2 0F 10 42 ? F2 0F 11 41 ? 8B 42 ? 89 41 ? 8B 42 ? 89 41 ? 8B 42";
    constexpr const char* sig_DrawAggregateSceneObjectArray = "48 8B C4 48 89 50 ? 48 89 48 ? 55 53 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 70";
    constexpr const char* sig_SkyboxDrawArray = "45 85 C9 0F 8E ? ? ? ? 4C 8B DC";
    constexpr const char* sig_GeneratePrimitives = "48 8B C4 48 89 58 ? 48 89 50 ? 55 56 57 41 54 41 55 41 56 41 57 48 81 EC";
    constexpr const char* sig_UpdateLightObject = "48 89 54 24 ? 55 57 41 56 48 83 EC";
    constexpr const char* sig_TonemapUpdate = "40 53 48 83 EC ? 48 8B D9 0F 29 74 24";

    // engine2.dll
    constexpr const char* sig_GetAspectRatio = "48 89 5C 24 ? 57 48 83 EC ? 8B FA 48 8D 0D";
    constexpr const char* sig_IsInGame = "48 8B 05 ? ? ? ? 48 85 C0 74 ? 80 B8 ? ? ? ? 00 75 ? 83 B8 ? ? ? ? ? 7C";
    constexpr const char* sig_IsConnected = "48 8B 05 ? ? ? ? 48 85 C0 74 ? 83 B8 ? ? ? ? ? 0F 9D C0";
    constexpr const char* sig_RunCommand = "48 8B C4 48 89 58 ? 48 89 68 ? 48 89 70 ? 57 41 56 41 57 48 81 EC ? ? ? ? 0F 29 70 ? 41 0F B6 E9";
    constexpr const char* sig_GetLevelName = "48 83 EC ? E8 ? ? ? ? 84 C0 74 ? 48 8D 05 ? ? ? ? 48 83 C4 ? C3 48 8B 0D ? ? ? ? 48 85 C9 74 ? 83 B9 ? ? ? ? ? 7C ? 48 8B 89 ? ? ? ? 48 8D 05 ? ? ? ? 48 85 C9 48 0F 45 C1 48 83 C4 ? C3 48 8D 05 ? ? ? ? 48 83 C4 ? C3 ? ? ? ? ? ? ? ? ? ? ? ? 48 83 EC";
    constexpr const char* sig_GetLevelNameShort = "48 83 EC ? E8 ? ? ? ? 84 C0 74 ? 48 8D 05 ? ? ? ? 48 83 C4 ? C3 48 8B 0D ? ? ? ? 48 85 C9 74 ? 83 B9 ? ? ? ? ? 7C ? 48 8B 89 ? ? ? ? 48 8D 05 ? ? ? ? 48 85 C9 48 0F 45 C1 48 83 C4 ? C3 48 8D 05 ? ? ? ? 48 83 C4 ? C3 ? ? ? ? ? ? ? ? ? ? ? ? B8";
    constexpr const char* sig_Connect = "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC ? 44 89 81";

    // tier0.dll
    constexpr const char* sig_LoadKV3_ProcAddress = "?LoadKV3@@YA_NPEAVKeyValues3@@PEAVCUtlString@@PEBDAEBUKV3ID_t@@2I@Z";
}
