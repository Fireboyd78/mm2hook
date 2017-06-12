#include "mm2_gfx.h"
using namespace MM2;

/*
    gfxTexture
*/
DECLARE_HOOK(gfxTexture::sm_EnableSetLOD)(0x684D34);
DECLARE_HOOK(gfxTexture::sm_Allow32)(0x684D36);