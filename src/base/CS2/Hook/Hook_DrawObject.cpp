#include "Hook_DrawObject.hpp"

auto Hook_DrawObject(
	void* animatableObject ,
	void* dx11 ,
	Chams::material_data_t* arrMaterialData ,
	int dataCount ,
	void* sceneView ,
	void* sceneLayer ,
	void* unk1 ,
	void* unk2 ) -> void
{
	if ( Chams::OnDrawObject(
			animatableObject ,
			dx11 ,
			arrMaterialData ,
			dataCount ,
			sceneView ,
			sceneLayer ,
			unk1 ,
			unk2 ,
			DrawObject_o ) )
	{
		return;
	}

	DrawObject_o( animatableObject , dx11 , arrMaterialData , dataCount , sceneView , sceneLayer , unk1 , unk2 );
}
