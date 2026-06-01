#pragma once

class C_BaseModelEntity;

namespace SkinChangerSetModel
{
	// Returns the model path that should be passed to the original SetModel.
	auto Resolve( C_BaseModelEntity* entity , const char* modelName ) noexcept -> const char*;
}
