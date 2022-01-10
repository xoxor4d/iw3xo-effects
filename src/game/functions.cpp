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

	Com_Error_t Com_Error = Com_Error_t(0x405820);
}
