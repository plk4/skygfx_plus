#include "UniPipe.h"
#include "skygfx.h"
#include "main.cpp"

VegPipeConfig g_vegPipeConfig;

void UniVegPipe_Init(void)
{
    memset(&g_vegPipeConfig, 0, sizeof(g_vegPipeConfig));
    
    g_vegPipeConfig.pixelShader = grassPixelShader;
    g_vegPipeConfig.stochasticPS = simpleStochasticPS;
    g_vegPipeConfig.renderCallback = grassRenderCallback;
    g_vegPipeConfig.ps2Modulate = config->ps2ModulateGrass;
    g_vegPipeConfig.dualPass = config->dualPassGrass;
    g_vegPipeConfig.backfaceCull = config->backfaceCull;
    g_vegPipeConfig.addAmbient = config->grassAddAmbient;
    g_vegPipeConfig.zwriteThreshold = config->zwriteThresholdGrass;
}

void UniVegPipe_RenderCallback(RpAtomic *atomic)
{
    if (atomic == NULL) return;
    
    if (g_vegPipeConfig.renderCallback) {
        g_vegPipeConfig.renderCallback(atomic);
    }
}