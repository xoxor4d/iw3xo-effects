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


	static utils::function<Material* (const char*, int)> Material_RegisterHandle = 0x48FFD0;
	static utils::function<XModel* (const char*)> R_RegisterModel = 0x4AC7F0;

	static utils::function<int (const char*, const char*)> I_stricmp = 0x443290;
	static utils::function<int (const char*, const char*)> I_strcmp = 0x4432F0;
	static utils::function<void (const char*, const char*, int)> I_strncpyz = 0x443070;

	static utils::function<char* (const char**)> Com_Parse = 0x442500;
	static utils::function<int (const char**, const char*, int)> Com_MatchToken = 0x4425B0;
	static utils::function<void ()> Com_UngetToken = 0x441DB0;
}
