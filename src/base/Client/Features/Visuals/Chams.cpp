#include "Chams.hpp"

#include <cstddef>

#include <CS2/SDK/FunctionListSDK.hpp>
#include <CS2/SDK/Interface/CMaterialSystem2.hpp>
#include <CS2/SDK/SDK.hpp>
#include <CS2/SDK/Types/CEntityData.hpp>
#include <CS2/SDK/Types/CHandle.hpp>

#include <GameClient/CL_Players.hpp>

namespace Chams
{
	Config config;

	namespace
	{
		struct KeyValues3Blob
		{
			uint64_t uKey = 0;
			void* pValue = nullptr;
			std::byte pad[0x8]{};
		};

		struct KV3ID_t
		{
			const char* szName = nullptr;
			uint64_t unk0 = 0;
			uint64_t unk1 = 0;
		};

		struct ChamsMaterialPair
		{
			CMaterial2* visible = nullptr;
			CMaterial2* ignoreZ = nullptr;
		};

		bool s_initialized = false;
		ChamsMaterialPair s_flatMaterials{};
		ChamsMaterialPair s_glowMaterials{};
		ChamsMaterialPair s_whiteMaterials{};
		ChamsMaterialPair s_defaultMaterials{};
		ChamsMaterialPair s_illumMaterials{};

		auto SetMaterialColor( material_data_t* data , const float color[4] ) noexcept -> void
		{
			if ( !data )
				return;

			auto* bytes = reinterpret_cast<std::byte*>( data ) + 0x40;
			bytes[0] = static_cast<std::byte>( color[0] * 255.f );
			bytes[1] = static_cast<std::byte>( color[1] * 255.f );
			bytes[2] = static_cast<std::byte>( color[2] * 255.f );
			bytes[3] = static_cast<std::byte>( color[3] * 255.f );
		}

		auto CreateMaterialFromVmat( const char* name , const char* vmatBuffer ) noexcept -> CMaterial2*
		{
			auto* materialSystem = SDK::Interfaces::MaterialSystem2();
			if ( !materialSystem )
				return nullptr;

			alignas( 16 ) std::byte storage[0x100 + sizeof( KeyValues3Blob )]{};
			auto* kv3 = reinterpret_cast<KeyValues3Blob*>( storage + 0x100 );

			KV3ID_t kv3Id{};
			kv3Id.szName = name;
			kv3Id.unk0 = 0x469806E97412167CULL;
			kv3Id.unk1 = 0xE73790B53EE6F2AFULL;

			if ( !KeyValues3_LoadKV3( kv3 , vmatBuffer , &kv3Id ) )
				return nullptr;

			CMaterial2** outMaterial = nullptr;
			const auto result = materialSystem->CreateMaterial( &outMaterial , name , kv3 );
			if ( !result || !*result )
				return nullptr;

			return *result;
		}

		auto CreateFlatMaterials( ChamsMaterialPair& pair ) noexcept -> bool
		{
			static constexpr const char* kFlatVisible = R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d} format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
{
	shader = "csgo_unlitgeneric.vfx"
	F_PAINT_VERTEX_COLORS = 1
	F_TRANSLUCENT = 1
	F_BLEND_MODE = 1
	g_vColorTint = [1, 1, 1, 1]
	TextureAmbientOcclusion = resource:"materials/default/default_mask_tga_fde710a5.vtex"
	g_tAmbientOcclusion = resource:"materials/default/default_mask_tga_fde710a5.vtex"
	g_tColor = resource:"materials/default/default_mask_tga_fde710a5.vtex"
	g_tNormal = resource:"materials/default/default_mask_tga_fde710a5.vtex"
	g_tTintMask = resource:"materials/default/default_mask_tga_fde710a5.vtex"
})";

			static constexpr const char* kFlatIgnoreZ = R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d} format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
{
	shader = "csgo_unlitgeneric.vfx"
	F_PAINT_VERTEX_COLORS = 1
	F_TRANSLUCENT = 1
	F_BLEND_MODE = 1
	F_DISABLE_Z_BUFFERING = 1
	g_vColorTint = [1, 1, 1, 1]
	TextureAmbientOcclusion = resource:"materials/default/default_mask_tga_fde710a5.vtex"
	g_tAmbientOcclusion = resource:"materials/default/default_mask_tga_fde710a5.vtex"
	g_tColor = resource:"materials/default/default_mask_tga_fde710a5.vtex"
	g_tNormal = resource:"materials/default/default_mask_tga_fde710a5.vtex"
	g_tTintMask = resource:"materials/default/default_mask_tga_fde710a5.vtex"
})";

			pair.visible = CreateMaterialFromVmat( "cookie_chams_flat" , kFlatVisible );
			pair.ignoreZ = CreateMaterialFromVmat( "cookie_chams_flat_z" , kFlatIgnoreZ );
			return pair.visible && pair.ignoreZ;
		}

		auto CreateGlowMaterials( ChamsMaterialPair& pair ) noexcept -> bool
		{
			static constexpr const char* kGlowVisible = R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d} format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
{
	shader = "solidcolor.vfx"
	F_SELF_ILLUM = 1
	F_PAINT_VERTEX_COLORS = 1
	F_TRANSLUCENT = 1
	F_IGNOREZ = 0
	F_DISABLE_Z_WRITE = 0
	F_DISABLE_Z_BUFFERING = 0
	g_tColor = resource:"materials/default/default_mask_tga_fde710a5.vtex"
	g_tNormal = resource:"materials/default/default_mask_tga_fde710a5.vtex"
	g_tRoughness = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
	g_tMetalness = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
	g_tSelfIllumMask = resource:"materials/default/default_mask_tga_fde710a5.vtex"
	TextureAmbientOcclusion = resource:"materials/debug/particleerror.vtex"
	g_tAmbientOcclusion = resource:"materials/debug/particleerror.vtex"
	g_vColorTint = [20, 20, 20, 20]
	g_flSelfIllumScale = [5, 5, 5, 5]
	g_flSelfIllumBrightness = [5, 5, 5, 5]
	g_vSelfIllumTint = [10, 10, 10, 10]
})";

			static constexpr const char* kGlowIgnoreZ = R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d} format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
{
	shader = "solidcolor.vfx"
	F_SELF_ILLUM = 1
	F_PAINT_VERTEX_COLORS = 1
	F_TRANSLUCENT = 1
	F_IGNOREZ = 1
	F_DISABLE_Z_WRITE = 1
	F_DISABLE_Z_BUFFERING = 1
	g_tColor = resource:"materials/default/default_mask_tga_fde710a5.vtex"
	g_tNormal = resource:"materials/default/default_mask_tga_fde710a5.vtex"
	g_tRoughness = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
	g_tMetalness = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
	g_tSelfIllumMask = resource:"materials/default/default_mask_tga_fde710a5.vtex"
	TextureAmbientOcclusion = resource:"materials/debug/particleerror.vtex"
	g_tAmbientOcclusion = resource:"materials/debug/particleerror.vtex"
	g_vColorTint = [20, 20, 20, 20]
	g_flSelfIllumScale = [5, 5, 5, 5]
	g_flSelfIllumBrightness = [5, 5, 5, 5]
	g_vSelfIllumTint = [10, 10, 10, 10]
})";

			pair.visible = CreateMaterialFromVmat( "cookie_chams_glow" , kGlowVisible );
			pair.ignoreZ = CreateMaterialFromVmat( "cookie_chams_glow_z" , kGlowIgnoreZ );
			return pair.visible && pair.ignoreZ;
		}

		auto CreateWhiteMaterials( ChamsMaterialPair& pair ) noexcept -> bool
		{
			static constexpr const char* kWhiteVisible = R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d} format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
{
	shader = "csgo_unlitgeneric.vfx"
	F_PAINT_VERTEX_COLORS = 1
	F_TRANSLUCENT = 1
	F_BLEND_MODE = 1
	g_vColorTint = [1, 1, 1, 1]
	TextureAmbientOcclusion = resource:"materials/default/default_mask_tga_fde710a5.vtex"
	g_tAmbientOcclusion = resource:"materials/default/default_mask_tga_fde710a5.vtex"
	g_tColor = resource:"materials/default/default_mask_tga_fde710a5.vtex"
	g_tNormal = resource:"materials/default/default_mask_tga_fde710a5.vtex"
	g_tTintMask = resource:"materials/default/default_mask_tga_fde710a5.vtex"
})";

			static constexpr const char* kWhiteIgnoreZ = R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d} format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
{
	shader = "csgo_unlitgeneric.vfx"
	F_PAINT_VERTEX_COLORS = 1
	F_TRANSLUCENT = 1
	F_BLEND_MODE = 1
	F_DISABLE_Z_BUFFERING = 1
	g_vColorTint = [1, 1, 1, 1]
	TextureAmbientOcclusion = resource:"materials/default/default_mask_tga_fde710a5.vtex"
	g_tAmbientOcclusion = resource:"materials/default/default_mask_tga_fde710a5.vtex"
	g_tColor = resource:"materials/default/default_mask_tga_fde710a5.vtex"
	g_tNormal = resource:"materials/default/default_mask_tga_fde710a5.vtex"
	g_tTintMask = resource:"materials/default/default_mask_tga_fde710a5.vtex"
})";

			pair.visible = CreateMaterialFromVmat( "cookie_chams_white" , kWhiteVisible );
			pair.ignoreZ = CreateMaterialFromVmat( "cookie_chams_white_z" , kWhiteIgnoreZ );
			return pair.visible && pair.ignoreZ;
		}

		auto CreateDefaultMaterials( ChamsMaterialPair& pair ) noexcept -> bool
		{
			static constexpr const char* kDefaultVisible = R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d} format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
{
	shader = "csgo_complex.vfx"
	F_PAINT_VERTEX_COLORS = 1
	F_TRANSLUCENT = 1
	g_vColorTint = [1, 1, 1, 1]
	TextureAmbientOcclusion = resource:"materials/default/default_mask_tga_fde710a5.vtex"
	g_tAmbientOcclusion = resource:"materials/default/default_mask_tga_fde710a5.vtex"
	g_tColor = resource:"materials/default/default_mask_tga_fde710a5.vtex"
	g_tNormal = resource:"materials/default/default_mask_tga_fde710a5.vtex"
	g_tTintMask = resource:"materials/default/default_mask_tga_fde710a5.vtex"
})";

			static constexpr const char* kDefaultIgnoreZ = R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d} format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
{
	shader = "csgo_complex.vfx"
	F_PAINT_VERTEX_COLORS = 1
	F_TRANSLUCENT = 1
	F_DISABLE_Z_BUFFERING = 1
	g_vColorTint = [1, 1, 1, 1]
	TextureAmbientOcclusion = resource:"materials/default/default_mask_tga_fde710a5.vtex"
	g_tAmbientOcclusion = resource:"materials/default/default_mask_tga_fde710a5.vtex"
	g_tColor = resource:"materials/default/default_mask_tga_fde710a5.vtex"
	g_tNormal = resource:"materials/default/default_mask_tga_fde710a5.vtex"
	g_tTintMask = resource:"materials/default/default_mask_tga_fde710a5.vtex"
})";

			pair.visible = CreateMaterialFromVmat( "cookie_chams_default" , kDefaultVisible );
			pair.ignoreZ = CreateMaterialFromVmat( "cookie_chams_default_z" , kDefaultIgnoreZ );
			return pair.visible && pair.ignoreZ;
		}

		auto CreateIllumMaterials( ChamsMaterialPair& pair ) noexcept -> bool
		{
			static constexpr const char* kIllumVisible = R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d} format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
{
	shader = "csgo_complex.vfx"
	F_SELF_ILLUM = 1
	F_PAINT_VERTEX_COLORS = 1
	F_TRANSLUCENT = 1
	g_tColor = resource:"materials/default/default_mask_tga_fde710a5.vtex"
	g_tNormal = resource:"materials/default/default_mask_tga_fde710a5.vtex"
	g_tSelfIllumMask = resource:"materials/default/default_mask_tga_fde710a5.vtex"
	TextureAmbientOcclusion = resource:"materials/debug/particleerror.vtex"
	g_tAmbientOcclusion = resource:"materials/debug/particleerror.vtex"
	g_vColorTint = [20, 20, 20, 20]
	g_flSelfIllumScale = [5, 5, 5, 5]
	g_flSelfIllumBrightness = [5, 5, 5, 5]
	g_vSelfIllumTint = [10, 10, 10, 10]
})";

			static constexpr const char* kIllumIgnoreZ = R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d} format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
{
	shader = "csgo_complex.vfx"
	F_SELF_ILLUM = 1
	F_PAINT_VERTEX_COLORS = 1
	F_TRANSLUCENT = 1
	F_DISABLE_Z_BUFFERING = 1
	g_tColor = resource:"materials/default/default_mask_tga_fde710a5.vtex"
	g_tNormal = resource:"materials/default/default_mask_tga_fde710a5.vtex"
	g_tSelfIllumMask = resource:"materials/default/default_mask_tga_fde710a5.vtex"
	TextureAmbientOcclusion = resource:"materials/debug/particleerror.vtex"
	g_tAmbientOcclusion = resource:"materials/debug/particleerror.vtex"
	g_vColorTint = [20, 20, 20, 20]
	g_flSelfIllumScale = [5, 5, 5, 5]
	g_flSelfIllumBrightness = [5, 5, 5, 5]
	g_vSelfIllumTint = [10, 10, 10, 10]
})";

			pair.visible = CreateMaterialFromVmat( "cookie_chams_illum" , kIllumVisible );
			pair.ignoreZ = CreateMaterialFromVmat( "cookie_chams_illum_z" , kIllumIgnoreZ );
			return pair.visible && pair.ignoreZ;
		}

		auto ShouldDrawPawn( C_CSPlayerPawn* pawn , C_CSPlayerPawn* localPawn ) noexcept -> bool
		{
			if ( !pawn || !localPawn || !pawn->IsAlive() )
				return false;

			const bool isLocal = pawn == localPawn;
			const bool isEnemy = pawn->m_iTeamNum() != localPawn->m_iTeamNum();

			if ( isLocal && config.local )
				return true;

			if ( isEnemy && config.enemy )
				return true;

			if ( !isEnemy && !isLocal )
			{
				if ( config.hideTeammateVanilla )
					return false;
				if ( config.teammate )
					return true;
			}

			return false;
		}

		auto ResolveChamsMaterial() noexcept -> const ChamsMaterialPair*
		{
			switch ( config.materialStyle )
			{
			case MaterialStyle::Glow:
				if ( s_glowMaterials.visible )
					return &s_glowMaterials;
				break;
			case MaterialStyle::White:
				if ( s_whiteMaterials.visible )
					return &s_whiteMaterials;
				break;
			case MaterialStyle::Default:
				if ( s_defaultMaterials.visible )
					return &s_defaultMaterials;
				break;
			case MaterialStyle::Illum:
				if ( s_illumMaterials.visible )
					return &s_illumMaterials;
				break;
			case MaterialStyle::Flat:
			default:
				break;
			}

			return &s_flatMaterials;
		}

		auto ShouldDrawWeapon( C_BaseEntity* entity , C_CSPlayerPawn* localPawn ) noexcept -> bool
		{
			if ( !config.weapon || !entity || !localPawn )
				return false;

			auto* weapon = reinterpret_cast<C_CSWeaponBase*>( entity );
			auto* owner = weapon->m_hOwnerEntity().Get<C_CSPlayerPawn>();
			if ( !owner )
				return false;

			return ShouldDrawPawn( owner , localPawn );
		}

		auto ShouldDrawHands( C_BaseEntity* entity , C_CSPlayerPawn* localPawn ) noexcept -> bool
		{
			if ( !config.hands || !entity || !localPawn )
				return false;

			if ( auto* viewModel = localPawn->GetViewModel() )
			{
				if ( viewModel == entity )
					return true;
			}

			const auto viewModels = localPawn->GetViewModels();
			for ( auto* vm : viewModels )
			{
				if ( vm == entity )
					return true;
			}

			return false;
		}

		auto OverrideMaterial(
			material_data_t* arrMaterialData ,
			int dataCount ,
			DrawObjectFn original ,
			void* animatableObject ,
			void* dx11 ,
			void* sceneView ,
			void* sceneLayer ,
			void* unk1 ,
			void* unk2 ) noexcept -> void
		{
			if ( !arrMaterialData || !original )
				return;

			const ChamsMaterialPair& materials = *ResolveChamsMaterial();

			if ( !config.skipOccludePass && config.ignoreZ && materials.ignoreZ )
			{
				arrMaterialData->m_material = materials.ignoreZ;
				SetMaterialColor( arrMaterialData , config.ignoreZColor );
				original( animatableObject , dx11 , arrMaterialData , dataCount , sceneView , sceneLayer , unk1 , unk2 );
			}

			if ( materials.visible )
			{
				arrMaterialData->m_material = materials.visible;
				SetMaterialColor( arrMaterialData , config.color );
				original( animatableObject , dx11 , arrMaterialData , dataCount , sceneView , sceneLayer , unk1 , unk2 );
			}
		}
	}

	auto Init() noexcept -> bool
	{
		if ( s_initialized )
			return true;

		if ( !CreateFlatMaterials( s_flatMaterials ) )
			return false;

		CreateGlowMaterials( s_glowMaterials );
		CreateWhiteMaterials( s_whiteMaterials );
		CreateDefaultMaterials( s_defaultMaterials );
		CreateIllumMaterials( s_illumMaterials );

		s_initialized = s_flatMaterials.visible && s_flatMaterials.ignoreZ;
		return s_initialized;
	}

	auto IsReady() noexcept -> bool
	{
		return s_initialized;
	}

	auto Shutdown() noexcept -> void
	{
		s_initialized = false;
		s_flatMaterials = {};
		s_glowMaterials = {};
		s_whiteMaterials = {};
		s_defaultMaterials = {};
		s_illumMaterials = {};
	}

	auto OnDrawObject(
		void* animatableObject ,
		void* dx11 ,
		material_data_t* arrMaterialData ,
		int dataCount ,
		void* sceneView ,
		void* sceneLayer ,
		void* unk1 ,
		void* unk2 ,
		DrawObjectFn original ) noexcept -> bool
	{
		if ( !config.enabled || !s_initialized || !arrMaterialData || !original )
			return false;

		auto* localPawn = GetCL_Players()->GetLocalPlayerPawn();
		if ( !localPawn )
			return false;

		if ( !arrMaterialData->m_scene_animable )
			return false;

		CHandle ownerHandle{};
		ownerHandle.m_Index = arrMaterialData->m_scene_animable->m_owner;
		if ( !ownerHandle.IsValid() )
			return false;

		auto* pawn = ownerHandle.Get<C_CSPlayerPawn>();
		if ( pawn )
		{
			if ( !ShouldDrawPawn( pawn , localPawn ) )
				return false;
		}
		else
		{
			auto* entity = ownerHandle.Get<C_BaseEntity>();
			if ( !entity )
				return false;

			if ( ShouldDrawWeapon( entity , localPawn ) )
			{
				// weapon chams
			}
			else if ( !ShouldDrawHands( entity , localPawn ) )
				return false;
		}

		OverrideMaterial(
			arrMaterialData ,
			dataCount ,
			original ,
			animatableObject ,
			dx11 ,
			sceneView ,
			sceneLayer ,
			unk1 ,
			unk2 );

		return true;
	}
}
