#include "std_include.hpp"

namespace dvars
{
	/* game::dvar_s* register_int(const char* dvar_name, int value, int mins, int maxs, __int16 flags, const char* description)
	{
		game::dvar_s* dvar = game::Dvar_RegisterInt(dvar_name, value, mins, maxs, flags, description);
		game::printf_to_console(utils::va("|-> %s <int>\n", dvar_name));

		// return a pointer to our dvar
		return dvar;
	}

	game::dvar_s* register_bool(const char* dvar_name, char value, __int16 flags, const char* description)
	{
		game::dvar_s* dvar = game::Dvar_RegisterBool(dvar_name, value, flags, description);
		game::printf_to_console(utils::va("|-> %s <bool>\n", dvar_name));

		// return a pointer to our dvar
		return dvar;
	}

	game::dvar_s* register_float(const char* dvar_name, float value, float mins, float maxs, __int16 flags, const char* description)
	{
		game::dvar_s*  dvar = game::Dvar_RegisterFloat(dvar_name, value, mins, maxs, flags, description);
		game::printf_to_console(utils::va("|-> %s <float>\n", dvar_name));

		// return a pointer to our dvar
		return dvar;
	}

	game::dvar_s* register_vec4(const char* dvar_name, float x, float y, float z, float w, float mins, float maxs, __int16 flags, const char* description)
	{
		game::dvar_s* dvar = game::Dvar_RegisterVec4(dvar_name, x, y, z, w, mins, maxs, flags, description);
		game::printf_to_console(utils::va("|-> %s <vec4>\n", dvar_name));

		// return a pointer to our dvar
		return dvar;
	}

	void set_bool(game::dvar_s* dvar, bool value)
	{
		game::Dvar_SetBool(dvar, value);
	}

	void set_int(game::dvar_s* dvar, int value)
	{
		game::Dvar_SetInt(dvar, value);
	}

	void set_float(game::dvar_s* dvar, float value)
	{
		game::Dvar_SetFloat(dvar, value);
	}

	bool assign_single_dvar(game::dvar_s*& dest, const char* stock_dvar_name)
	{
		if (!dest)
		{
			if (const auto	var = game::Dvar_FindVar(stock_dvar_name); 
							var) 
			{
				dest = var;
			}
			else
			{
				return false;
			}
		}

		return true;
	} */

	/* void assign_stock_dvars()
	{
		bool valid_dvars = true;

		valid_dvars = assign_single_dvar(dvars::r_filmtweakenable, "r_filmtweakenable");
		valid_dvars = assign_single_dvar(dvars::r_filmtweakdesaturation, "r_filmtweakdesaturation");
		valid_dvars = assign_single_dvar(dvars::r_filmtweakbrightness, "r_filmtweakbrightness");
		valid_dvars = assign_single_dvar(dvars::r_filmtweakcontrast, "r_filmtweakcontrast");
		valid_dvars = assign_single_dvar(dvars::r_filmtweakdarktint, "r_filmtweakdarktint");
		valid_dvars = assign_single_dvar(dvars::r_filmtweaklighttint, "r_filmtweaklighttint");

		if(!valid_dvars)
		{
			game::Com_Error("assign_stock_dvars: failed to assign stock dvars");
		}
	} */

	// --------------------------------------------------

	// register all new dvars here (exec. after config was loaded)
	void register_addon_dvars()
	{
		printf("[Dvars]: register_addon_dvars() start ...\n");

		//fx_system::register_dvars();


		printf("\n");
	}
}