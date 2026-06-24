#include "UniPipe.h"
#include "skygfx.h"
#include "buildingPipe.cpp"

BuildPipeConfig g_buildPipeConfigs[NUMBUILDINGPIPES];

void UniBuildPipe_RenderCallback(RwResEntry *repEntry, void *object, RwUInt8 type, RwUInt32 flags)
{
    if (object == NULL) return;
    
    RpAtomic *atomic = (RpAtomic*)object;
    RxD3D9ResEntryHeader *header = (RxD3D9ResEntryHeader*)(repEntry + 1);
    RxD3D9InstanceData *inst = (RxD3D9InstanceData*)(header + 1);
    
    int numMeshes = header->numMeshes;
    
    BuildPipeConfig *cfg = &g_buildPipeConfigs[config->buildingPipe];
    
    if (cfg == NULL) return;
    
    float transform[16];
    pipeGetComposedTransformMatrix(atomic, transform);
    UniPipe_SetVSConst(cfg->regTransform, transform, 4);
    
    if (flags & rpGEOMETRYLIGHT) {
        RwMatrix lightmat;
        RwMatrixInvert(&lightmat, RwFrameGetLTM(RpAtomicGetFrame(atomic)));
        pipeUploadLights(&lightmat);
    } else {
        pipeUploadNoLights();
    }
    
    cfg->setupEnv(atomic, NULL, &envmat);
    UniPipe_SetVSConst(cfg->regEnvMat, &envmat, 3);
    
    for (; numMeshes--; inst++) {
        RpMaterial *material = inst->material;
        
        if (inst->material->color.alpha == 0) continue;
        
        TexInfo *texinfo = RwTextureGetTexDBInfo(material->texture);
        
        if (material->pipeline == (RxPipeline*)TagRenderCB) {
            TagRenderCB(atomic, header, inst);
            continue;
        }
        
        D3D9RenderDual(cfg->supportsDualPass && config->dualPassBuilding, header, inst, texinfo);
        
        if (*(int*)&material->surfaceProps.specular & 1) {
            CustomEnvMapPipeMaterialData *envData = *RWPLUGINOFFSET(CustomEnvMapPipeMaterialData*, material, CCustomCarEnvMapPipeline__ms_envMapPluginOffset);
            RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, (void*)rwTEXTUREADDRESSWRAP);
            UniPipe_SetTexture(1, envData->texture);
            
            float fxParams[2];
            fxParams[0] = envData->GetShininess();
            fxParams[1] = 1.0f;
            UniPipe_SetVSConst(cfg->regFxParams, fxParams, 1);
            
            UniPipe_SetVSConst(cfg->regEnvXform, &envXform, 1);
            envXform.z = envData->GetScaleX();
            envXform.w = envData->GetScaleY();
            
            D3D9Render(header, inst);
            
            RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)TRUE);
            RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, (void*)rwTEXTUREADDRESSWRAP);
        }
    }
    
    fixSAMP();
}

void UniBuildPipe_InitConfigs(void)
{
    memset(&g_buildPipeConfigs, 0, sizeof(g_buildPipeConfigs));
    
    g_buildPipeConfigs[BUILDING_PS2] = {
        .baseVS = ps2BuildingVS,
        .basePS = simplePS,
        .windVS = ps2BuildingWindVS,
        .detailPS = simpleDetailPS,
        .detailStochasticPS = simpleDetailStochasticPS,
        .stochasticPS = simpleStochasticPS,
        .fogPS = simpleFogPS,
        .regTransform = REG_transform,
        .regAmbient = REG_ambient,
        .regDirectCol = REG_directCol,
        .regDirectDir = REG_directDir,
        .regMatCol = REG_matCol,
        .regSurfProps = REG_surfProps,
        .regWindPos = REG_windPos,
        .regWindIntensity = REG_windIntensity,
        .regDayParam = REG_dayparam,
        .regNightParam = REG_nightparam,
        .regTexMat = REG_texmat,
        .regFxParams = REG_fxParams,
        .regEnvXform = REG_envXform,
        .regEnvMat = REG_envmat,
        .setupEnv = CustomBuildingEnvMapPipeline__SetupEnv,
        .setDnParams = setDnParams,
        .setWindParams = setWindParams,
        .usesVSConstants = true,
        .supportsDetailMap = true,
        .supportsStochastic = true,
        .supportsEnvMap = true,
        .supportsDualPass = true,
        .supportsWind = true,
    };
    
    g_buildPipeConfigs[BUILDING_XBOX] = {
        .baseVS = xboxBuildingVS,
        .basePS = xboxBuildingPS,
        .windVS = xboxBuildingWindVS,
        .detailPS = simpleDetailPS,
        .detailStochasticPS = simpleDetailStochasticPS,
        .stochasticPS = simpleStochasticPS,
        .fogPS = simpleFogPS,
        .regTransform = REG_transform,
        .regAmbient = REG_ambient,
        .regDirectCol = REG_directCol,
        .regDirectDir = REG_directDir,
        .regMatCol = REG_matCol,
        .regSurfProps = REG_surfProps,
        .regWindPos = REG_windPos,
        .regWindIntensity = REG_windIntensity,
        .regDayParam = REG_dayparam,
        .regNightParam = REG_nightparam,
        .regTexMat = REG_texmat,
        .regFxParams = REG_fxParams,
        .regEnvXform = REG_envXform,
        .regEnvMat = REG_envmat,
        .setupEnv = CustomBuildingEnvMapPipeline__SetupEnv,
        .setDnParams = setDnParams,
        .setWindParams = setWindParams,
        .usesVSConstants = true,
        .supportsDetailMap = true,
        .supportsStochastic = true,
        .supportsEnvMap = true,
        .supportsDualPass = true,
        .supportsWind = true,
    };
}