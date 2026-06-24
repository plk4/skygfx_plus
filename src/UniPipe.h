#pragma once

#include "skygfx.h"

struct VehPipeConfig {
    void *baseVS;
    void *basePS;
    void *fxVS;
    void *fxPS;
    void *windVS;
    int regTransform;
    int regAmbient;
    int regDirectCol;
    int regDirectDir;
    int regMatCol;
    int regSurfProps;
    int regFxParams;
    int regEnvXform;
    int regEnvMat;
    int regSpecMat;
    int regLightDir;
    int regEye;
    void (*uploadLights)(RwMatrix *lightmat);
    void (*uploadNoLights)(void);
    void (*setupEnv1Xform)(RpAtomic *atomic, CustomEnvMapPipeMaterialData *envData, float *envXform);
    void (*setupEnv2Xform)(RpAtomic *atomic, RwMatrix *envmat, CustomEnvMapPipeMaterialData *envData, CustomEnvMapPipeAtomicData *atmEnvData, float *envXform);
    void (*setupSpec)(RpAtomic *atomic, RwMatrix *specmat, RwV3d *specdir);
    void (*setupEnv)(RpAtomic *atomic, RwFrame *envframe, RwMatrix *frminv, RwMatrix *envmat);
    bool usesVSConstants;
    bool usesPSConstants;
    bool supportsSpecular;
    bool supportsEnvMap;
    bool supportsDualPass;
    void (*renderBasePass)(RxD3D9ResEntryHeader *header, RxD3D9InstanceData *inst, int numMeshes, RpAtomic *atomic, RwUInt32 flags);
    void (*renderFxPass)(RxD3D9ResEntryHeader *header, RxD3D9InstanceData *inst, int numMeshes, RpAtomic *atomic, RwUInt32 flags, CustomEnvMapPipeMaterialData *envData, CustomSpecMapPipeMaterialData *specData, CustomEnvMapPipeAtomicData *atmEnvData, RwMatrix *envmat, RwV4d *envXform, float shininess, float specularity, int fxSwitch);
};

struct BuildPipeConfig {
    void *baseVS;
    void *basePS;
    void *windVS;
    void *detailPS;
    void *detailStochasticPS;
    void *stochasticPS;
    void *fogPS;
    int regTransform;
    int regAmbient;
    int regDirectCol;
    int regDirectDir;
    int regMatCol;
    int regSurfProps;
    int regWindPos;
    int regWindIntensity;
    int regDayParam;
    int regNightParam;
    int regTexMat;
    int regFxParams;
    int regEnvXform;
    int regEnvMat;
    void (*setupEnv)(RpAtomic *atomic, RwFrame *envframe, RwMatrix *envmat);
    void (*setDnParams)(RpAtomic *atomic);
    void (*setWindParams)(RpAtomic *atomic, RwFrame *frame);
    bool usesVSConstants;
    bool supportsDetailMap;
    bool supportsStochastic;
    bool supportsEnvMap;
    bool supportsDualPass;
    bool supportsWind;
};

struct VegPipeConfig {
    void *pixelShader;
    void *stochasticPS;
    void (*renderCallback)(RpAtomic *atomic);
    bool ps2Modulate;
    bool dualPass;
    bool backfaceCull;
    bool addAmbient;
    int zwriteThreshold;
};

struct PostFXConfig {
    void *colorFilterPS;
    void *blurPS;
    void *radiosityPS;
    void *trailsPS;
    void *gradingPS;
    void *contrastPS;
    void *ssaoPS;
    void *smaaPS;
    void *gtaivPS;
    void (*applyColorFilter)(RwRGBA rgb1, RwRGBA rgb2);
    void (*applyRadiosity)(int intensityLimit, int filterPasses, int renderPasses, int intensity);
    void (*applyBlur)(void);
    void (*applyGrain)(int strength, bool generate);
    void (*applyInfrared)(RwRGBA c1, RwRGBA c2);
    void (*applyNightVision)(RwRGBA color);
    void (*applyYCbCr)(void);
    void (*applySMAA)(void);
    void (*applySSAO)(void);
    bool usesYCbCr;
    bool usesRadiosity;
    bool usesTrails;
    bool usesSMAA;
    bool usesSSAO;
    bool usesGTAIV;
};

extern VehPipeConfig g_vehPipeConfigs[NUMCARPIPES];
extern BuildPipeConfig g_buildPipeConfigs[NUMBUILDINGPIPES];
extern VegPipeConfig g_vegPipeConfig;
extern PostFXConfig g_postFXConfigs[8]; // COLORFILTER_NONE to COLORFILTER_GTAIV

void UniVehPipe_RenderCallback(RwResEntry *repEntry, void *object, RwUInt8 type, RwUInt32 flags);
void UniVehPipe_InitConfigs(void);

void UniBuildPipe_RenderCallback(RwResEntry *repEntry, void *object, RwUInt8 type, RwUInt32 flags);
void UniBuildPipe_InitConfigs(void);

void UniVegPipe_Init(void);
void UniVegPipe_RenderCallback(RpAtomic *atomic);

void UniPostFX_ApplyColorFilter(RwRGBA rgb1, RwRGBA rgb2);
void UniPostFX_ApplyRadiosity(int intensityLimit, int filterPasses, int renderPasses, int intensity);
void UniPostFX_ApplyBlur(void);
void UniPostFX_ApplyFinalEffects(void);
void UniPostFX_InitConfigs(void);

inline void UniPipe_SetTexture(int stage, RwTexture *tex) {
    RwD3D9SetTexture(tex ? tex : gpWhiteTexture, stage);
}

inline void UniPipe_SetVSConst(int reg, void *data, int count) {
    RwD3D9SetVertexShaderConstant(reg, data, count);
}

inline void UniPipe_SetPSConst(int reg, void *data, int count) {
    RwD3D9SetPixelShaderConstant(reg, data, count);
}

inline void UniPipe_RenderDual(int dual, RxD3D9ResEntryHeader *header, RxD3D9InstanceData *inst, TexInfo *texInfo = nullptr) {
    D3D9RenderDual(dual, header, inst, texInfo);
}

#define UNI_PIPE_REG(name) cfg->reg##name

#define UNI_VEH_PIPE_BEGIN(cfg, atomic, flags) \
    RpAtomic *atomic = (RpAtomic*)object; \
    RxD3D9ResEntryHeader *header = (RxD3D9ResEntryHeader*)(repEntry + 1); \
    RxD3D9InstanceData *inst = (RxD3D9InstanceData*)(header + 1); \
    float transform[16]; \
    pipeGetComposedTransformMatrix(atomic, transform); \
    UniPipe_SetVSConst(cfg->regTransform, transform, 4); \
    if(flags & rpGEOMETRYLIGHT) { \
        RwMatrix lightmat; \
        RwMatrixInvert(&lightmat, RwFrameGetLTM(RpAtomicGetFrame(atomic))); \
        cfg->uploadLights(&lightmat); \
    } else { \
        cfg->uploadNoLights(); \
    }

#define UNI_VEH_PIPE_BASE_PASS(cfg, header, inst, numMeshes, atomic, flags) \
    if(cfg->renderBasePass) \
        cfg->renderBasePass(header, inst, numMeshes, atomic, flags)

#define UNI_VEH_PIPE_FX_PASS(cfg, header, inst, numMeshes, atomic, flags, envData, specData, atmEnvData, envmat, envXform, shininess, specularity, fxSwitch) \
    if(cfg->renderFxPass && cfg->supportsEnvMap) \
        cfg->renderFxPass(header, inst, numMeshes, atomic, flags, envData, specData, atmEnvData, envmat, envXform, shininess, specularity, fxSwitch)

enum GTAIV_VehPipeType {
    IV_PIPE_STANDARD = 0,
    IV_PIPE_FORWARD_PLUS = 1,
    IV_PIPE_DEFERRED = 2,
};

enum GTAV_VehPipeType {
    V_PIPE_STANDARD = 0,
    V_PIPE_PBR = 1,
    V_PIPE_RAYTRACED = 2,
};

struct GTAIVVehPipeExtra {
    GTAIV_VehPipeType pipeType;
    float exposure;
    float bloomThreshold;
    float bloomIntensity;
    void *forwardPlusVS;
    void *forwardPlusPS;
    void *deferredVS;
    void *deferredPS;
    void *lightCullingCS;
    void *clusteredLightCS;
};

struct GTAVVehPipeExtra {
    GTAV_VehPipeType pipeType;
    float exposure;
    float metalness;
    float roughness;
    void *pbrVS;
    void *pbrPS;
    void *raytracedVS;
    void *raytracedPS;
    void *rtReflectionPS;
    void *clusteredLightCS;
    void *ssrPS;
};

extern GTAIVVehPipeExtra g_gtaivVehPipeExtra;
extern GTAVVehPipeExtra g_gtavVehPipeExtra;

void UniVehPipe_InitGTAIVConfig(void);
void UniVehPipe_InitGTAVConfig(void);

struct PipeShaderRegistry {
    const char *name;
    int resourceId;
    void **vsOut;
    void **psOut;
};

extern PipeShaderRegistry g_vehShaders[];
extern PipeShaderRegistry g_buildShaders[];
extern PipeShaderRegistry g_postfxShaders[];

void UniPipe_LoadShaders(PipeShaderRegistry *registry, int count);
void *UniPipe_GetShader(const char *name, bool isVS);

struct PipeDebugParams {
    bool wireframe;
    bool showNormals;
    bool showUVs;
    bool showTangents;
    bool disableEnvMap;
    bool disableSpecular;
    bool disableDualPass;
    bool forceShader;
    int forcedShaderIdx;
    float envMapScale;
    float specularScale;
    float fresnelBias;
    float powerBias;
};

extern PipeDebugParams g_pipeDebug;

#define UNI_PIPE_DBG(name) g_pipeDebug.name

struct PipeVersionInfo {
    int major;
    int minor;
    int patch;
    const char *buildDate;
    const char *gitHash;
    bool isUniPipe;
};

extern PipeVersionInfo g_pipeVersion;

inline bool UniPipe_IsUniPipe(void) { return g_pipeVersion.isUniPipe; }
inline int UniPipe_GetVersion(void) { return (g_pipeVersion.major << 16) | (g_pipeVersion.minor << 8) | g_pipeVersion.patch; }

#define UNI_PIPE_VER 0x010000


