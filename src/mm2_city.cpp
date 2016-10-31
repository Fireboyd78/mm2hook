#include "mm2_city.h"
using namespace MM2;

/*
    sdlPage16
*/
DECLARE_HOOK(sdlPage16::$$ctor);

DECLARE_HOOK(sdlPage16::$Draw);

DECLARE_HOOK(sdlPage16::$ArcMap);
DECLARE_HOOK(sdlPage16::$WallMap);
DECLARE_HOOK(sdlPage16::$PointInPerimeter);

DECLARE_HOOK(sdlPage16::$GetCentroid);
DECLARE_HOOK(sdlPage16::$GetPerimeterCount);
DECLARE_HOOK(sdlPage16::$GetPerimeterVertexIndex);
DECLARE_HOOK(sdlPage16::$GetCodedVertex);
DECLARE_HOOK(sdlPage16::$GetFloat);
DECLARE_HOOK(sdlPage16::$GetTexture);

DECLARE_HOOK(sdlPage16::$GetShadedColor_$1);
DECLARE_HOOK(sdlPage16::$GetShadedColor_$2);

DECLARE_HOOK(sdlPage16::$LoadBinary);