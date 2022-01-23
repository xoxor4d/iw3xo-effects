#pragma once
#include "fx_system/fx_structs.hpp"

namespace game
{
	namespace glob
	{
		// Init
		extern std::vector<std::string> loadedModules;
	}

	extern game::vec3_t vec3_origin;
	extern game::vec4_t color_white;

	extern fx_system::FxSystem& fx_systemPool;
	extern game::DxGlobals& dx;

	typedef void(*Com_Error_t)(const char* error, ...);
		extern Com_Error_t Com_Error;

	static utils::function<game::dvar_s* (const char* name)> Dvar_FindVar = 0x43DE60;

}
