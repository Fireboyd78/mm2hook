#include "mm2_gfx.h"
using namespace MM2;

/*
    gfxTexture
*/
DECLARE_HOOK(gfxTexture::sm_EnableSetLOD);
DECLARE_HOOK(gfxTexture::sm_Allow32);