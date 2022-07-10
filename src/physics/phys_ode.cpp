#include "std_include.hpp"

#define Assert()	if(IsDebuggerPresent()) __debugbreak();	else {	\
					game::Com_Error("Line %d :: %s\n%s ", __LINE__, __func__, __FILE__); }

namespace physics
{
	bool physInited = false;

	int g_phys_msecStep[3] = { 11, 11, 11 };
	int g_phys_minMsecStep[3] = { 11, 11, 11 };
	int g_phys_maxMsecStep[3] = { 67, 67, 67 };

	odeGlob_t odeGlob = {};
	PhysGlob physGlob = {};

	/*void ODE_Init()
	{
		Pool_Init(odeGlob.bodies, &odeGlob.bodyPool, 0x150u, 512);
		Pool_Init(odeGlob.geoms, &odeGlob.geomPool, 0xD0u, 2048);
	}*/

	void** Pool_Alloc(pooldata_t* pooldata)
	{
		if (!pooldata)
		{
			Assert();
		}

		if (!pooldata->firstFree)
		{
			return nullptr;
		}

		auto ff = (void**)pooldata->firstFree;
		if (pooldata->firstFree)
		{
			++pooldata->activeCount;
			pooldata->firstFree = *ff;
		}

		return ff;
	}

	void Pool_Init(void* pool, pooldata_t* pooldata, unsigned int itemSize, unsigned int itemCount)
	{
		//unsigned int v8; // r11
		//unsigned int v9; // r8
		//DWORD* v10; // r10

		//if (!pool || !pooldata || itemSize < 4 || itemCount < 2)
		//{
		//	Assert();
		//}

		//v8 = itemCount - 1;
		//pooldata->firstFree = pool;
		//v9 = 0;

		//if (itemCount != 1)
		//{
		//	v10 = static_cast<DWORD*>(pool);
		//	v9 = itemCount - 1;
		//	do
		//	{
		//		--v8;
		//		*v10 = reinterpret_cast<DWORD>((char*)v10 + itemSize);
		//		v10 = (DWORD*)((char*)v10 + itemSize);

		//	} while (v8);
		//}

		//*(DWORD*)((char*)pool + v9 * itemSize) = 0;
		//pooldata->activeCount = 0;


		if (!pool || !pooldata || itemSize < 4 || itemCount < 2)
		{
			Assert();
		}

		if (reinterpret_cast<std::uint8_t>(pool) & 3)
		{
			Assert();
		}

		unsigned int v8; // r9
		DWORD* v9; // r10
		unsigned int v10; // ctr

		pooldata->firstFree = pool;
		v8 = 0;

		if (itemCount != 1)
		{
			v9 = (DWORD*)pool;
			v10 = itemCount - 1;
			v8 = itemCount - 1;
			do
			{
				*v9 = (DWORD)((char*)v9 + itemSize);
				v9 = (DWORD*)((char*)v9 + itemSize);
				--v10;
			} while (v10);
		}
		*(DWORD*)((char*)pool + v8 * itemSize) = 0;
		pooldata->activeCount = 0;
	}

	void Pool_Free(void* data, pooldata_t* pooldata)
	{
		if (!data || !pooldata)
		{
			Assert();
		}
		
		for (void* item = pooldata->firstFree; item; item = *(void**)item)
		{
			if (item == data)
			{
				Assert();
			}
		}

		*(DWORD*)data = *(DWORD*)pooldata->firstFree; // ????
		--pooldata->activeCount;
		pooldata->firstFree = data;
	}

	int Pool_FreeCount(pooldata_t* pooldata)
	{
		DWORD* v1; // ecx

		if (!pooldata)
		{
			Assert();
		}

		v1 = (DWORD*)pooldata->firstFree;

		int freed = 0;
		if (pooldata->firstFree)
		{
			do
			{
				v1 = (DWORD*)*v1;
				++freed;
			} while (v1);
		}
		return freed;
	}

	dxBody* Phys_ObjCreate(int worldIndex, const float* position, const float* quat, const float* velocity, game::PhysPreset* physPreset)
	{
		if (std::isnan(position[0]) || std::isnan(position[1]) || std::isnan(position[2]))
		{
			Assert();
		}

		if (std::isnan(quat[0]) || std::isnan(quat[1]) || std::isnan(quat[2]) || std::isnan(quat[3]))
		{
			Assert();
		}

		if (std::isnan(velocity[0]) || std::isnan(velocity[1]) || std::isnan(velocity[2]))
		{
			Assert();
		}

		if (!physInited || !physPreset)
		{
			Assert();
		}

		float axis[3][3];
		QuatToAxis(quat, axis[0]);

		return Phys_ObjCreateAxis((PhysWorld)worldIndex, position, axis[0], velocity, physPreset);
	}

	void Phys_ObjDestroy(int worldIndex, int id)
	{
		if (!physInited || !id || reinterpret_cast<dxBody*>(id)->world != physGlob.world[worldIndex])
		{
			Assert();
		}

		const auto data = static_cast<PhysObjUserData*>(dBodyGetData((dxBody*)id));
		dBodyDestroy(reinterpret_cast<dxBody*>(id));
		Pool_Free(data, &physGlob.userDataPool);
	}

	void Phys_ObjSetAngularVelocity(int id, const float* velocity)
	{
		const auto body = reinterpret_cast<dxBody*>(id);
		if (!body)
		{
			Assert();
		}

		dBodySetAngularVel(body, velocity[2], *velocity, velocity[1]);
	}

	void Phys_BodyGetRotation(dxBody* body, float(*outAxis)[3])
	{
		if (!body)
		{
			Assert();
		}

		const auto rot = dBodyGetRotation(body);
		return Phys_OdeMatrix3ToAxis(rot, outAxis);
	}

	void Phys_GetBodyPosition(dxBody* body, float* outpos)
	{
		const auto pos = dBodyGetPosition(body);
		outpos[0] = pos[0];
		outpos[1] = pos[1];
		outpos[2] = pos[2];
	}

	void Phys_ObjGetPositionFromCenterOfMass(dxBody* body, const float* centerOfGravity, float* objPos, const float(*rotation)[3])
	{
		const auto userData = static_cast<PhysObjUserData*>(dBodyGetData(body));
		if (!userData)
		{
			Assert();
		}

		float vec[3];
		AxisTransformVec3(rotation[0], userData->translation, vec);
		objPos[0] = centerOfGravity[0] + vec[0];
		objPos[1] = centerOfGravity[1] + vec[1];
		objPos[2] = centerOfGravity[2] + vec[2];
	}

	int Phys_ObjGetSnapshot(PhysWorld worldIndex, dxBody* body, float* outPos, float(*outMat)[3])
	{
		const auto data = static_cast<PhysObjUserData*>(dBodyGetData(body));
		Phys_ObjGetPositionFromCenterOfMass(body, data->savedPos, outPos, data->savedRot);
		memcpy(outMat, data->savedRot, 36u);
		return physGlob.worldData[worldIndex].timeLastSnapshot;
	}

	void Phys_ObjGetPosition(dxBody* id, float* outPosition, float(*outRotation)[3])
	{
		if (!physInited || !id)
		{
			Assert();
		}

		const auto dx_posr = dBodyGetPosition(id);

		Phys_OdeMatrix3ToAxis(dx_posr, outRotation);
		outPosition[0] = dx_posr[0];
		outPosition[1] = dx_posr[1];
		outPosition[2] = dx_posr[2];

		return Phys_ObjGetPositionFromCenterOfMass(id, outPosition, outPosition, outRotation);
	}

	// id = dxBody
	void Phys_ObjGetInterpolatedState(int worldIndex, int id, float* outPos, float* out_quat)
	{
		float obj_rot[3][3];
		float snap_rot[3][3];
		float pos_quat[4];
		float rot_quat[4];
		float snap_pos[3];
		float obj_pos[3];

		const auto lerp = physGlob.worldData[worldIndex].timeNowLerpFrac;
		Phys_ObjGetSnapshot(static_cast<PhysWorld>(worldIndex), reinterpret_cast<dxBody*>(id), snap_pos, snap_rot);
		Phys_ObjGetPosition(reinterpret_cast<dxBody*>(id), obj_pos, obj_rot);

		outPos[0] = snap_pos[0] + (obj_pos[0] - snap_pos[0]) * lerp;
		outPos[1] = snap_pos[1] + (obj_pos[1] - snap_pos[1]) * lerp;
		outPos[2] = snap_pos[2] + (obj_pos[2] - snap_pos[2]) * lerp;

		fx_system::AxisToQuat(snap_rot, pos_quat);
		fx_system::AxisToQuat(obj_rot, rot_quat);
		QuatLerp(pos_quat, rot_quat, lerp, out_quat);
		fx_system::Vec4Normalize(out_quat);
	}

	void ODE_GeomTransformGetOffset(dupe_dxGeomTransform* g, float* vec)
	{
		if (!g || g->type != 6)
		{
			Assert();
		}

		vec[0] = g->transform_posr.pos[0];
		vec[1] = g->transform_posr.pos[1];
		vec[2] = g->transform_posr.pos[2];
	}

	void ODE_GeomTransformSetOffset(dupe_dxGeomTransform* g, const float* vec)
	{
		if (!g || g->type != 6)
		{
			Assert();
		}

		g->transform_posr.pos[0] = vec[0];
		g->transform_posr.pos[1] = vec[1];
		g->transform_posr.pos[2] = vec[2];
		//g->transform_posr.R[0] = 0.0f; // ??
	}

	void ODE_GeomTransformSetRotation(dupe_dxGeomTransform* g, const float* origin, const float(*rotation)[3])
	{
		if (!g || g->type != 6)
		{
			Assert();
		}

		Phys_AxisToOdeMatrix3(rotation, g->transform_posr.R);
		g->transform_posr.pos[0] = origin[0];
		g->transform_posr.pos[1] = origin[1];
		g->transform_posr.pos[2] = origin[2];
		//g->transform_posr.R[0] = 0.0f; // ??
	}

	void Phys_AdjustForNewCenterOfMass(dxBody* b, const float* newRelCenterOfMass)
	{
		const auto bodyUserData = static_cast<PhysObjUserData*>(dBodyGetData(b));
		if (!bodyUserData)
		{
			Assert();
		}

		float b_rot[3][3];
		float b_pos[3];
		float objPos[3];

		Phys_BodyGetRotation(b, b_rot);
		Phys_GetBodyPosition(b, b_pos);
		Phys_ObjGetPositionFromCenterOfMass(b, b_pos, objPos, b_rot);

		b_pos[0] = -bodyUserData->translation[0];
		b_pos[1] = -bodyUserData->translation[1];
		b_pos[2] = -bodyUserData->translation[2];
		bodyUserData->translation[0] = -*newRelCenterOfMass;
		bodyUserData->translation[1] = -newRelCenterOfMass[1];
		bodyUserData->translation[2] = -newRelCenterOfMass[2];

		float vec[3], dvec[3];
		AxisTransformVec3(b_rot[0], newRelCenterOfMass, vec);
		dvec[0] = vec[0] + objPos[0];
		dvec[1] = vec[1] + objPos[1];
		dvec[2] = vec[2] + objPos[2];
		dBodySetPosition(b, dvec[0], dvec[1], dvec[2]);

		auto geom = dBodyGetFirstGeom(b);
		for (auto i = geom; geom; i = geom)
		{
			if (dGeomGetClass(i) == 6)
			{
				float tvec[3];
				ODE_GeomTransformGetOffset(reinterpret_cast<dupe_dxGeomTransform*>(i), tvec);
				tvec[0] = tvec[0] + b_pos[0];
				tvec[1] = tvec[1] + b_pos[1];
				tvec[2] = tvec[2] + b_pos[2];
				tvec[0] = tvec[0] - *newRelCenterOfMass;
				tvec[1] = tvec[1] - newRelCenterOfMass[1];
				tvec[2] = tvec[2] - newRelCenterOfMass[2];
				ODE_GeomTransformSetOffset(reinterpret_cast<dupe_dxGeomTransform*>(i), tvec);
			}
			geom = dGeomGetBodyNext(i);
		}

		bodyUserData->savedPos[0] = dvec[0];
		bodyUserData->savedPos[1] = dvec[1];
		bodyUserData->savedPos[2] = dvec[2];
	}

	void Phys_MassSetBrushTotal(const float* productsOfInertia, const float* momentsOfInertia, dMass* mass, float total_mass)
	{
		float clamped_vec[3] =
		{
			momentsOfInertia[0], momentsOfInertia[1], momentsOfInertia[2]
		};

		if (clamped_vec[2] <= 0.0f) {
			clamped_vec[2] = 100.0;
		}
		
		if (clamped_vec[1] <= 0.0f) {
			clamped_vec[1] = 100.0f;
		}
		
		if (clamped_vec[0] <= 0.0f) {
			clamped_vec[0] = 100.0f;
		}

		dMassSetParameters(mass, total_mass, 0.0f, 0.0f, 0.0f, 
			clamped_vec[0] * total_mass, 
			clamped_vec[1] * total_mass, 
			clamped_vec[2] * total_mass, 
			productsOfInertia[0] * total_mass, 
			productsOfInertia[1] * total_mass, 
			productsOfInertia[2] * total_mass);
	}

	// custom geom was initiated in Phys_Init
	dxGeom* ODE_CreateGeom(int geom_type)
	{
		if (geom_type < GEOM_CLASS_BRUSHMODEL || geom_type > GEOM_CLASS_WORLD)
		{
			Assert();
		}

		return dCreateGeom(geom_type);
	}

	dxGeom* Phys_CreateBrushGeom(game::cbrush_t* brush, const float* centerOfMass)
	{
		if (!brush || !centerOfMass)
		{
			Assert();
		}
		
		if (brush->mins[0] > brush->maxs[0] || brush->mins[1] > brush->maxs[1] || brush->mins[2] > brush->maxs[2])
		{
			Assert();
		}
		
		const auto geom = ODE_CreateGeom(GEOM_CLASS_BRUSH);
		if (geom)
		{
			const auto data = static_cast<GeomStateBrush*>(dGeomGetClassData(geom));
			data->u.brush = brush;
			data->momentsOfInertia[0] = *centerOfMass;
			data->momentsOfInertia[1] = centerOfMass[1];
			data->momentsOfInertia[2] = centerOfMass[2];
		}

		return geom;
	}

	dxGeom* Phys_CreateBrushmodelGeom(__int16 brushModel, const float* centerOfMass)
	{
		if (!brushModel || !centerOfMass)
		{
			Assert();
		}

		const auto geom = ODE_CreateGeom(GEOM_CLASS_BRUSHMODEL);
		if (geom)
		{
			const auto data = static_cast<GeomStateBrush*>(dGeomGetClassData(geom));
			data->u.brushModel = brushModel;
			data->momentsOfInertia[0] = *centerOfMass;
			data->momentsOfInertia[1] = centerOfMass[1];
			data->momentsOfInertia[2] = centerOfMass[2];
		}

		return geom;
	}

	dxGeom* Phys_CreateCylinderGeom(GeomStateCylinder* cylinder)
	{
		if (!cylinder)
		{
			Assert();
		}

		const auto geom = ODE_CreateGeom(GEOM_CLASS_CYLINDER);
		if (geom)
		{
			const auto data = static_cast<GeomStateCylinder*>(dGeomGetClassData(geom));
			data->direction = cylinder->direction;
			data->radius = cylinder->radius;
			data->halfHeight = cylinder->halfHeight;
		}

		return geom;
	}

	dxGeom* Phys_CreateCapsuleGeom(GeomStateCylinder* cylinder)
	{
		if (!cylinder)
		{
			Assert();
		}

		const auto geom = ODE_CreateGeom(GEOM_CLASS_CAPSULE);
		if (geom)
		{
			const auto data = static_cast<GeomStateCylinder*>(dGeomGetClassData(geom));
			data->direction = cylinder->direction;
			data->radius = cylinder->radius;
			data->halfHeight = cylinder->halfHeight;
		}

		return geom;
	}

	void Phys_BodyAddGeomAndSetMass(GeomState* state, dxBody* body, PhysWorld worldIndex, float totalMass, const float* centerOfMass)
	{
		if (totalMass <= 0.0f || !dBodyGetData(body))
		{
			Assert();
		}

		dMass mass;
		dMassSetZero(&mass);

		dxGeom* new_geom = nullptr;

		Phys_AdjustForNewCenterOfMass(body, centerOfMass);
		switch (state->type)
		{
		case PHYS_GEOM_NONE:
			dMassSetSphereTotal(&mass, totalMass, 1.0f);
			break;

		case PHYS_GEOM_BOX:
			dMassSetBoxTotal(&mass, totalMass, state->u.boxState.extent[0], state->u.boxState.extent[1], state->u.boxState.extent[2]);
			new_geom = dCreateBox(physGlob.space[worldIndex], state->u.boxState.extent[0], state->u.boxState.extent[1], state->u.boxState.extent[2]);
			break;

		case PHYS_GEOM_BRUSHMODEL:
			Phys_MassSetBrushTotal(state->u.brushState.productsOfInertia, state->u.brushState.momentsOfInertia, &mass, totalMass);
			new_geom = Phys_CreateBrushmodelGeom(state->u.brushState.u.brushModel, centerOfMass);
			break;

		case PHYS_GEOM_BRUSH:
			Phys_MassSetBrushTotal(state->u.brushState.productsOfInertia, state->u.brushState.momentsOfInertia, &mass, totalMass);
			new_geom = Phys_CreateBrushGeom(state->u.brushState.u.brush, centerOfMass);
			break;

		case PHYS_GEOM_CYLINDER:
			dMassSetCylinderTotal(&mass, totalMass, state->u.cylinderState.direction, state->u.cylinderState.radius, state->u.cylinderState.halfHeight);
			new_geom = Phys_CreateCylinderGeom(&state->u.cylinderState);
			break;

		case PHYS_GEOM_CAPSULE:
			dMassSetCappedCylinderTotal(&mass, totalMass, state->u.cylinderState.direction, state->u.cylinderState.radius, state->u.cylinderState.halfHeight);
			new_geom = Phys_CreateCapsuleGeom(&state->u.cylinderState);
			break;

		case PHYS_GEOM_COUNT:
		default:
			new_geom = nullptr;
			break;
		}

		//if (!new_geom)
		//{
			//Com_PrintWarning(20, "Maximum number of physics geoms exceeded\n");
		//}

		if (state->isOriented && new_geom)
		{
			auto phys_geom = dCreateGeomTransform(physGlob.space[worldIndex]);
			if (!phys_geom)
			{
				//Com_PrintError(20, "Maximum number of physics geoms exceeded\n");
				return dBodySetMass(body, &mass);
			}

			dGeomTransformSetGeom(phys_geom, new_geom);
			ODE_GeomTransformSetRotation(reinterpret_cast<dupe_dxGeomTransform*>(phys_geom), game::vec3_origin, state->orientation);
		}
		return dBodySetMass(body, &mass);
	}

	void Phys_ObjAddGeomBrush(PhysWorld worldIndex, dxBody* id, game::BrushWrapper* bw, const game::PhysMass* mass)
	{
		if (!id)
		{
			Assert();
		}

		dMass z_mass = {};
		dMassSetZero(&z_mass);

		GeomState state = {};
		state.type = PHYS_GEOM_BRUSH;
		state.u.brushState.momentsOfInertia[0] = mass->momentsOfInertia[0];
		state.u.brushState.momentsOfInertia[1] = mass->momentsOfInertia[1];
		state.u.brushState.momentsOfInertia[2] = mass->momentsOfInertia[2];
		state.u.brushState.u.brush = reinterpret_cast<game::cbrush_t*>(bw);
		state.u.brushState.productsOfInertia[0] = mass->productsOfInertia[0];
		state.u.brushState.productsOfInertia[1] = mass->productsOfInertia[1];
		state.u.brushState.productsOfInertia[2] = mass->productsOfInertia[2];
		state.isOriented = false;

		dBodyGetMass(id, &z_mass);
		return Phys_BodyAddGeomAndSetMass(&state, id, worldIndex, z_mass.mass, mass->centerOfMass);
	}

	void Phys_ObjAddGeomBoxRotated(PhysWorld worldIndex, dxBody* body, const float* center, const float* halfLengths, const float(*orientation)[3])
	{
		if (!body)
		{
			Assert();
		}

		dMass z_mass = {};
		dMassSetZero(&z_mass);

		GeomState state = {};
		state.type = PHYS_GEOM_BOX;
		state.isOriented = true;
		state.u.boxState.extent[0] = halfLengths[0] * 2.0f;
		state.u.boxState.extent[1] = halfLengths[1] * 2.0f;
		state.u.boxState.extent[2] = halfLengths[2] * 2.0f;
		state.orientation[0][0] = (*orientation)[0];
		state.orientation[0][1] = (*orientation)[1];
		state.orientation[0][2] = (*orientation)[2];
		state.orientation[1][0] = (*orientation)[3];
		state.orientation[1][1] = (*orientation)[4];
		state.orientation[1][2] = (*orientation)[5];
		state.orientation[2][0] = (*orientation)[6];
		state.orientation[2][1] = (*orientation)[7];
		state.orientation[2][2] = (*orientation)[8];

		dBodyGetMass(body, &z_mass);
		return Phys_BodyAddGeomAndSetMass(&state, body, worldIndex, z_mass.mass, center);
	}

	void Phys_ObjAddGeomCylinderRotated(PhysWorld worldIndex, dxBody* id, const int direction, float radius, float halfHeight, const float* centerOfMass, const float(*orientation)[3])
	{
		if (!id || direction + 1 < 1 || direction + 1 > 3)
		{
			Assert();
		}

		dMass z_mass = {};
		dMassSetZero(&z_mass);

		GeomState state = {};
		state.type = PHYS_GEOM_CYLINDER;
		state.u.cylinderState.direction = direction + 1;
		state.u.cylinderState.radius = radius;
		state.u.cylinderState.halfHeight = halfHeight;
		state.isOriented = true;
		state.orientation[0][0] = (*orientation)[0];
		state.orientation[0][1] = (*orientation)[1];
		state.orientation[0][2] = (*orientation)[2];
		state.orientation[1][0] = (*orientation)[3];
		state.orientation[1][1] = (*orientation)[4];
		state.orientation[1][2] = (*orientation)[5];
		state.orientation[2][0] = (*orientation)[6];
		state.orientation[2][1] = (*orientation)[7];
		state.orientation[2][2] = (*orientation)[8];

		dBodyGetMass(id, &z_mass);
		return Phys_BodyAddGeomAndSetMass(&state, id, worldIndex, z_mass.mass, centerOfMass);
	}

	void Phys_ObjAddGeomBox(PhysWorld worldIndex, dxBody* id, const float* mins, const float* maxs)
	{
		float centerOfMass[3]; // [esp+98h] [ebp-Ch] BYREF

		if (!id)
		{
			Assert();
		}

		dMass z_mass = {};
		dMassSetZero(&z_mass);

		GeomState state = {};
		state.type = PHYS_GEOM_BOX;
		state.isOriented = false;
		centerOfMass[0] = (mins[0] + maxs[0]) * 0.5f;
		centerOfMass[1] = (mins[1] + maxs[1]) * 0.5f;
		centerOfMass[2] = (mins[2] + maxs[2]) * 0.5f;
		state.u.boxState.extent[0] = maxs[0] - mins[0];
		state.u.boxState.extent[1] = maxs[1] - mins[1];
		state.u.boxState.extent[2] = maxs[2] - mins[2];

		dBodyGetMass(id, &z_mass);
		return Phys_BodyAddGeomAndSetMass(&state, id, worldIndex, z_mass.mass, centerOfMass);
	}

	void Phys_ObjSetInertialTensor(dxBody* body, const game::PhysMass* physMass)
	{
		if (!body || !physMass)
		{
			Assert();
		}

		dMass z_mass = {};
		dMassSetZero(&z_mass);

		dBodyGetMass(body, &z_mass);
		Phys_MassSetBrushTotal(physMass->productsOfInertia, physMass->momentsOfInertia, &z_mass, z_mass.mass);
		dBodySetMass(body, &z_mass);

		return Phys_AdjustForNewCenterOfMass(body, physMass->centerOfMass);
	}

	void Phys_ObjSetCollisionFromXModel(const game::XModel* model, int worldIndex, dxBody* body)
	{
		const PhysWorld w_idx = static_cast<PhysWorld>(worldIndex);

		if (model->physGeoms)
		{
			if (model->physGeoms->count)
			{
				auto geom_count = 0u;
				for (auto geom = model->physGeoms->geoms; geom_count < model->physGeoms->count; geom++)
				{
					if (geom->brush)
					{
						Phys_ObjAddGeomBrush(w_idx, body, geom->brush, &model->physGeoms->mass);
					}
					else if (geom->type == PHYS_GEOM_BOX)
					{
						Phys_ObjAddGeomBoxRotated(w_idx, body, geom->offset, geom->halfLengths, geom->orientation);
					}
					else
					{
						Phys_ObjAddGeomCylinderRotated(w_idx, body, 0, geom->halfLengths[1], geom->halfLengths[0], geom->offset, geom->orientation);
					}

					geom_count++;
				}
			}

			Phys_ObjSetInertialTensor(body, &model->physGeoms->mass);
		}
		else
		{
			float mins[3] = 
			{
				model->mins[0], model->mins[1], model->mins[2]
			};

			float maxs[3] =
			{
				model->maxs[0], model->maxs[1], model->maxs[2]
			};

			if (maxs[0] == mins[0] || maxs[1] == mins[1] || maxs[2] == mins[2])
			{
				mins[0] = -50.0;
				mins[1] = -50.0;
				mins[2] = -50.0;
				maxs[0] = 50.0;
				maxs[1] = 50.0;
				maxs[2] = 50.0;
			}

			Phys_ObjAddGeomBox(w_idx, body, mins, maxs);
		}
	}

	dxBody* Phys_CreateBodyFromState(PhysWorld worldIndex, BodyState_t* state)
	{
		if (!state || !physGlob.world[worldIndex] || !physGlob.space[worldIndex])
		{
			Assert();
		}
		
		dWorldSetAutoDisableLinearThreshold(physGlob.world[worldIndex], phys_autoDisableLinear->current.value);
		dWorldSetAutoDisableAngularThreshold(physGlob.world[worldIndex], phys_autoDisableAngular->current.value);
		dWorldSetAutoDisableTime(physGlob.world[worldIndex], phys_autoDisableTime->current.value);

		if (const auto body = dBodyCreate(physGlob.world[worldIndex]); 
								  body)
		{
			if (worldIndex == PHYS_WORLD_RAGDOLL)
			{
				dBodySetFiniteRotationMode(body, 1);
			}

			const auto user_data = (PhysObjUserData*)Pool_Alloc(&physGlob.userDataPool);
			if (!user_data)
			{
				Assert();
			}

			memset(user_data, 0, sizeof(PhysObjUserData));
			dBodySetData(body, user_data);
			dBodySetPosition(body, state->position[0], state->position[1], state->position[2]);
			dBodySetLinearVel(body, state->velocity[0], state->velocity[1], state->velocity[2]);
			dBodySetAngularVel(body, state->angVelocity[0], state->angVelocity[1], state->angVelocity[2]);

			float ode_mtx[12];
			Phys_AxisToOdeMatrix3(state->rotation, ode_mtx);
			dBodySetRotation(body, ode_mtx);

			GeomState geom_state = {};
			geom_state.type = PHYS_GEOM_NONE;
			geom_state.isOriented = false;

			float center[3];
			center[0] = -state->centerOfMassOffset[0];
			center[1] = -state->centerOfMassOffset[1];
			center[2] = -state->centerOfMassOffset[2];
			Phys_BodyAddGeomAndSetMass(&geom_state, body, worldIndex, state->mass, center);

			user_data->translation[0] = state->centerOfMassOffset[0];
			user_data->translation[1] = state->centerOfMassOffset[1];
			user_data->translation[2] = state->centerOfMassOffset[2];
			user_data->body = body;
			user_data->savedPos[0] = state->position[0];
			user_data->savedPos[1] = state->position[1];
			user_data->savedPos[2] = state->position[2];
			memcpy(user_data->savedRot, state->rotation, sizeof(user_data->savedRot));

			user_data->bounce = state->bounce;
			user_data->friction = state->friction;
			user_data->state = state->state;
			user_data->timeLastAsleep = state->timeLastAsleep;
			user_data->sndClass = state->type;
			Phys_GetBodyPosition(body, user_data->awakeTooLongLastPos);

			if (!state->unk)
			{
				dBodyDisable(body);
			}

			 return body;
		}

		//Com_PrintWarning(20, "Maximum number of physics bodies exceeded (more than %i)\n");
		return nullptr;
	}

	dxBody* Phys_ObjCreateAxis(PhysWorld worldIndex, const float* position, float* axis, const float* velocity, game::PhysPreset* physPreset)
	{
		if ((std::_Is_nan(position[0]) || std::_Is_nan(position[1]) || std::_Is_nan(position[2])))
		{
			Assert();
		}
		if ((std::_Is_nan(velocity[0]) || std::_Is_nan(velocity[1]) || std::_Is_nan(velocity[2])))
		{
			Assert();
		}

		if (!physInited || !physPreset)
		{
			Assert();
		}

		BodyState_t state = {};

		fx_system::AxisCopy(axis, state.rotation[0]);
		state.position[0] = position[0];
		state.position[1] = position[1];
		state.position[2] = position[2];
		state.velocity[0] = velocity[0];
		state.velocity[1] = velocity[1];
		state.velocity[2] = velocity[2];
		state.angVelocity[0] = 0.0f;
		state.angVelocity[1] = 0.0f;
		state.angVelocity[2] = 0.0f;
		state.centerOfMassOffset[0] = 0.0f;
		state.centerOfMassOffset[1] = 0.0f;
		state.centerOfMassOffset[2] = 0.0f;
		state.timeLastAsleep = physGlob.worldData[worldIndex].timeLastUpdate;
		state.state = PHYS_OBJ_STATE_POSSIBLY_STUCK;
		state.unk = true;
		state.mass = physPreset->mass;
		state.bounce = physPreset->bounce;
		state.friction = physPreset->friction;
		return Phys_CreateBodyFromState(worldIndex, &state);
	}

	dxWorld* ODE_BodyGetWorld(dxBody* b)
	{
		return b->world;
	}

	void Phys_ObjAddForce(PhysWorld worldIndex, dxBody* id, const float* worldPos, const float* impulse)
	{
		const auto step = 1000.0f / static_cast<float>(g_phys_msecStep[worldIndex]);
		if (!physInited || !id)
		{
			Assert();
		}

		dBodyAddForceAtPos(id, impulse[0] * step, impulse[1] * step, impulse[2] * step, *worldPos, worldPos[1], worldPos[2]);
		dBodyEnable(id);
		const auto data = static_cast<PhysObjUserData*>(dBodyGetData(id));
		data->timeLastAsleep = physGlob.worldData[Phys_IndexFromODEWorld(ODE_BodyGetWorld(id))].timeLastUpdate;
	}

	// *
	// *

	void Phys_RewindCurrentTime(PhysWorld world, int timeNow)
	{
		const auto last_update = physGlob.worldData[world].timeLastUpdate;
		const auto last_snap = physGlob.worldData[world].timeLastSnapshot;

		if (last_update <= last_snap)
		{
			physGlob.worldData[world].timeNowLerpFrac = 1.0f;
			physGlob.worldData[world].timeLastUpdate = timeNow;
			physGlob.worldData[world].timeLastSnapshot = timeNow;
			return;
		}

		const auto delta = static_cast<float>((timeNow - last_snap)) / static_cast<float>((last_update - last_snap));

		if (physGlob.worldData[world].timeNowLerpFrac <= delta)
		{
			return;
		}

		if (delta < 0.0f || delta > 1.0f)
		{
			physGlob.worldData[world].timeNowLerpFrac = 1.0f;
			physGlob.worldData[world].timeLastUpdate = timeNow;
			physGlob.worldData[world].timeLastSnapshot = timeNow;
		}
		else
		{
			physGlob.worldData[world].timeNowLerpFrac = delta;
		}
	}

	void Phys_BodyGrabSnapshot(dxBody* body)
	{
		if (!body)
		{
			Assert();
		}

		const auto userData = static_cast<PhysObjUserData*>(dBodyGetData(body));
		if (!userData)
		{
			Assert();
		}

		Phys_GetBodyPosition(body, userData->savedPos);
		if (std::isnan(userData->savedPos[0]) || std::isnan(userData->savedPos[1]) || std::isnan(userData->savedPos[2]))
		{
			Assert();
		}

		Phys_BodyGetRotation(body, userData->savedRot);
		if (	std::isnan(userData->savedRot[0][0]) || std::isnan(userData->savedRot[0][1]) || std::isnan(userData->savedRot[0][2])
			 || std::isnan(userData->savedRot[1][0]) || std::isnan(userData->savedRot[1][1]) || std::isnan(userData->savedRot[1][2])
			 || std::isnan(userData->savedRot[2][0]) || std::isnan(userData->savedRot[2][1]) || std::isnan(userData->savedRot[2][2]))
		{
			Assert();
		}
	}

	void Phys_DoBodyOncePerRun(dxBody* body)
	{
		if (dBodyIsEnabled(body))
		{
			float o_mins, o_maxs;

			auto mins = 131072.0f;
			auto maxs = -131072.0f;

			auto counter = 0;

			for (float* i = body->posr.pos; ; i++)
			{
				o_mins = mins;
				o_maxs = maxs;
				if (o_mins < i[0])
				{
					break;
				}

				maxs = o_maxs;
				mins = o_mins;
				if (maxs > i[0])
				{
					break;
				}

				if (++counter == 3)
				{
					return;
				}
			}

			dBodyDisable(body);
		}
	}

	bool Phys_DoBodyOncePerFrame(dxBody* k, PhysWorld worldIndex, float deltaT)
	{
		if (!dBodyIsEnabled(k))
		{
			return false;
		}

		if (worldIndex == PHYS_WORLD_RAGDOLL)
		{
			k->avel[0] = k->avel[0] + k->avel[0] * -0.009999999776482582f;
			k->avel[1] = k->avel[1] + k->avel[1] * -0.009999999776482582f;
			k->avel[2] = k->avel[2] + k->avel[2] * -0.009999999776482582f;
		}
		else
		{
			auto linear_drag = 1.0f - phys_dragLinear->current.value * deltaT;
			linear_drag = linear_drag < 0.0f ? 0.0f : linear_drag;

			k->lvel[0] = k->lvel[0] * linear_drag;
			k->lvel[1] = k->lvel[1] * linear_drag;
			k->lvel[2] = k->lvel[2] * linear_drag;

			auto angular_drag = 1.0f - deltaT * phys_dragAngular->current.value;
			angular_drag = angular_drag < 0.0f ? 0.0f : angular_drag;

			k->avel[0] = k->avel[0] * angular_drag;
			k->avel[1] = k->avel[1] * angular_drag;
			k->avel[2] = k->avel[2] * angular_drag;
		}

		return true;
	}

	void ODE_ForEachBody(dxWorld* world, void(__cdecl *func)(dxBody*))
	{
		for (auto i = world->firstbody; i; i = (dxBody*)i->next)
		{
			func(i);
		}
	}

	void dxPostProcessIslands(PhysWorld worldIndex)
	{
		const auto world = physGlob.world[worldIndex];
		for (auto b = world->firstbody; b; b = (dxBody*)b->next)
		{
			if ((b->flags & 4) == 0)
			{
				b->facc[0] = 0.0;
				b->facc[1] = 0.0;
				b->facc[2] = 0.0;
				b->tacc[0] = 0.0;
				b->tacc[1] = 0.0;
				b->tacc[2] = 0.0;

				for (auto g = b->geom; g; g = dGeomGetBodyNext(g))
				{
					dGeomMoved(g);
				}
			}
		}

		dJointGroupEmpty(physGlob.contactgroup[worldIndex]);
		physGlob.worldData[worldIndex].numJitterRegions = 0;

		auto body = world->firstbody;
		int body_count;

		for (body_count = 0; body; body = (dxBody*)body->next)
		{
			if (Phys_DoBodyOncePerFrame(body, worldIndex, physGlob.world_seconds[worldIndex]))
			{
				++body_count;
			}
		}

		const auto step_for_count = static_cast<float>((body_count - 32)) / 18.0f;
		const auto s0 = step_for_count - 1.0f < 0.0f ? step_for_count : 1.0f;
		const auto s1 = 0.0f - step_for_count < 0.0f ? s0 : 0.0f;

		g_phys_msecStep[worldIndex] = g_phys_minMsecStep[worldIndex] + static_cast<int>(( static_cast<float>((g_phys_maxMsecStep[worldIndex] - g_phys_minMsecStep[worldIndex])) * s1));
	}

	void Phys_NearCallback(void* userData, dxGeom* geom1, dxGeom* geom2)
	{
		ContactList list1 = {};
		ContactList list2 = {};

		unsigned int world_type = *((DWORD*)userData + 1);
		auto body_1 = dGeomGetBody(geom1);
		auto body_2 = dGeomGetBody(geom2);

		if (!body_1 || !body_2 || !dAreConnectedExcluding(body_1, body_2, 4))
		{
			const int contact_points = dCollide(geom1, geom2, 128, (dContactGeom*)&list2, sizeof(dContactGeomExt)); // size changed 
			list2.contactCount = contact_points;
			auto body_two_as_one = body_2;

			if (body_1)
			{
				body_two_as_one = (dxBody*)(body_2 == nullptr ? (unsigned int)body_1 : 0);
			}
			if (contact_points <= 0)
			{
				if (body_two_as_one)
				{
					const auto bodyUserData = static_cast<PhysObjUserData*>(dBodyGetData(body_two_as_one));
					if (!bodyUserData)
					{
						Assert();
					}

					bodyUserData->state = PHYS_OBJ_STATE_FREE;
				}
			}
			else
			{
				auto friction = 0.0f;
				auto bounce = 1.0f;

				if (body_1)
				{
					const auto data = static_cast<PhysObjUserData*>(dBodyGetData(body_1));
					bounce = ClampMin(data->bounce, 1.0f);
					friction = data->friction + 0.0f;
				}
				if (body_2)
				{
					const auto data = static_cast<PhysObjUserData*>(dBodyGetData(body_2));
					bounce = ClampMin(data->bounce, bounce);
					friction = data->friction + friction;
				}

				dSurfaceParameters surface_parameters = {};

				if (!phys_contact_cfm || !phys_contact_erp)
				{
					Assert();
				}

				float erp_val;

				if (world_type == PHYS_WORLD_RAGDOLL)
				{
					surface_parameters.soft_cfm = phys_contact_cfm_ragdoll->current.value;
					erp_val = phys_contact_erp_ragdoll->current.value;
				}
				else
				{
					surface_parameters.soft_cfm = phys_contact_cfm->current.value;
					erp_val = phys_contact_erp->current.value;
				}

				surface_parameters.mode = 12316;
				surface_parameters.soft_erp = erp_val;
				surface_parameters.mu = phys_frictionScale->current.value * friction;
				surface_parameters.mu2 = 0.0f;
				surface_parameters.bounce = bounce;
				surface_parameters.bounce_vel = 0.1f;

				if (list2.contactCount >= 5)
				{
					Phys_ReduceContacts(&list2, &list1);
					Phys_CreateJointForEachContact(&list1, body_1, body_2, &surface_parameters, static_cast<PhysWorld>(world_type));
				}
				else
				{
					Phys_CreateJointForEachContact(&list2, body_1, body_2, &surface_parameters, static_cast<PhysWorld>(world_type));
				}
			}
		}
	}

	void Phys_RunFrame(PhysWorld worldIndex, float seconds)
	{
		if (!physInited)
		{
			Assert();
		}

		const auto world = physGlob.world[worldIndex];
		physGlob.world_seconds[worldIndex] = seconds;

		if (phys_dumpcontacts->current.enabled)
		{
			physGlob.dumpContacts = true;
			game::Dvar_SetBool(phys_dumpcontacts, false);
		}

		dWorldSetCFM(world, phys_cfm->current.value);
		dWorldSetERP(world, phys_erp->current.value);

		float max_correcting_val;
		if (worldIndex == PHYS_WORLD_RAGDOLL)
		{
			max_correcting_val = phys_mcv_ragdoll->current.value;
		}
		else
		{
			max_correcting_val = phys_mcv->current.value;
		}

		dWorldSetContactMaxCorrectingVel(world, max_correcting_val);
		dWorldSetContactSurfaceLayer(world, phys_csl->current.value);
		dWorldSetQuickStepNumIterations(world, phys_qsi->current.integer);
		dWorldSetAutoDisableLinearThreshold(world, phys_autoDisableLinear->current.value);
		dWorldSetAutoDisableAngularThreshold(world, phys_autoDisableAngular->current.value);
		dWorldSetAutoDisableTime(world, phys_autoDisableTime->current.value);

		dWorldSetGravity(world, 
			physGlob.gravityDirection[0] * -phys_gravity->current.value,
			physGlob.gravityDirection[1] * -phys_gravity->current.value,
			physGlob.gravityDirection[2] * -phys_gravity->current.value);

		if (phys_interBodyCollision->current.enabled)
		{
			dSpaceCollide(physGlob.space[worldIndex], &worldIndex, Phys_NearCallback);
		}

		const auto callback = physGlob.worldData[worldIndex].collisionCallback;
		if (callback)
		{
			callback();
		}

		dWorldQuickStep(world, seconds);
		physGlob.dumpContacts = false;
	}

	void Phys_RunToTime(int worldIndex, int timeNow)
	{
		const auto wd = &physGlob.worldData[worldIndex];
		if (!physInited)
		{
			Assert();
		}
		if (timeNow < wd->timeLastSnapshot)
		{
			Phys_RewindCurrentTime(static_cast<PhysWorld>(worldIndex), timeNow);
		}

		const auto dx_world = physGlob.world[worldIndex];
		if (wd->timeLastUpdate < timeNow)
		{
			wd->timeLastSnapshot = wd->timeLastUpdate;
			ODE_ForEachBody(dx_world, Phys_BodyGrabSnapshot);

			auto maxIter = 2u;
			for (auto i = 2u; ; maxIter = i)
			{
				if (!maxIter)
				{
					Assert();
				}

				const auto delta = (timeNow - wd->timeLastUpdate) / static_cast<int>(maxIter);
				auto step = g_phys_msecStep[worldIndex];

				if (step < delta)
				{
					step = delta;
				}

				--i;

				Phys_RunFrame(static_cast<PhysWorld>(worldIndex), static_cast<float>(step) * 0.001f);
				wd->timeLastUpdate += step;
				dxPostProcessIslands(static_cast<PhysWorld>(worldIndex));

				if (wd->timeLastUpdate >= timeNow)
				{
					break;
				}
			}

			ODE_ForEachBody(dx_world, Phys_DoBodyOncePerRun);
		}

		if (wd->timeLastSnapshot > timeNow || timeNow > wd->timeLastUpdate)
		{
			Assert();
		}

		if (wd->timeLastUpdate <= wd->timeLastSnapshot)
		{
			if (wd->timeLastUpdate != wd->timeLastSnapshot)
			{
				Assert();
			}

			wd->timeNowLerpFrac = 1.0f;
		}
		else
		{
			const auto delta = static_cast<float>((timeNow - wd->timeLastSnapshot)) / static_cast<float>((wd->timeLastUpdate - wd->timeLastSnapshot));
			wd->timeNowLerpFrac = delta;

			if (delta < 0.0f || delta > 1.0f)
			{
				Assert();
			}
		}
	}

	void Phys_Init()
	{
		if (!physInited)
		{
			memset(&physGlob, 0, sizeof(physGlob));
			Pool_Init(physGlob.userData, &physGlob.userDataPool, sizeof(PhysObjUserData), 512);

			dInitODE();

			for (auto w = 0; w < 3; w++)
			{
				physGlob.world[w] = dWorldCreate();
				physGlob.space[w] = dSimpleSpaceCreate(nullptr);

				physGlob.contactgroup[w] = &odeGlob.contactsGroup[w];

				dWorldSetAutoDisableFlag(physGlob.world[w], 1);
				dWorldSetAutoDisableSteps(physGlob.world[w], 0);
			}

			physGlob.dumpContacts = false;
		}

		register_dvars();

		Phys_InitBrushmodelGeomClass();
		Phys_InitBrushGeomClass();
		Phys_InitCylinderGeomClass();
		Phys_InitCapsuleGeomClass();

		physGlob.gravityDirection[0] =  0.0f;
		physGlob.gravityDirection[1] =  0.0f;
		physGlob.gravityDirection[2] = -1.0f;

		physInited = true;
	}

	void Phys_Shutdown()
	{
		if (physInited)
		{

			if (physGlob.world[0]->nb || physGlob.world[1]->nb || physGlob.world[2]->nb)
			{
				Assert();
			}

			if (Pool_FreeCount(&physGlob.userDataPool) != 512)
			{
				Assert();
			}

			if (Pool_FreeCount(&odeGlob.bodyPool) != 512)
			{
				Assert();
			}
			if (Pool_FreeCount(&odeGlob.geomPool) != 2048)
			{
				Assert();
			}

			dCloseODE();
			physGlob.triMeshInfo.verts = nullptr;
			physInited = false;
		}
	}
}
