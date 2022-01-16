#pragma once
#include "fx_structs.hpp"

namespace fx_system
{
	void			FX_DrawElem_BillboardSprite(FxDrawState* draw);
	void			FX_DrawElem_OrientedSprite(FxDrawState* draw);
	void			FX_DrawElem_Tail(FxDrawState* draw);
	void			FX_DrawElem_Cloud(FxDrawState* draw);
	void			FX_DrawElem_Model(FxDrawState* draw);
	void			FX_DrawElem_Light(FxDrawState* draw);
	void			FX_DrawElem_SpotLight(FxDrawState* draw);

	FxElemVisuals	FX_GetElemVisuals(FxElemDef* elemDef, int randomSeed);
	void			FX_SetupVisualState(FxElemDef* elemDef, FxEffect* effect, int randomSeed, float normTimeUpdateEnd, FxElemPreVisualState* preVisState);
	void			FX_EvaluateSize(FxElemPreVisualState* preVisState, FxElemVisualState* visState);

	float			FX_CalculateFade(float dist, FxFloatRange* range);
	void			FX_EvaluateDistanceFade(FxDrawState* draw);

	void			FX_DrawElement_Setup_1_(FxDrawState* draw, int msecBegin, int sequence, float* origin, float* outRealNormTime);
	void			FX_DrawNonSpriteEffect(int elemClass, int drawTime, FxSystem* system, FxEffect* effect);
	void			FX_DrawNonSpriteElems(FxSystem* system);

}