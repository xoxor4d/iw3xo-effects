#include "std_include.hpp"

#define Assert()	if(IsDebuggerPresent()) __debugbreak();	\
					game::Com_Error("Line %d :: %s\n%s ", __LINE__, __func__, __FILE__)

namespace fx_system
{
	game::Material* Material_FromHandle(game::Material* handle)
	{
		if (!handle || !handle->info.name || !*handle->info.name)
		{
			Assert();
		}
		
		return handle;
	}

	void Material_GetInfo(game::Material* handle, game::MaterialInfo* matInfo)
	{
		if (!handle || !matInfo)
		{
			Assert();
		}
		
		memcpy(matInfo, Material_FromHandle(handle), sizeof(game::MaterialInfo));
	}

}