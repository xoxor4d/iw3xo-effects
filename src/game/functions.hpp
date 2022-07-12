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

	extern game::DxGlobals* dx;

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

	inline auto Dvar_RegisterBool = reinterpret_cast<game::dvar_s * (*)(const char* dvar_name, char value, __int16 flags, const char* description)>(0x43F300);
	inline auto Dvar_RegisterInt = reinterpret_cast<game::dvar_s * (*)(const char* dvar_name, int value, int mins, int maxs, __int16 flags, const char* description)>(0x43F360);
	inline auto Dvar_RegisterFloat = reinterpret_cast<game::dvar_s * (*)(const char* dvar_name, float value, float mins, float maxs, __int16 flags, const char* description)>(0x43F3C0);
	//inline auto Dvar_RegisterVec2 = reinterpret_cast<game::dvar_s * (*)(const char* dvar_name, float x, float y, float mins, float maxs, __int16 flags, const char* description)>(0x4B2750);
	//inline auto Dvar_RegisterVec3 = reinterpret_cast<game::dvar_s * (*)(const char* dvar_name, float x, float y, float z, float mins, float maxs, __int16 flags, const char* description)>(0x4B27D0);

	inline auto Dvar_SetBool = reinterpret_cast<void (*)(game::dvar_s* dvar, bool value)>(0x440490);
	inline auto Dvar_SetInt = reinterpret_cast<void (*)(game::dvar_s * dvar, int value)>(0x440730);
	inline auto Dvar_SetFloat = reinterpret_cast<void (*)(game::dvar_s * dvar, float value)>(0x4404D0);

	inline auto FX_RegisterPhysPreset = reinterpret_cast<game::PhysPreset* (*)(const char* name)>(0x475F30);
	inline auto R_AddDebugString = reinterpret_cast<void (*)(game::DebugGlobals* debug, const float* origin, const float* color, float scale, const char* string)>(0x4C9930);

	inline auto R_InitFonts = reinterpret_cast<void (*)()>(0x4D3990);
	inline auto Dvar_SetFromStringByNameFromSource = reinterpret_cast<void (*)(const char* dvar, const char* val, int src)>(0x440A40);
	inline auto Dvar_DisplayableValue = reinterpret_cast<const char* (*)(game::dvar_s* dvar)>(0x43CAD0);
	
}
