#include "UniPipe.h"
#include "skygfx.h"
#include "vehiclePipe.cpp"
#include "neoCarpipe.cpp"

VehPipeConfig g_vehPipeConfigs[NUMCARPIPES];
GTAIVVehPipeExtra g_gtaivVehPipeExtra;
GTAVVehPipeExtra g_gtavVehPipeExtra;

void UniVehPipe_RenderCallback(RwResEntry *repEntry, void *object, RwUInt8 type, RwUInt32 flags)
{
    if (object == NULL) return;
    
    RpAtomic *atomic = (RpAtomic*)object;
    RxD3D9ResEntryHeader *header = (RxD3D9ResEntryHeader*)(repEntry + 1);
    RxD3D9InstanceData *inst = (RxD3D9InstanceData*)(header + 1);
    
    int numMeshes = header->numMeshes;
    
    VehPipeConfig *cfg = &g_vehPipeConfigs[config->vehiclePipe];
    
    if (cfg == NULL) return;
    
    float transform[16];
    pipeGetComposedTransformMatrix(atomic, transform);
    UniPipe_SetVSConst(cfg->regTransform, transform, 4);
    
    if (flags & rpGEOMETRYLIGHT) {
        RwMatrix lightmat;
        RwMatrixInvert(&lightmat, RwFrameGetLTM(RpAtomicGetFrame(atomic)));
        cfg->uploadLights(&lightmat);
    } else {
        cfg->uploadNoLights();
    }
    
    for (; numMeshes--; inst++) {
        RpMaterial *material = inst->material;
        
        if (inst->material->color.alpha == 0) continue;
        
        cfg->renderBasePass(header, inst, 1, atomic, flags);
        
        CustomEnvMapPipeMaterialData *envData = *GETENVMAP(material);
        CustomSpecMapPipeMaterialData *specData = *GETSPECMAP(material);
        
        RwUInt32 materialFlags = *(RwUInt32*)&material->surfaceProps.specular;
        bool hasEnv = !!(materialFlags & 3);
        bool hasSpec = !!(materialFlags & 4) && !renderingWheel;
        
        if (RpMatFXMaterialGetEffects(material) != rpMATFXEFFECTENVMAP) {
            hasEnv = false;
            hasSpec = false;
        }
        
        int fxSwitch = 0;
        float shininess = 0.0f;
        float specularity = 0.0f;
        
        if (hasEnv) {
            if (materialFlags & 1) {
                fxSwitch = 1;
                shininess = envData->GetShininess();
            } else if (materialFlags & 2) {
                fxSwitch = 2;
                shininess = envData->GetShininess();
            }
        }
        
        if (hasSpec) {
            specularity = specData->specularity;
            fxSwitch = 1;
        }
        
        if (fxSwitch > 0) {
            RwMatrix envmat;
            RwV4d envXform;
            
            if (fxSwitch == 1) {
                cfg->setupEnv(atomic, carfx_env1Frame, &carfx_env1Inv, &envmat);
                cfg->setupEnv1Xform(atomic, envData, &envXform.x);
            } else {
                cfg->setupEnv(atomic, carfx_env2Frame, &carfx_env2Inv, &envmat);
                CustomEnvMapPipeAtomicData *atmEnvData = CCustomCarEnvMapPipeline__AllocEnvMapPipeAtomicData(atomic);
                cfg->setupEnv2Xform(atomic, &envmat, envData, atmEnvData, &envXform.x);
            }
            
            envXform.z = envData->GetScaleX();
            envXform.w = envData->GetScaleY();
            
            cfg->renderFxPass(header, inst, 1, atomic, flags, envData, specData, NULL, &envmat, &envXform, shininess, specularity, fxSwitch);
        }
    }
    
    fixSAMP();
}

void UniVehPipe_InitConfigs(void)
{
    memset(&g_vehPipeConfigs, 0, sizeof(g_vehPipeConfigs));
    
    g_vehPipeConfigs[CAR_PS2] = {
        .baseVS = vehiclePipeVS,
        .basePS = simplePS,
        .fxVS = ps2CarFxVS,
        .fxPS = ps2EnvSpecFxPS,
        .windVS = NULL,
        .regTransform = REG_transform,
        .regAmbient = REG_ambient,
        .regDirectCol = REG_directCol,
        .regDirectDir = REG_directDir,
        .regMatCol = REG_matCol,
        .regSurfProps = REG_surfProps,
        .regFxParams = REG_fxParams,
        .regEnvXform = REG_envXform,
        .regEnvMat = REG_envmat,
        .regSpecMat = REG_specmat,
        .regLightDir = REG_lightdir,
        .regEye = REG_eye,
        .uploadLights = uploadLights,
        .uploadNoLights = uploadNoLights,
        .setupEnv1Xform = CCustomCarEnvMapPipeline__Env1Xform,
        .setupEnv2Xform = CCustomCarEnvMapPipeline__Env2Xform,
        .setupSpec = CCustomCarEnvMapPipeline__SetupSpec,
        .setupEnv = CCustomCarEnvMapPipeline__SetupEnv,
        .usesVSConstants = true,
        .usesPSConstants = false,
        .supportsSpecular = true,
        .supportsEnvMap = true,
        .supportsDualPass = true,
        .renderBasePass = NULL,
        .renderFxPass = CCustomCarEnvMapPipeline__CustomPipeRenderCB_PS2,
    };
    
    g_vehPipeConfigs[CAR_PC] = {
        .baseVS = vehiclePipeVS,
        .basePS = simplePS,
        .fxVS = specCarFxVS,
        .fxPS = specCarFxPS,
        .windVS = NULL,
        .regTransform = REG_transform,
        .regAmbient = REG_ambient,
        .regDirectCol = REG_directCol,
        .regDirectDir = REG_directDir,
        .regMatCol = REG_matCol,
        .regSurfProps = REG_surfProps,
        .regFxParams = REG_fxParams,
        .regEnvXform = REG_envXform,
        .regEnvMat = REG_envmat,
        .regSpecMat = REG_specmat,
        .regLightDir = REG_lightdir,
        .regEye = REG_eye,
        .uploadLights = uploadLights,
        .uploadNoLights = uploadNoLights,
        .setupEnv1Xform = CCustomCarEnvMapPipeline__Env1Xform_PC,
        .setupEnv2Xform = CCustomCarEnvMapPipeline__Env2Xform_PC,
        .setupSpec = CCustomCarEnvMapPipeline__SetupSpec,
        .setupEnv = CCustomCarEnvMapPipeline__SetupEnv,
        .usesVSConstants = true,
        .usesPSConstants = true,
        .supportsSpecular = true,
        .supportsEnvMap = true,
        .supportsDualPass = true,
        .renderBasePass = NULL,
        .renderFxPass = CCustomCarEnvMapPipeline__CustomPipeRenderCB_exe,
    };
    
    g_vehPipeConfigs[CAR_XBOX] = {
        .baseVS = xboxCarVS,
        .basePS = NULL,
        .fxVS = NULL,
        .fxPS = NULL,
        .windVS = xboxBuildingWindVS,
        .regTransform = LOC_World,
        .regAmbient = LOC_ambient,
        .regDirectCol = LOC_directCol,
        .regDirectDir = LOC_directDir,
        .regMatCol = LOC_matCol,
        .regSurfProps = LOC_surfProps,
        .regFxParams = LOC_fxParams,
        .regEnvXform = LOC_envXform,
        .regEnvMat = LOC_envmat,
        .regSpecMat = LOC_specmat,
        .regLightDir = LOC_lightdir,
        .regEye = LOC_eye,
        .uploadLights = uploadLights,
        .uploadNoLights = uploadNoLights,
        .setupEnv1Xform = CCustomCarEnvMapPipeline__Env1Xform_PC,
        .setupEnv2Xform = CCustomCarEnvMapPipeline__Env2Xform_PC,
        .setupSpec = CCustomCarEnvMapPipeline__SetupSpec,
        .setupEnv = CCustomCarEnvMapPipeline__SetupEnv,
        .usesVSConstants = true,
        .usesPSConstants = false,
        .supportsSpecular = true,
        .supportsEnvMap = true,
        .supportsDualPass = true,
        .renderBasePass = NULL,
        .renderFxPass = CCustomCarEnvMapPipeline__CustomPipeRenderCB_Xbox,
    };
    
    g_vehPipeConfigs[CAR_SPEC] = {
        .baseVS = vehiclePipeVS,
        .basePS = simplePS,
        .fxVS = specCarFxVS,
        .fxPS = specCarFxPS,
        .windVS = NULL,
        .regTransform = REG_transform,
        .regAmbient = REG_ambient,
        .regDirectCol = REG_directCol,
        .regDirectDir = REG_directDir,
        .regMatCol = REG_matCol,
        .regSurfProps = REG_surfProps,
        .regFxParams = REG_fxParams,
        .regEnvXform = REG_envXform,
        .regEnvMat = REG_envmat,
        .regSpecMat = REG_specmat,
        .regLightDir = REG_lightdir,
        .regEye = REG_eye,
        .uploadLights = uploadLights,
        .uploadNoLights = uploadNoLights,
        .setupEnv1Xform = CCustomCarEnvMapPipeline__Env1Xform,
        .setupEnv2Xform = CCustomCarEnvMapPipeline__Env2Xform,
        .setupSpec = CCustomCarEnvMapPipeline__SetupSpec,
        .setupEnv = CCustomCarEnvMapPipeline__SetupEnv,
        .usesVSConstants = true,
        .usesPSConstants = true,
        .supportsSpecular = true,
        .supportsEnvMap = true,
        .supportsDualPass = true,
        .renderBasePass = NULL,
        .renderFxPass = CCustomCarEnvMapPipeline__CustomPipeRenderCB_Specular,
    };
    
    g_vehPipeConfigs[CAR_NEO] = {
        .baseVS = CarPipe::vertexShaderPass1,
        .basePS = NULL,
        .fxVS = CarPipe::vertexShaderPass2,
        .fxPS = NULL,
        .windVS = NULL,
        .regTransform = LOC_combined,
        .regAmbient = LOC_ambient,
        .regDirectCol = LOC_directCol,
        .regDirectDir = LOC_directDir,
        .regMatCol = LOC_matCol,
        .regSurfProps = LOC_surfProps,
        .regFxParams = LOC_reflProps,
        .regEnvXform = 0,
        .regEnvMat = 0,
        .regSpecMat = 0,
        .regLightDir = LOC_lightDir,
        .regEye = LOC_eye,
        .uploadLights = CarPipe::UploadLightColorWithSpecular,
        .uploadNoLights = uploadNoLights,
        .setupEnv1Xform = NULL,
        .setupEnv2Xform = NULL,
        .setupSpec = NULL,
        .setupEnv = NULL,
        .usesVSConstants = true,
        .usesPSConstants = false,
        .supportsSpecular = true,
        .supportsEnvMap = false,
        .supportsDualPass = true,
        .renderBasePass = CarPipe::DiffusePass,
        .renderFxPass = NULL,
    };
    
    g_vehPipeConfigs[CAR_LCS] = {
        .baseVS = vehiclePipeVS,
        .basePS = simplePS,
        .fxVS = leedsCarFxVS,
        .fxPS = NULL,
        .windVS = NULL,
        .regTransform = REG_transform,
        .regAmbient = REG_ambient,
        .regDirectCol = REG_directCol,
        .regDirectDir = REG_directDir,
        .regMatCol = REG_matCol,
        .regSurfProps = REG_surfProps,
        .regFxParams = REG_fxParams,
        .regEnvXform = REG_envXform,
        .regEnvMat = REG_envmat,
        .regSpecMat = REG_specmat,
        .regLightDir = REG_lightdir,
        .regEye = REG_eye,
        .uploadLights = uploadLights,
        .uploadNoLights = uploadNoLights,
        .setupEnv1Xform = CCustomCarEnvMapPipeline__Env1Xform,
        .setupEnv2Xform = CCustomCarEnvMapPipeline__Env2Xform,
        .setupSpec = CCustomCarEnvMapPipeline__SetupSpec,
        .setupEnv = CCustomCarEnvMapPipeline__SetupEnv,
        .usesVSConstants = true,
        .usesPSConstants = true,
        .supportsSpecular = true,
        .supportsEnvMap = true,
        .supportsDualPass = true,
        .renderBasePass = NULL,
        .renderFxPass = CCustomCarEnvMapPipeline__CustomPipeRenderCB_leeds,
    };
    
    g_vehPipeConfigs[CAR_VCS] = {
        .baseVS = vehiclePipeVS,
        .basePS = simplePS,
        .fxVS = leedsCarFxVS,
        .fxPS = NULL,
        .windVS = NULL,
        .regTransform = REG_transform,
        .regAmbient = REG_ambient,
        .regDirectCol = REG_directCol,
        .regDirectDir = REG_directDir,
        .regMatCol = REG_matCol,
        .regSurfProps = REG_surfProps,
        .regFxParams = REG_fxParams,
        .regEnvXform = REG_envXform,
        .regEnvMat = REG_envmat,
        .regSpecMat = REG_specmat,
        .regLightDir = REG_lightdir,
        .regEye = REG_eye,
        .uploadLights = uploadLights,
        .uploadNoLights = uploadNoLights,
        .setupEnv1Xform = CCustomCarEnvMapPipeline__Env1Xform,
        .setupEnv2Xform = CCustomCarEnvMapPipeline__Env2Xform,
        .setupSpec = CCustomCarEnvMapPipeline__SetupSpec,
        .setupEnv = CCustomCarEnvMapPipeline__SetupEnv,
        .usesVSConstants = true,
        .usesPSConstants = true,
        .supportsSpecular = true,
        .supportsEnvMap = true,
        .supportsDualPass = true,
        .renderBasePass = NULL,
        .renderFxPass = CCustomCarEnvMapPipeline__CustomPipeRenderCB_leeds,
    };
    
    g_vehPipeConfigs[CAR_MOBILE] = {
        .baseVS = vehiclePipeVS,
        .basePS = simplePS,
        .fxVS = mobileVehiclePipeVS,
        .fxPS = mobileVehiclePipePS,
        .windVS = NULL,
        .regTransform = REG_transform,
        .regAmbient = REG_ambient,
        .regDirectCol = REG_directCol,
        .regDirectDir = REG_directDir,
        .regMatCol = REG_matCol,
        .regSurfProps = REG_surfProps,
        .regFxParams = REG_fxParams,
        .regEnvXform = REG_envXform,
        .regEnvMat = REG_envmat,
        .regSpecMat = REG_specmat,
        .regLightDir = REG_lightdir,
        .regEye = REG_eye,
        .uploadLights = uploadLights,
        .uploadNoLights = uploadNoLights,
        .setupEnv1Xform = CCustomCarEnvMapPipeline__Env1Xform,
        .setupEnv2Xform = CCustomCarEnvMapPipeline__Env2Xform,
        .setupSpec = CCustomCarEnvMapPipeline__SetupSpec,
        .setupEnv = CCustomCarEnvMapPipeline__SetupEnv,
        .usesVSConstants = true,
        .usesPSConstants = true,
        .supportsSpecular = true,
        .supportsEnvMap = true,
        .supportsDualPass = true,
        .renderBasePass = NULL,
        .renderFxPass = CCustomCarEnvMapPipeline__CustomPipeRenderCB_mobile,
    };
    
    g_vehPipeConfigs[CAR_ENV] = {
        .baseVS = envCarVS,
        .basePS = envCarPS,
        .fxVS = NULL,
        .fxPS = NULL,
        .windVS = NULL,
        .regTransform = REG_transform,
        .regAmbient = REG_ambient,
        .regDirectCol = REG_directCol,
        .regDirectDir = REG_directDir,
        .regMatCol = REG_matCol,
        .regSurfProps = REG_surfProps,
        .regFxParams = REG_fxParams,
        .regEnvXform = REG_envXform,
        .regEnvMat = REG_envmat,
        .regSpecMat = REG_specmat,
        .regLightDir = REG_lightdir,
        .regEye = REG_eye,
        .uploadLights = uploadLights,
        .uploadNoLights = uploadNoLights,
        .setupEnv1Xform = CCustomCarEnvMapPipeline__Env1Xform,
        .setupEnv2Xform = CCustomCarEnvMapPipeline__Env2Xform,
        .setupSpec = CCustomCarEnvMapPipeline__SetupSpec,
        .setupEnv = CCustomCarEnvMapPipeline__SetupEnv,
        .usesVSConstants = true,
        .usesPSConstants = true,
        .supportsSpecular = false,
        .supportsEnvMap = true,
        .supportsDualPass = true,
        .renderBasePass = NULL,
        .renderFxPass = CCustomCarEnvMapPipeline__CustomPipeRenderCB_Env,
    };
    
    g_gtaivVehPipeExtra = {
        .pipeType = IV_PIPE_FORWARD_PLUS,
        .exposure = 1.0f,
        .bloomThreshold = 0.8f,
        .bloomIntensity = 1.5f,
        .forwardPlusVS = gtaivVehicleVS,
        .forwardPlusPS = gtaivVehiclePS,
        .deferredVS = NULL,
        .deferredPS = NULL,
        .lightCullingCS = NULL,
        .clusteredLightCS = NULL,
    };
    
    g_gtavVehPipeExtra = {
        .pipeType = V_PIPE_PBR,
        .exposure = 1.2f,
        .metalness = 0.5f,
        .roughness = 0.4f,
        .pbrVS = gtaivVehicleVS,
        .pbrPS = gtaivVehiclePS,
        .raytracedVS = NULL,
        .raytracedPS = NULL,
        .rtReflectionPS = NULL,
        .clusteredLightCS = NULL,
        .ssrPS = NULL,
    };
}

void UniVehPipe_InitGTAIVConfig(void)
{
    g_vehPipeConfigs[CAR_IV] = {
        .baseVS = g_gtaivVehPipeExtra.forwardPlusVS,
        .basePS = g_gtaivVehPipeExtra.forwardPlusPS,
        .fxVS = NULL,
        .fxPS = NULL,
        .windVS = NULL,
        .regTransform = 0,
        .regAmbient = 0,
        .regDirectCol = 0,
        .regDirectDir = 0,
        .regMatCol = 0,
        .regSurfProps = 0,
        .regFxParams = 0,
        .regEnvXform = 0,
        .regEnvMat = 0,
        .regSpecMat = 0,
        .regLightDir = 0,
        .regEye = 0,
        .uploadLights = uploadLights,
        .uploadNoLights = uploadNoLights,
        .setupEnv1Xform = NULL,
        .setupEnv2Xform = NULL,
        .setupSpec = NULL,
        .setupEnv = NULL,
        .usesVSConstants = true,
        .usesPSConstants = true,
        .supportsSpecular = true,
        .supportsEnvMap = false,
        .supportsDualPass = true,
        .renderBasePass = NULL,
        .renderFxPass = NULL,
    };
}

void UniVehPipe_InitGTAVConfig(void)
{
    g_vehPipeConfigs[CAR_V] = {
        .baseVS = g_gtavVehPipeExtra.pbrVS,
        .basePS = g_gtavVehPipeExtra.pbrPS,
        .fxVS = NULL,
        .fxPS = NULL,
        .windVS = NULL,
        .regTransform = 0,
        .regAmbient = 0,
        .regDirectCol = 0,
        .regDirectDir = 0,
        .regMatCol = 0,
        .regSurfProps = 0,
        .regFxParams = 0,
        .regEnvXform = 0,
        .regEnvMat = 0,
        .regSpecMat = 0,
        .regLightDir = 0,
        .regEye = 0,
        .uploadLights = uploadLights,
        .uploadNoLights = uploadNoLights,
        .setupEnv1Xform = NULL,
        .setupEnv2Xform = NULL,
        .setupSpec = NULL,
        .setupEnv = NULL,
        .usesVSConstants = true,
        .usesPSConstants = true,
        .supportsSpecular = true,
        .supportsEnvMap = false,
        .supportsDualPass = true,
        .renderBasePass = NULL,
        .renderFxPass = NULL,
    };
}