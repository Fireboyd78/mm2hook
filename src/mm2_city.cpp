#include "mm2_city.h"
using namespace MM2;

/*
    sdlCommon
*/
DECLARE_HOOK(sdlCommon::sm_CamPos)      (0x62B090);
DECLARE_HOOK(sdlCommon::sm_LightTable)  (0x62B0A0);