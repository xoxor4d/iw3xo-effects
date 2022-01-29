#include "std_include.hpp"

#define Assert()	if(IsDebuggerPresent()) __debugbreak();	else {	\
					game::Com_Error("Line %d :: %s\n%s ", __LINE__, __func__, __FILE__); }

namespace fx_system
{
	void FxCurveIterator_AddRef(FxCurve* original)
	{
		if (!original || original->refCount <= 0)
		{
			Assert();
		}
		
		++original->refCount;
	}

	void FxCurveIterator_Create(FxCurveIterator* createe, FxCurve* master)
	{
		if (!createe || !master || master->keyCount <= 0 || master->dimensionCount <= 0 || master->refCount <= 0)
		{
			Assert();
		}

		if (master == (FxCurve*)0xFFFFFFF8)
		{
			Assert();
		}

		FxCurveIterator_AddRef(master);
		createe->master = master;
		createe->currentKeyIndex = 0;
	}

	void FxCurveIterator_FreeRef(FxCurve* releasee)
	{
		if (!releasee || releasee->refCount <= 0)
		{
			Assert();
		}
		
		if (releasee->refCount-- == 1)
		{
			free(releasee);
		}
	}

	void FxCurveIterator_Release(FxCurveIterator* releasee)
	{
		if (!releasee || !releasee->master)
		{
			Assert();
		}

		FxCurveIterator_FreeRef(releasee->master);
		releasee->master = nullptr;
	}

	void FxCurveIterator_MoveToTime(FxCurveIterator* source, float time)
	{
		if (!source || !source->master || source->master->keyCount <= 0 || source->master->dimensionCount <= 0)
		{
			Assert();
		}
		
		if (source->master == (FxCurve*)0xFFFFFFF8)
		{
			Assert();
		}

		if (time < 0.0f || time > 1.0f)
		{
			Assert();
		}
		
		if (source->currentKeyIndex >= static_cast<std::uint32_t>(source->master->keyCount))
		{
			Assert();
		}

		const int keySize = source->master->dimensionCount + 1;
		float* key = &source->master->keys[keySize * source->currentKeyIndex]; // TODO

		if (*key > time)
		{
			source->currentKeyIndex = 0;
			key = source->master->keys;
		}

		while (time > key[keySize])
		{
			++source->currentKeyIndex;
			key += keySize;
		}

		if (key != &source->master->keys[keySize * source->currentKeyIndex])
		{
			Assert();
		}

		if (source->currentKeyIndex >= (unsigned int)source->master->keyCount)
		{
			Assert();
		}
	}

	float FxCurve_Interpolate1d(const float* key, float intermediateTime)
	{
		if (intermediateTime < key[0])
		{
			Assert();
		}

		if(key[2] < intermediateTime)
		{
			Assert();
		}

		if(key[0] == key[2])
		{
			Assert();
		}

		return (intermediateTime - key[0]) * (key[3] - key[1]) / (key[2] - key[0]) + key[1];
	}

	float FxCurveIterator_SampleTime(FxCurveIterator* source, float time)
	{
		FxCurveIterator_MoveToTime(source, time);
		if (source->currentKeyIndex >= static_cast<std::uint32_t>(source->master->keyCount - 1))
		{
			Assert();
		}

		return FxCurve_Interpolate1d(&source->master->keys[2 * source->currentKeyIndex], time);
	}

	float FX_SampleCurve1D(FxCurve* curve, float scale, float time)
	{
		FxCurveIterator iter = {};
		FxCurveIterator_Create(&iter, curve);

		const float value = FxCurveIterator_SampleTime(&iter, time) * scale;
		FxCurveIterator_Release(&iter);

		return value;
	}

}