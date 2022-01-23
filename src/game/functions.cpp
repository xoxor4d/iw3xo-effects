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

	fx_system::FxSystem& fx_systemPool = *reinterpret_cast<fx_system::FxSystem*>(0xAD5D80);
	game::DxGlobals& dx = *reinterpret_cast<game::DxGlobals*>(0xB6E204);

	Com_Error_t Com_Error = Com_Error_t(0x405820);
}
