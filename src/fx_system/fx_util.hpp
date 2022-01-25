#pragma once
#include "fx_structs.hpp"

namespace fx_system
{
	game::Material*		Material_FromHandle(game::Material* handle);
	void				Material_GetInfo(game::Material* handle, game::MaterialInfo* matInfo);
}