#pragma once

#include <cstddef>
#include <cstdint>

class CMaterial2;

namespace Chams
{
	enum class MaterialStyle : int
	{
		Flat = 0 ,
		Glow = 1 ,
		White = 2 ,
		Default = 3 ,
		Illum = 4 ,
	};

	struct Config
	{
		bool          enabled = false;
		bool          ignoreZ = false;
		bool          enemy = true;
		bool          local = false;
		bool          teammate = false;
		bool          weapon = false;
		bool          hands = false;
		MaterialStyle materialStyle = MaterialStyle::Glow;
		float         color[4] = { 0.2f , 0.6f , 1.f , 1.f };
		float         ignoreZColor[4] = { 1.f , 0.2f , 0.2f , 0.8f };
	};

	extern Config config;

	struct scene_animable_object_t
	{
		std::byte pad0[0xB0];
		uint32_t m_owner;
	};

	struct material_data_t
	{
		std::byte pad0[0x18];
		scene_animable_object_t* m_scene_animable;
		CMaterial2* m_material;
		std::byte pad1[0x20];
		std::byte m_color[4];
	};

	using DrawObjectFn = void ( __fastcall* )(
		void* animatableObject ,
		void* dx11 ,
		material_data_t* arrMaterialData ,
		int dataCount ,
		void* sceneView ,
		void* sceneLayer ,
		void* unk1 ,
		void* unk2 );

	auto Init() noexcept -> bool;
	auto IsReady() noexcept -> bool;
	auto Shutdown() noexcept -> void;

	// Returns true when chams were applied (caller should skip default draw).
	auto OnDrawObject(
		void* animatableObject ,
		void* dx11 ,
		material_data_t* arrMaterialData ,
		int dataCount ,
		void* sceneView ,
		void* sceneLayer ,
		void* unk1 ,
		void* unk2 ,
		DrawObjectFn original ) noexcept -> bool;
}
