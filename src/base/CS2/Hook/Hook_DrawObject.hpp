#pragma once

#include <Common/Common.hpp>

#include <Client/Features/Visuals/Chams.hpp>

auto Hook_DrawObject(
	void* animatableObject ,
	void* dx11 ,
	Chams::material_data_t* arrMaterialData ,
	int dataCount ,
	void* sceneView ,
	void* sceneLayer ,
	void* unk1 ,
	void* unk2 ) -> void;

using DrawObject_t = decltype( &Hook_DrawObject );
inline DrawObject_t DrawObject_o = nullptr;
