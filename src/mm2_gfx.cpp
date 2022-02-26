#include "mm2_gfx.h"
using namespace MM2;

/*
    gfxTextureCachePool
*/
declfield(gfxTextureCachePool::sm_FirstPool)(0x68452C);

/*
    gfxTexture
*/

declfield(gfxTexture::sm_First)                 = 0x684D38;
declfield(gfxTexture::sm_UseInternalCache)      = 0x684524;
declfield(gfxTexture::sm_EnableSetLOD)          = 0x684D34;
declfield(gfxTexture::sm_Allow32)               = 0x684D36;

/*
    gfxRenderState
*/

declfield(gfxRenderState::sm_Camera)            = 0x685460;
declfield(gfxRenderState::sm_Composite)         = 0x6854F8;
declfield(gfxRenderState::sm_World)             = 0x685538;
declfield(gfxRenderState::sm_FullComposite)     = 0x6855E0;
declfield(gfxRenderState::sm_Transform)         = 0x685620;
declfield(gfxRenderState::sm_View)              = 0x685660;
declfield(gfxRenderState::sm_Modelview)         = 0x685738;
declfield(gfxRenderState::m_Touched)            = 0x685778;
declfield(gfxRenderState::m_TouchedMask)        = 0x685780;
declfield(gfxRenderState::m_TouchedMasks)       = 0x5CD604;

/*
    ltLight
*/
declfield(ltLight::GlowScale)                   = 0x5DDE9C;