#pragma once
#include <modules\phys.h>
#include <modules\city.h>

namespace MM2
{
    // Forward declarations
    class lvlSDL;

    // External declarations


    // Class definitions

    class lvlSDL {};

    struct sdlPoly : phPolygon {};

    struct sdlPolyCached : public sdlPoly
    {
        Vector3 Verts[4];
    };
    ASSERT_SIZEOF(sdlPolyCached, 0x90);


    // Lua initialization

}