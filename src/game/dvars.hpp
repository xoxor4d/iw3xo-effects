#pragma once

namespace dvars
{

	// -----------------------------------------------------

	/* game::dvar_s* register_int(const char* dvar_name, int value, int mins, int maxs, __int16 flags, const char* description);
	game::dvar_s* register_bool(const char* dvar_name, char value, __int16 flags, const char* description);
	game::dvar_s* register_float(const char* dvar_name, float value, float mins, float maxs, __int16 flags, const char* description);
	game::dvar_s* register_vec4(const char* dvar_name, float x, float y, float z, float w, float mins, float maxs, __int16 flags, const char* description);

	void set_bool(game::dvar_s* dvar, bool value);
	void set_int(game::dvar_s* dvar, int value);
	void set_float(game::dvar_s* dvar, float value); */

	//void assign_stock_dvars();

	// -----------------------------------------------------

	void register_addon_dvars();
}