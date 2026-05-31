#pragma once
#include <cstddef>
#include <cstdint>

// Memory offsets — synced from sdk/offsets.hpp + sdk/client_dll.hpp (cs2-dumper 2026-05-29)
// Signature patterns live in Patterns.hpp (see patterns/pattern.txt)

namespace Offsets
{
    // === Global Offsets (sdk/offsets.hpp - client.dll) ===
    constexpr std::ptrdiff_t dwEntityList                          = 0x24E5590;
    constexpr std::ptrdiff_t dwLocalPlayerController               = 0x231E700;
    constexpr std::ptrdiff_t dwLocalPlayerPawn                     = 0x233F698;
    constexpr std::ptrdiff_t dwPlantedC4                           = 0x234DF88;
    constexpr std::ptrdiff_t dwViewAngles                          = 0x23548B8;
    constexpr std::ptrdiff_t dwViewMatrix                          = 0x2344B30;
    constexpr std::ptrdiff_t dwCSGOInput                           = 0x2354230;
    constexpr std::ptrdiff_t dwGameRules                           = 0x233F158;
    constexpr std::ptrdiff_t dwGlobalVars                          = 0x205F6D0;
    constexpr std::ptrdiff_t dwGameEntitySystem_highestEntityIndex  = 0x2090;

    // === C_BaseEntity ===
    constexpr std::ptrdiff_t m_pGameSceneNode  = 0x330;  // CGameSceneNode*
    constexpr std::ptrdiff_t m_hOwnerEntity    = 0x520;  // CHandle<C_BaseEntity>
    constexpr std::ptrdiff_t m_nSubclassID     = 0x380;  // CUtlStringToken

    // === C_BasePlayerPawn ===
    constexpr std::ptrdiff_t m_pWeaponServices = 0x11E0; // CPlayer_WeaponServices*
    constexpr std::ptrdiff_t m_pObserverServices = 0x11F8; // CPlayer_ObserverServices*
    constexpr std::ptrdiff_t m_pCameraServices = 0x1218; // CPlayer_CameraServices*

    // === CCSPlayerBase_CameraServices ===
    constexpr std::ptrdiff_t m_iFOV      = 0x290; // uint32
    constexpr std::ptrdiff_t m_iFOVStart = 0x294; // uint32

    // === CPlayer_WeaponServices ===
    constexpr std::ptrdiff_t m_hMyWeapons      = 0x48;   // C_NetworkUtlVectorBase<CHandle<C_BasePlayerWeapon>>
    constexpr std::ptrdiff_t m_hActiveWeapon   = 0x60;   // CHandle<C_BasePlayerWeapon>

    // === CCSPlayerController ===
    constexpr std::ptrdiff_t m_pInventoryServices = 0x810; // CCSPlayerController_InventoryServices*
    constexpr std::ptrdiff_t m_sSanitizedPlayerName = 0x860; // CUtlString
    constexpr std::ptrdiff_t m_hPlayerPawn        = 0x90C; // CHandle<C_CSPlayerPawn>
    constexpr std::ptrdiff_t m_hObserverPawn      = 0x910; // CHandle<CCSObserverPawn>
    constexpr std::ptrdiff_t m_bPawnIsAlive       = 0x914; // bool

    // === CCSPlayerController_InventoryServices ===
    constexpr std::ptrdiff_t m_unMusicID = 0x58; // uint16

    // === C_CSPlayerPawn ===
    constexpr std::ptrdiff_t m_bNeedToReApplyGloves = 0x1655; // bool
    constexpr std::ptrdiff_t m_EconGloves           = 0x1658; // C_EconItemView
    constexpr std::ptrdiff_t m_nEconGlovesChanged   = 0x1AC8; // uint8
    constexpr std::ptrdiff_t m_hHudModelArms        = 0x1B58; // CHandle<C_CS2HudModelArms>
    constexpr std::ptrdiff_t m_iIDEntIndex          = 0x33FC; // CEntityIndex (crosshair entity)

    // === C_BaseModelEntity ===
    constexpr std::ptrdiff_t m_nRenderMode            = 0xC78;  // RenderMode_t
    constexpr std::ptrdiff_t m_clrRender              = 0xC98;  // Color
    constexpr std::ptrdiff_t m_Glow                   = 0xDD8;  // CGlowProperty
    constexpr std::ptrdiff_t m_flGlowBackfaceMult     = 0xE30;  // float32
    constexpr std::ptrdiff_t m_ClientOverrideTint     = 0xF58;  // Color
    constexpr std::ptrdiff_t m_bUseClientOverrideTint = 0xF5C;  // bool

    // === CSkeletonInstance ===
    constexpr std::ptrdiff_t m_materialGroup = 0x3C4; // CUtlStringToken
    constexpr std::ptrdiff_t m_modelState    = 0x150; // CModelState

    // === CModelState ===
    constexpr std::ptrdiff_t m_MeshGroupMask = 0x1C8; // uint64

    // === C_EconEntity ===
    constexpr std::ptrdiff_t m_AttributeManager       = 0x1180; // C_AttributeContainer
    constexpr std::ptrdiff_t m_OriginalOwnerXuidLow   = 0x1650; // uint32
    constexpr std::ptrdiff_t m_OriginalOwnerXuidHigh  = 0x1654; // uint32
    constexpr std::ptrdiff_t m_nFallbackPaintKit      = 0x1658; // int32
    constexpr std::ptrdiff_t m_nFallbackSeed          = 0x165C; // int32
    constexpr std::ptrdiff_t m_flFallbackWear         = 0x1660; // float32
    constexpr std::ptrdiff_t m_nFallbackStatTrak      = 0x1664; // int32

    // === C_AttributeContainer ===
    constexpr std::ptrdiff_t m_Item = 0x50; // C_EconItemView

    // === C_EconItemView ===
    constexpr std::ptrdiff_t m_iItemDefinitionIndex               = 0x1BA; // uint16
    constexpr std::ptrdiff_t m_iEntityQuality                     = 0x1BC; // int32
    constexpr std::ptrdiff_t m_iItemIDHigh                        = 0x1D0; // uint32
    constexpr std::ptrdiff_t m_iItemIDLow                         = 0x1D4; // uint32
    constexpr std::ptrdiff_t m_iAccountID                         = 0x1D8; // uint32
    constexpr std::ptrdiff_t m_bInitialized                       = 0x1E8; // bool
    constexpr std::ptrdiff_t m_bRestoreCustomMaterialAfterPrecache = 0x1B8; // bool
    constexpr std::ptrdiff_t m_AttributeList                      = 0x208; // CAttributeList
    constexpr std::ptrdiff_t m_NetworkedDynamicAttributes         = 0x280; // CAttributeList
    constexpr std::ptrdiff_t m_szCustomName                       = 0x2F8; // char[161]
    constexpr std::ptrdiff_t m_szCustomNameOverride               = 0x399; // char[161]

    // === CAttributeList ===
    constexpr std::ptrdiff_t m_Attributes = 0x8; // C_UtlVectorEmbeddedNetworkVar<CEconItemAttribute>

    // === CEconItemAttribute ===
    constexpr std::ptrdiff_t m_iAttributeDefinitionIndex = 0x30; // uint16
    constexpr std::ptrdiff_t m_flValue                   = 0x34; // float32
    constexpr std::ptrdiff_t m_flInitialValue            = 0x38; // float32
    constexpr std::ptrdiff_t m_nRefundableCurrency       = 0x3C; // int32
    constexpr std::ptrdiff_t m_bSetBonus                 = 0x40; // bool

    // === Aimbot / Entity Offsets ===
    constexpr std::ptrdiff_t m_iHealth          = 0x34C; // int32
    constexpr std::ptrdiff_t m_lifeState        = 0x354; // uint8 (0=alive)
    constexpr std::ptrdiff_t m_iTeamNum         = 0x3EB; // uint8
    constexpr std::ptrdiff_t m_fFlags           = 0x3F8; // uint32
    constexpr std::ptrdiff_t m_vecAbsOrigin     = 0xC8;  // CGameSceneNode, VectorWS
    constexpr std::ptrdiff_t m_vecViewOffset    = 0xE70; // CNetworkViewOffsetVector
    constexpr std::ptrdiff_t m_BoneArray      = 0x80;  // CModelState (community-confirmed)
    constexpr std::ptrdiff_t m_flSimulationTime = 0x3B8; // float32

    // === Visuals (Bomb, Glow, Spectator) ===
    constexpr std::ptrdiff_t m_bBombTicking      = 0x1160; // bool
    constexpr std::ptrdiff_t m_flC4Blow          = 0x1190; // GameTime_t
    constexpr std::ptrdiff_t m_flDefuseCountDown = 0x11B0; // GameTime_t
    constexpr std::ptrdiff_t m_flDefuseLength    = 0x11AC; // float32

    constexpr std::ptrdiff_t m_hObserverTarget = 0x4C; // CPlayer_ObserverServices
    constexpr std::ptrdiff_t m_iObserverMode   = 0x48; // uint8

    constexpr std::ptrdiff_t m_bGlowing          = 0x51; // CGlowProperty
    constexpr std::ptrdiff_t m_glowColorOverride = 0x40; // Color
    constexpr std::ptrdiff_t m_iGlowType         = 0x30; // int32

    constexpr int CCSGOInput_CreateMoveIdx = 21;
    constexpr int CCSGOInput_AllowCameraChangeIdx = 8;
}
