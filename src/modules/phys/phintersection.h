#pragma once
#include <modules\phys.h>

namespace MM2
{
    // Forward declarations
    struct phIntersection;
    struct lvlIntersection;

    // External declarations
    extern class phBound;
    extern struct phPolygon;
    extern struct phIntersectionPoint;

    // Class definitions

    struct phIntersection
    {
        phIntersectionPoint IntersectionPoint;
        int PolyIndex;
        int field_28;
        int field_2C;
        int field_30;
        Vector3 StartPos;
        Vector3 EndPos;
        Vector3 field_4C;
        phPolygon* Poly;
        int MaterialIndex;
        int field_60;
        int field_64;
        int field_68;
        phBound* Bound;
        float field_70;
        int field_74;
        float field_78;
        Vector3 field_7C;
        Vector3 field_88;
        float field_94;
        short Flags;
    };
    ASSERT_SIZEOF(phIntersection, 0x9C);

    struct lvlIntersection : public phIntersection {};

    // Lua initialization

}