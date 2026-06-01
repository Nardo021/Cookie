#include "Hook_SetModel.hpp"

#include <Client/Features/Inventory/SkinChangerSetModel.hpp>

#include <CS2/SDK/Types/CEntityData.hpp>

auto Hook_SetModel( C_BaseModelEntity* entity , const char* modelName ) -> void
{
	const char* resolved = SkinChangerSetModel::Resolve( entity , modelName );
	SetModel_o( entity , resolved );
}
