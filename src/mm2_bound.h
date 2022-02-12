#pragma once
#include "mm2_common.h"

#include <modules\phys.h>

namespace MM2
{
    // Extern declarations


    // Forward declarations


    // Class definitions


    template<>
    void luaAddModule<module_bound>(LuaState L) {
        luaBind<phBound>(L);
        luaBind<phHotEdge>(L);
        luaBind<phBoundPolygonal>(L);
        luaBind<phBoundGeometry>(L);
        luaBind<phBoundBox>(L);
        luaBind<phBoundSphere>(L);
        luaBind<phBoundHotdog>(L);
        luaBind<phBoundTerrain>(L);
        luaBind<phBoundTerrainLocal>(L);
    }
}