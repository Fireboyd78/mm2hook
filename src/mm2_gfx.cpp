#include "mm2_gfx.h"
using namespace MM2;

/*
    gfxLight
*/
DECLARE_HOOK(gfxLight::$Reset);

/*
    gfxTexture
*/
DECLARE_HOOK(gfxTexture::sm_EnableSetLOD);
DECLARE_HOOK(gfxTexture::sm_Allow32);