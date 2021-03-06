#include "std_include.hpp"

namespace game
{
	namespace glob
	{
		// Init
		std::vector<std::string> loadedModules;
	}

	game::vec3_t vec3_origin = { 0.0f, 0.0f, 0.0f };
	game::vec4_t color_white = { 1.0f, 1.0f, 1.0f, 1.0f };

	int& g_processCodeMesh = *reinterpret_cast<int*>(0xDDDF28);

	fx_system::FxSystem& fx_systemPool = *reinterpret_cast<fx_system::FxSystem*>(0xAD5D80);
	fx_system::FxSystemBuffers& fx_systemBufferPool = *reinterpret_cast<fx_system::FxSystemBuffers*>(0xB1F100);

	game::DxGlobals* dx = reinterpret_cast<game::DxGlobals*>(0xB6E204);

	game::GfxBackEndData* get_backenddata()
	{
		const auto out = reinterpret_cast<game::GfxBackEndData*>(*game::backEndDataOut_ptr);
		return out;
	}

	game::GfxBackEndData* get_frontenddata()
	{
		const auto out = reinterpret_cast<game::GfxBackEndData*>(*game::frontEndDataOut_ptr);
		return out;
	}

	// #ENV_DEPENDENT
	Com_Error_t Com_Error = Com_Error_t(0x405820);

	bool& allow_warnings = *reinterpret_cast<bool*>(0x673876);
	Com_PrintError_t Com_PrintError = Com_PrintError_t(0x402640);
}
