#pragma once
#include "fx_structs.hpp"

namespace fx_system
{
	float				Com_ParseFloat(const char** buf_p);
	int					Com_ParseInt(const char** buf_p);

	game::Material*		Material_FromHandle(game::Material* handle);
	const char*			Material_GetName(game::Material* handle);
	void				Material_GetInfo(game::Material* handle, game::MaterialInfo* matInfo);

	void				alloc_assign_string(const char** visuals, const char* in);

	FxEditorEffectDef*	get_editor_effect();

#ifdef FXEDITOR
	const char* get_loaded_effect_string();
#else
	std::string			get_loaded_effect_string();
#endif

}