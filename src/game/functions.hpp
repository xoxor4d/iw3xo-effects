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
	extern fx_system::FxSystemBuffers& fx_systemBufferPool;

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

	static utils::function<unsigned int (const char*, void**)> FS_ReadFile = 0x42DF20;
	static utils::function<void (void*)> FS_FreeFile = 0x42DFD0;

	static utils::function<void (const char*)> Com_BeginParseSession = 0x441970;
	static utils::function<void ()> Com_EndParseSession = 0x441A10;
	static utils::function<void (int)> Com_SetSpaceDelimited = 0x441A50;
	static utils::function<void (int)> Com_SetParseNegativeNumbers = 0x441AE0;

	static utils::function<char* (const char**)> Com_Parse = 0x442500;
	static utils::function<int (const char**, const char*, int)> Com_MatchToken = 0x4425B0;
	static utils::function<void ()> Com_UngetToken = 0x441DB0;
}
