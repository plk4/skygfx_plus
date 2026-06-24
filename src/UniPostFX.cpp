#include "UniPipe.h"
#include "skygfx.h"
#include "postfx.cpp"

PostFXConfig g_postFXConfigs[8];

void UniPostFX_InitConfigs(void)
{
    memset(&g_postFXConfigs, 0, sizeof(g_postFXConfigs));
    
    g_postFXConfigs[COLORFILTER_PS2] = {
        .colorFilterPS = ps2EnvSpecFxPS,
        .blurPS = blurPS,
        .radiosityPS = radiosityPS,
        .trailsPS = vcTrailsPS,
        .gradingPS = gradingPS,
        .contrastPS = contrastPS,
        .ssaoPS = SSAO,
        .smaaPS = SMAA,
        .gtaivPS = GTAIV_PS,
        .applyColorFilter = CPostEffects::ColourFilter_PS2,
        .applyRadiosity = CPostEffects::Radiosity,
        .applyBlur = CPostEffects::Blur_VCS,
        .applyGrain = CPostEffects::Grain_PS2,
        .applyInfrared = CPostEffects::InfraredVision_PS2,
        .applyNightVision = CPostEffects::NightVision_PS2,
        .applyYCbCr = NULL,
        .applySMAA = DrawSMAA,
        .applySSAO = DrawSSAO,
        .applyGTAIV = NULL,
        .usesYCbCr = false,
        .usesRadiosity = true,
        .usesTrails = true,
        .usesSMAA = true,
        .usesSSAO = true,
        .usesGTAIV = false,
    };
    
    g_postFXConfigs[COLORFILTER_PC] = {
        .colorFilterPS = ps2EnvSpecFxPS,
        .blurPS = blurPS,
        .radiosityPS = radiosityPS,
        .trailsPS = vcTrailsPS,
        .gradingPS = gradingPS,
        .contrastPS = contrastPS,
        .ssaoPS = SSAO,
        .smaaPS = SMAA,
        .gtaivPS = GTAIV_PS,
        .applyColorFilter = CPostEffects::ColourFilter,
        .applyRadiosity = CPostEffects::Radiosity,
        .applyBlur = CPostEffects::Blur_VCS,
        .applyGrain = CPostEffects::Grain,
        .applyInfrared = CPostEffects::InfraredVision,
        .applyNightVision = CPostEffects::NightVision,
        .applyYCbCr = CPostEffects::DrawFinalEffects,
        .applySMAA = DrawSMAA,
        .applySSAO = DrawSSAO,
        .applyGTAIV = NULL,
        .usesYCbCr = true,
        .usesRadiosity = true,
        .usesTrails = true,
        .usesSMAA = true,
        .usesSSAO = true,
        .usesGTAIV = false,
    };
    
    g_postFXConfigs[COLORFILTER_MOBILE] = {
        .colorFilterPS = ps2EnvSpecFxPS,
        .blurPS = blurPS,
        .radiosityPS = radiosityPS,
        .trailsPS = vcTrailsPS,
        .gradingPS = gradingPS,
        .contrastPS = contrastPS,
        .ssaoPS = SSAO,
        .smaaPS = SMAA,
        .gtaivPS = GTAIV_PS,
        .applyColorFilter = CPostEffects::ColourFilter_Mobile,
        .applyRadiosity = CPostEffects::Radiosity,
        .applyBlur = CPostEffects::Blur_VCS,
        .applyGrain = CPostEffects::Grain,
        .applyInfrared = CPostEffects::InfraredVision,
        .applyNightVision = CPostEffects::NightVision,
        .applyYCbCr = NULL,
        .applySMAA = DrawSMAA,
        .applySSAO = DrawSSAO,
        .applyGTAIV = NULL,
        .usesYCbCr = false,
        .usesRadiosity = true,
        .usesTrails = true,
        .usesSMAA = true,
        .usesSSAO = true,
        .usesGTAIV = false,
    };
    
    g_postFXConfigs[COLORFILTER_III] = {
        .colorFilterPS = ps2EnvSpecFxPS,
        .blurPS = blurPS,
        .radiosityPS = radiosityPS,
        .trailsPS = iiiTrailsPS,
        .gradingPS = gradingPS,
        .contrastPS = contrastPS,
        .ssaoPS = SSAO,
        .smaaPS = SMAA,
        .gtaivPS = GTAIV_PS,
        .applyColorFilter = CPostEffects::ColourFilter_Generic,
        .applyRadiosity = CPostEffects::Radiosity,
        .applyBlur = CPostEffects::Blur_VCS,
        .applyGrain = CPostEffects::Grain,
        .applyInfrared = CPostEffects::InfraredVision,
        .applyNightVision = CPostEffects::NightVision,
        .applyYCbCr = NULL,
        .applySMAA = DrawSMAA,
        .applySSAO = DrawSSAO,
        .applyGTAIV = NULL,
        .usesYCbCr = false,
        .usesRadiosity = true,
        .usesTrails = true,
        .usesSMAA = true,
        .usesSSAO = true,
        .usesGTAIV = false,
    };
    
    g_postFXConfigs[COLORFILTER_VC] = {
        .colorFilterPS = ps2EnvSpecFxPS,
        .blurPS = blurPS,
        .radiosityPS = radiosityPS,
        .trailsPS = vcTrailsPS,
        .gradingPS = gradingPS,
        .contrastPS = contrastPS,
        .ssaoPS = SSAO,
        .smaaPS = SMAA,
        .gtaivPS = GTAIV_PS,
        .applyColorFilter = CPostEffects::ColourFilter_Generic,
        .applyRadiosity = CPostEffects::Radiosity,
        .applyBlur = CPostEffects::Blur_VCS,
        .applyGrain = CPostEffects::Grain,
        .applyInfrared = CPostEffects::InfraredVision,
        .applyNightVision = CPostEffects::NightVision,
        .applyYCbCr = NULL,
        .applySMAA = DrawSMAA,
        .applySSAO = DrawSSAO,
        .applyGTAIV = NULL,
        .usesYCbCr = false,
        .usesRadiosity = true,
        .usesTrails = true,
        .usesSMAA = true,
        .usesSSAO = true,
        .usesGTAIV = false,
    };
    
    g_postFXConfigs[COLORFILTER_VCS] = {
        .colorFilterPS = ps2EnvSpecFxPS,
        .blurPS = blurPS,
        .radiosityPS = radiosityPS,
        .trailsPS = vcTrailsPS,
        .gradingPS = gradingPS,
        .contrastPS = contrastPS,
        .ssaoPS = SSAO,
        .smaaPS = SMAA,
        .gtaivPS = GTAIV_PS,
        .applyColorFilter = CPostEffects::ColourFilter_Generic,
        .applyRadiosity = CPostEffects::Radiosity,
        .applyBlur = CPostEffects::Blur_VCS,
        .applyGrain = CPostEffects::Grain,
        .applyInfrared = CPostEffects::InfraredVision,
        .applyNightVision = CPostEffects::NightVision,
        .applyYCbCr = NULL,
        .applySMAA = DrawSMAA,
        .applySSAO = DrawSSAO,
        .applyGTAIV = NULL,
        .usesYCbCr = false,
        .usesRadiosity = true,
        .usesTrails = true,
        .usesSMAA = true,
        .usesSSAO = true,
        .usesGTAIV = false,
    };
    
    g_postFXConfigs[COLORFILTER_GTAIV] = {
        .colorFilterPS = ps2EnvSpecFxPS,
        .blurPS = blurPS,
        .radiosityPS = radiosityPS,
        .trailsPS = vcTrailsPS,
        .gradingPS = gradingPS,
        .contrastPS = contrastPS,
        .ssaoPS = SSAO,
        .smaaPS = SMAA,
        .gtaivPS = GTAIV_PS,
        .applyColorFilter = CPostEffects::ColourFilter_Generic,
        .applyRadiosity = CPostEffects::Radiosity,
        .applyBlur = CPostEffects::Blur_VCS,
        .applyGrain = CPostEffects::Grain,
        .applyInfrared = CPostEffects::InfraredVision,
        .applyNightVision = CPostEffects::NightVision,
        .applyYCbCr = NULL,
        .applySMAA = DrawSMAA,
        .applySSAO = DrawSSAO,
        .applyGTAIV = CPostEffects::DrawFinalEffects,
        .usesYCbCr = false,
        .usesRadiosity = true,
        .usesTrails = true,
        .usesSMAA = true,
        .usesSSAO = true,
        .usesGTAIV = true,
    };
    
    g_postFXConfigs[COLORFILTER_NONE] = {
        .colorFilterPS = NULL,
        .blurPS = NULL,
        .radiosityPS = NULL,
        .trailsPS = NULL,
        .gradingPS = NULL,
        .contrastPS = NULL,
        .ssaoPS = NULL,
        .smaaPS = NULL,
        .gtaivPS = NULL,
        .applyColorFilter = NULL,
        .applyRadiosity = NULL,
        .applyBlur = NULL,
        .applyGrain = NULL,
        .applyInfrared = NULL,
        .applyNightVision = NULL,
        .applyYCbCr = NULL,
        .applySMAA = NULL,
        .applySSAO = NULL,
        .applyGTAIV = NULL,
        .usesYCbCr = false,
        .usesRadiosity = false,
        .usesTrails = false,
        .usesSMAA = false,
        .usesSSAO = false,
        .usesGTAIV = false,
    };
}

void UniPostFX_ApplyColorFilter(RwRGBA rgb1, RwRGBA rgb2)
{
    PostFXConfig *cfg = &g_postFXConfigs[config->colorFilter];
    if (cfg->applyColorFilter) {
        cfg->applyColorFilter(rgb1, rgb2);
    }
}

void UniPostFX_ApplyRadiosity(int intensityLimit, int filterPasses, int renderPasses, int intensity)
{
    PostFXConfig *cfg = &g_postFXConfigs[config->colorFilter];
    if (cfg->applyRadiosity) {
        cfg->applyRadiosity(intensityLimit, filterPasses, renderPasses, intensity);
    }
}

void UniPostFX_ApplyBlur(void)
{
    PostFXConfig *cfg = &g_postFXConfigs[config->colorFilter];
    if (cfg->applyBlur) {
        cfg->applyBlur();
    }
}

void UniPostFX_ApplyFinalEffects(void)
{
    PostFXConfig *cfg = &g_postFXConfigs[config->colorFilter];
    if (cfg->applyYCbCr) {
        cfg->applyYCbCr();
    }
}