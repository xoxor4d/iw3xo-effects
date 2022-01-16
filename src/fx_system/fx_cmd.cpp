#include "std_include.hpp"

#define Assert()	if(IsDebuggerPresent()) \
						__debugbreak();		\
					game::Com_Error("Line %d :: %s\n%s ", __LINE__, __func__, __FILE__)

namespace fx_system
{
	void R_ProcessCmd_UpdateFxRemaining(FxCmd* cmd)
	{
		FxGenerateVertsCmd genVertsCmd = {};

		FX_UpdateRemaining(cmd);
		FX_EndUpdate(cmd->localClientNum);
		Sys_DoWorkerCmd(WRKCMD_GENERATE_MARK_VERTS, cmd);
		FX_AddNonSpriteDrawSurfs(cmd); // part of R_DrawEffects
		FX_FillGenerateVertsCmd(cmd->localClientNum, &genVertsCmd);
		Sys_DoWorkerCmd(WRKCMD_GENERATE_FX_VERTS, &genVertsCmd);
	}

	// #ENV_DEPENDENT
	void Sys_DoWorkerCmd(WorkerCmdType type, void* cmd)
	{
		switch (type)
		{
		case WRKCMD_FIRST_FRONTEND:
			Assert(); //R_ProcessCmd_UpdateFxSpotLight((int)cmd);
			break;

		case WRKCMD_UPDATE_FX_NON_DEPENDENT:
			Assert(); //R_ProcessCmd_UpdateFxNonDependent((FxSystem**)cmd);
			break;

		case WRKCMD_UPDATE_FX_REMAINING:
			R_ProcessCmd_UpdateFxRemaining(static_cast<FxCmd*>(cmd));
			break;

		case WRKCMD_DPVS_CELL_STATIC:
			Assert(); //R_AddCellStaticSurfacesInFrustumCmd((int)cmd);
			break;

		case WRKCMD_DPVS_CELL_SCENE_ENT:
			Assert(); //R_AddCellSceneEntSurfacesInFrustumCmd(savedregs, (int*)cmd);
			break;

		case WRKCMD_DPVS_CELL_DYN_MODEL:
			return;

		case WRKCMD_DPVS_CELL_DYN_BRUSH:
			Assert(); //R_AddCellDynBrushSurfacesInFrustumCmd((int)cmd);
			break;

		case WRKCMD_DPVS_ENTITY:
			Assert(); //R_AddEntitySurfacesInFrustumCmd((int)cmd);
			break;

		case WRKCMD_ADD_SCENE_ENT:
			Assert(); //R_AddSceneEntCmd(*(DWORD*)cmd);
			break;

		case WRKCMD_SPOT_SHADOW_ENT:
			Assert(); //R_AddSpotShadowEntCmd((int)cmd);
			break;

		case WRKCMD_SHADOW_COOKIE:
			Assert(); //R_AddShadowCookies((int)cmd);
			break;

		case WRKCMD_BOUNDS_ENT_DELAYED:
			Assert(); //R_UpdateGfxEntityBoundsCmd(cmd);
			break;

		case WRKCMD_SKIN_ENT_DELAYED:
			Assert(); //R_SkinGfxEntityCmd((int*)cmd);
			break;

		case WRKCMD_GENERATE_FX_VERTS:
			Assert();
			//if (!dx.deviceLost)
			//{
			//	FX_GenerateVerts((FxGenerateVertsCmd*)cmd);// FX_DrawCallback
			//}
			break;

		case WRKCMD_GENERATE_MARK_VERTS:
			Assert();
			//if (!dx.deviceLost)
			//{
			//	FX_GenerateMarkVertsForWorldFX_GenerateMarkVertsForWorld(*((DWORD*)cmd + 1));// FxCmd => localclientnum
			//}
			break;

		case WRKCMD_SKIN_CACHED_STATICMODEL:
			Assert(); //R_SkinXModelCmd(savedregs, (int*)cmd);
			break;

		default:
			Assert();
			break;
		}
	}
}