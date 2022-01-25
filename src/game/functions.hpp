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

	extern int& g_processCodeMesh;

	extern fx_system::FxSystem& fx_systemPool;
	extern game::DxGlobals& dx;

	static DWORD* frontEndDataOut_ptr = (DWORD*)(0xCCAA80);
	static DWORD* backEndDataOut_ptr = (DWORD*)(0xDED840);

	extern game::GfxBackEndData* get_backenddata();
	extern game::GfxBackEndData* get_frontenddata();

	typedef void(*Com_Error_t)(const char* error, ...);
		extern Com_Error_t Com_Error;

	extern bool& allow_warnings;

	typedef void(*Com_PrintError_t)(int unused, const char* format, ...);
		extern Com_PrintError_t Com_PrintError;

	static utils::function<game::dvar_s* (const char* name)> Dvar_FindVar = 0x43DE60;

}
