#include "std_include.hpp"

namespace physics
{
	int Phys_IndexFromODEWorld(dxWorld* world)
	{
		int id = 0;

		while (world != physGlob.world[id])
		{
			if (++id >= 3)
			{
				Assert(); 
				return 3;
			}
		}

		return id;
	}

	void Phys_OdeMatrix3ToAxis(const float* inMatrix, float(*outAxis)[3])
	{
		const float* in;
		float(*out)[3];
		int c;

		int r = 3;
		do
		{
			in = inMatrix;
			out = outAxis;
			c = 3;
			do
			{
				(*out)[0] = *in;
				--c;
				++in;
				++out;
			} while (c);

			--r;
			inMatrix += 4;
			outAxis = (float(*)[3])((char*)outAxis + 4);

		} while (r);
	}

	void Phys_AxisToOdeMatrix3(const float(*inAxis)[3], float* outMatrix)
	{
		float* out;
		const float(*in)[3];
		int c;

		int r = 3;
		do
		{
			out = outMatrix;
			in = inAxis;
			c = 3;
			do
			{
				*out = (*in)[0];
				--c;
				++out;
				++in;
			} while (c);

			outMatrix[3] = 0.0f;
			--r;
			inAxis = (const float(*)[3])((char*)inAxis + 4);
			outMatrix += 4;

		} while (r);
	}

	void AxisTransformVec3(const float* v1, const float* v2, float* out)
	{
		out[0] = v1[3] * v2[1] + v1[0] * v2[0] + v1[6] * v2[2];
		out[1] = v1[1] * v2[0] + v1[4] * v2[1] + v1[7] * v2[2];
		out[2] = v1[2] * v2[0] + v1[5] * v2[1] + v1[8] * v2[2];
	}
}