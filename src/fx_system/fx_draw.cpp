#include "std_include.hpp"

#define Assert()	if(IsDebuggerPresent()) \
						__debugbreak();		\
					game::Com_Error("Line %d :: %s\n%s ", __LINE__, __func__, __FILE__)

#define LODWORD(x)  (*((DWORD*)&(x)))  // low dword

namespace fx_system
{
	void (* const s_drawElemHandler[8])(FxDrawState*) =
	{
		FX_DrawElem_BillboardSprite,
		FX_DrawElem_OrientedSprite,
		FX_DrawElem_Tail,
		nullptr,
		FX_DrawElem_Cloud,
		FX_DrawElem_Model,
		FX_DrawElem_Light,
		FX_DrawElem_SpotLight
	};

	// ----------------------

	void FX_DrawElem_BillboardSprite(FxDrawState* draw)
	{
		Assert();
	}

	void FX_DrawElem_OrientedSprite(FxDrawState* draw)
	{
		Assert();
	}

	void FX_DrawElem_Tail(FxDrawState* draw)
	{
		Assert();
	}

	void FX_DrawElem_Cloud(FxDrawState* draw)
	{
		Assert();
	}

	void FX_DrawElem_Model(FxDrawState* draw)
	{
		Assert();
	}

	void FX_DrawElem_Light(FxDrawState* draw)
	{
		Assert();
	}

	void FX_DrawElem_SpotLight(FxDrawState* draw)
	{
		Assert();
	}


	FxElemVisuals FX_GetElemVisuals(FxElemDef* elemDef, int randomSeed)
	{
		FxElemVisuals result = {};

		if (!elemDef->visualCount)
		{
			Assert();
		}

		if (elemDef->visualCount == 1)
		{
			result.anonymous = elemDef->visuals.instance.anonymous;
		}
		else
		{
			//result.anonymous = elemDef->visuals.markArray->materials[(elemDef->visualCount * static_cast<std::uint16_t>(fx_randomTable[21 + randomSeed])) >> 16];
			result.anonymous = elemDef->visuals.markArray->materials[(elemDef->visualCount * FX_RandomFloatAsUInt16(21, randomSeed)) >> 16];
			
		}
		return result;
	}

	void FX_SetupVisualState(FxElemDef* elemDef, FxEffect* effect, int randomSeed, float normTimeUpdateEnd, FxElemPreVisualState* preVisState)
	{
		const float samplePoint = static_cast<float>( static_cast<std::uint8_t>(elemDef->visStateIntervalCount) ) * normTimeUpdateEnd;
		const int point = static_cast<int>(floorf(samplePoint));

		preVisState->sampleLerp = samplePoint - static_cast<float>(point);
		preVisState->sampleLerpInv = 1.0f - preVisState->sampleLerp;
		preVisState->elemDef = elemDef;
		preVisState->effect = effect;
		preVisState->refState = &elemDef->visSamples[point];
		preVisState->randomSeed = randomSeed;
		preVisState->distanceFade = 255;
	}

	void FX_EvaluateSize(FxElemPreVisualState* preVisState, FxElemVisualState* visState)
	{
		const float rnd_width = fx_randomTable[25 + preVisState->randomSeed];
		const float graphA = rnd_width * preVisState->refState->amplitude.size[0] + preVisState->refState->base.size[0];
		const float graphB = rnd_width * preVisState->refState[1].amplitude.size[0] + preVisState->refState[1].base.size[0];

		visState->size[0] = graphB * preVisState->sampleLerp + graphA * preVisState->sampleLerpInv;

		if ((preVisState->elemDef->flags & FX_ELEM_NONUNIFORM_SCALE) != 0)
		{
			const float rnd_height = fx_randomTable[26 + preVisState->randomSeed];
			const float graphX = rnd_height * preVisState->refState->amplitude.size[1] + preVisState->refState->base.size[1];
			const float graphY = rnd_height * preVisState->refState[1].amplitude.size[1] + preVisState->refState[1].base.size[1];

			visState->size[1] = graphY * preVisState->sampleLerp + graphX * preVisState->sampleLerpInv;
		}
		else
		{
			visState->size[1] = visState->size[0];
		}
	}

	float FX_CalculateFade(float dist, FxFloatRange* range)
	{
		const float prog = dist - range->base;
		float fade = 0.0f;

		if (prog < 0.0f)
		{
			return 1.0f;
		}

		if (range->amplitude > prog)
		{
			fade = 1.0f - prog / range->amplitude;
		}

		return fade;
	}

	void FX_EvaluateDistanceFade(FxDrawState* draw)
	{
		if (draw->elemDef->fadeInRange.amplitude != 0.0f || draw->elemDef->fadeOutRange.amplitude != 0.0f)
		{
			float fade_in = 1.0f;
			float fade_out = 1.0f;

			const float dist = Vec3Distance(draw->camera->origin, draw->posWorld);

			if (draw->elemDef->fadeInRange.amplitude != 0.0f)
			{
				fade_in = FX_CalculateFade(dist, &draw->elemDef->fadeInRange);
			}
			if (draw->elemDef->fadeOutRange.amplitude != 0.0f)
			{
				fade_out = 1.0f - FX_CalculateFade(dist, &draw->elemDef->fadeInRange);
			}

			float end = fade_out;
			if (fade_out - fade_in >= 0.0f)
			{
				end = fade_in;
			}

			draw->preVisState.distanceFade = static_cast<std::int64_t>(end * 255.0f + 0.5f);
		}
	}

	void FX_DrawElement_Setup_1_(FxDrawState* draw, int msecBegin, int sequence, float* origin, float* outRealNormTime)
	{
		const int msecElapsed = draw->msecDraw - msecBegin;
		draw->randomSeed = FX_ElemRandomSeed(draw->effect->randomSeed, msecBegin, sequence);
		draw->msecLifeSpan = static_cast<float>(FX_GetElemLifeSpanMsec(draw->randomSeed, draw->elemDef));

		const float msecElapsedFloat = static_cast<float>(msecElapsed);
		const float normTime = msecElapsedFloat / draw->msecLifeSpan;

		if (outRealNormTime)
		{
			*outRealNormTime = normTime;
		}

		if (msecElapsedFloat < draw->msecLifeSpan)
		{
			draw->msecElapsed = msecElapsedFloat;
			draw->normTimeUpdateEnd = normTime;
		}
		else
		{
			draw->msecElapsed = draw->msecLifeSpan;
			draw->normTimeUpdateEnd = 1.0f;
		}

		FX_GetOrientation(draw->elemDef, &draw->effect->frameAtSpawn, &draw->effect->frameNow, draw->randomSeed, &draw->orient);
		FX_OrientationPosToWorldPos(&draw->orient, origin, draw->posWorld);
		FX_SetupVisualState(draw->elemDef, draw->effect, draw->randomSeed, draw->normTimeUpdateEnd, &draw->preVisState);
		FX_EvaluateSize(&draw->preVisState, &draw->visState);

		draw->camera = &draw->system->camera;
		FX_EvaluateDistanceFade(draw);
	}

	void FX_DrawElement(FxElemDef* elemDef, FxElem* elem, FxDrawState* state)
	{
		const std::uint8_t elem_type = static_cast<uint8_t>(elemDef->elemType);
		if (elem_type >= FX_ELEM_TYPE_LAST_DRAWN + 1)
		{
			Assert();
		}

		if (elemDef->visualCount)
		{
			if (elem->msecBegin <= state->msecDraw)
			{
				state->elem = elem;
				state->elemDef = elemDef;
				FX_DrawElement_Setup_1_(state, elem->msecBegin, static_cast<std::uint8_t>(elem->sequence), elem->___u8.origin, 0);

				if (!s_drawElemHandler[elem_type])
				{
					Assert();
				}

				s_drawElemHandler[elem_type](state);
			}
		}
	}

	void FX_DrawNonSpriteEffect(int elemClass, int drawTime, FxSystem* system, FxEffect* effect)
	{
		if (!system)
		{
			Assert();
		}

		FxDrawState drawState = {};
		drawState.effect = effect;
		drawState.msecDraw = drawTime;
		drawState.system = system;

		std::uint16_t elemHandle = effect->firstElemHandle[elemClass];

		if (elemHandle != 0xFFFF)
		{
			while (elemHandle != 0xFFFF)
			{
				FxElem* elem = FX_ElemFromHandle(system, elemHandle);
				FxElemDef* elemDef = &effect->def->elemDefs[static_cast<std::uint8_t>(elem->defIndex)];

				if (elemDef->elemType <= FX_ELEM_TYPE_LAST_SPRITE)
				{
					Assert();
				}

				FX_DrawElement(elemDef, elem, &drawState);
				elemHandle = elem->nextElemHandleInEffect;
			}
		}
	}

	void FX_DrawNonSpriteElems(FxSystem* system)
	{
		if (!system || !system->camera.isValid || system->isArchiving)
		{
			Assert();
		}
		
		for (auto i = system->firstActiveEffect; i != system->firstNewEffect; ++i)
		{
			FxEffect* effect = FX_EffectFromHandle(system, system->allEffectHandles[i & 0x3FF]);
			FX_DrawNonSpriteEffect(1, system->msecDraw, system, effect);
		}

		if (system->needsGarbageCollection)
		{
			FX_RunGarbageCollectionAndPrioritySort(system);
		}
	}
}