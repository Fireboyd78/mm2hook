#include "mm2_gfx.h"
using namespace MM2;

/*
    gfxTexture
*/
HOOK_DECLARE(0x684D34, gfxTexture::sm_EnableSetLOD);
HOOK_DECLARE(0x684D36, gfxTexture::sm_Allow32);