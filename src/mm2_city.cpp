#include "mm2_city.h"
using namespace MM2;

/*
    sdlCommon
*/
HOOK_DECLARE(0x62B090, sdlCommon::sm_CamPos);
HOOK_DECLARE(0x62B0A0, sdlCommon::sm_LightTable);