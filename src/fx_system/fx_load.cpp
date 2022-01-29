#include "std_include.hpp"

#define Assert()	if(IsDebuggerPresent()) __debugbreak();	else {	\
					game::Com_Error("Line %d :: %s\n%s ", __LINE__, __func__, __FILE__); }

#define AssertS(str)	if(IsDebuggerPresent()) __debugbreak();	else {	\
					game::Com_Error("%s\nLine %d :: %s\n%s ", str, __LINE__, __func__, __FILE__); }

namespace fx_system
{
	$145C5CACE7A579404A9D7C1B73F29F79 fx_load = {};

	// * --------------------------------

	void* FX_AllocMem(size_t size)
	{
		auto buff = malloc(size);
		if(buff)
		{
			memset(buff, 0, size);
		}
		else
		{
			Assert();
		}
		

		return buff;
	}

	int FX_HashName(const char* name)
	{
		char letter; 

		std::int16_t hash = 0;
		int scale = 119;

		while (*name)
		{
			letter = *name;

			if (*name == '\\')
			{
				letter = '/';
			}
			else if (letter >= 'A' && letter <= 'Z')
			{
				letter += 32;
			}

			hash += static_cast<std::int16_t>(scale * letter);
			++name;
			++scale;
		}
		return hash & 0x1FF;
	}

	int FX_GetHashIndex(const char* name, bool* exists)
	{
		if (!name || !exists)
		{
			Assert();
		}

		int hashIndex;

		for (hashIndex = FX_HashName(name); fx_load.effectDefs[hashIndex]; hashIndex = ((WORD)hashIndex + 1) & 0x1FF)
		{
			if (!game::I_stricmp(name, fx_load.effectDefs[hashIndex]->name))
			{
				*exists = true;
				return hashIndex;
			}
		}

		*exists = false;
		return hashIndex;
	}

	FxEffectDef* FX_Register_LoadObj(const char* name)
	{
		bool exists = false;
		const int hash_idx = FX_GetHashIndex(name, &exists);

		if (!exists)
		{
			fx_load.effectDefs[hash_idx] = FX_Load(name);
		}

		if (!fx_load.effectDefs[hash_idx])
		{
			Assert();
		}

		return fx_load.effectDefs[hash_idx];
	}

	FxEffectDef* FX_Register(const char* name)
	{
		return FX_Register_LoadObj(name);
	}

	void FX_RegisterDefaultEffect()
	{
		fx_load.defaultEffect = FX_Register("misc/missing_fx");
	}

	FxEffectDef* FX_LoadFailed(const char* name)
	{
		if (!fx_load.defaultEffect)
		{
			if (!game::I_stricmp(name, "misc/missing_fx"))
			{
				AssertS("Couldn't load default effect");
			}

			FX_RegisterDefaultEffect();
			if (!fx_load.defaultEffect || !fx_load.defaultEffect->name)
			{
				Assert();
			}

			if (game::I_strcmp(fx_load.defaultEffect->name, "misc/missing_fx"))
			{
				Assert();
			}
		}

		const char* def_name = &fx_load.defaultEffect->name[strlen(fx_load.defaultEffect->name) + 1];
		const size_t baseBytesNeeded = fx_load.defaultEffect->totalSize - (def_name - fx_load.defaultEffect->name);

		FxEffectDef* effectDef = (FxEffectDef*)FX_AllocMem(fx_load.defaultEffect->totalSize - (def_name - (fx_load.defaultEffect->name + 1)) + strlen(name));

		memcpy(effectDef, fx_load.defaultEffect, baseBytesNeeded);
		effectDef->name = (char*)effectDef + baseBytesNeeded;
		strcpy((char*)effectDef + baseBytesNeeded, name);

		const int relocationDistance = (char*)effectDef - (char*)fx_load.defaultEffect;
		effectDef->elemDefs = (FxElemDef*) ((char*)effectDef->elemDefs + relocationDistance);

		for (int elemIndex = 0; elemIndex < effectDef->elemDefCountOneShot + effectDef->elemDefCountLooping; ++elemIndex)
		{
			effectDef->elemDefs[elemIndex].velSamples = (FxElemVelStateSample*)((char*)effectDef->elemDefs[elemIndex].velSamples + relocationDistance);
			effectDef->elemDefs[elemIndex].visSamples = (FxElemVisStateSample*)((char*)effectDef->elemDefs[elemIndex].visSamples + relocationDistance);
		}

		return effectDef;
	}

	FxEffectDef* FX_Load(const char* name)
	{
		FxEffectDef* effectDef = nullptr;
		FxEditorEffectDef editorEffect = {};

		strcpy(editorEffect.name, name);
		;
		
		//if (!FX_LoadEditorEffect(name, &editorEffect) || (effectDef = FX_Convert(&editorEffect, FX_AllocMem)) == nullptr)
		if (!utils::hook::call<bool(__cdecl)(const char*, FxEditorEffectDef*)>(0x477030)(name, &editorEffect) 
			|| (effectDef = FX_Convert(&editorEffect, FX_AllocMem)) == nullptr)
		{
			effectDef = (FxEffectDef*)FX_LoadFailed(name);
		}

		return effectDef;
	}

}