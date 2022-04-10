#pragma once
#include <modules\phys.h>
#include <modules\city.h>

namespace MM2
{
    // Forward declarations
    class lvlSDL;

    // External declarations


    // Class definitions

    class sdlCommon : public lvlLevelBound {
    public:
        static hook::Type<int> sm_RoomCount;
        static hook::Type<Vector3> sm_CamPos;
        static hook::Type<uint> sm_LightTable;

        AGE_API static void UpdateLighting()
        {
            hook::StaticThunk<0x448090>::Call<void>();
        }

        AGE_API static bool BACKFACE(const Vector3& vec1, const Vector3& vec2)
        {
            return hook::StaticThunk<0x45CBC0>::Call<bool>(&vec1, &vec2);
        }

        AGE_API virtual int CollideEdgePoint(phSegment& a1, lvlIntersectionPoint* a2, int a3)
        {
            return hook::Thunk<0x4482E0>::Call<int>(this, &a1, a2, a3);
        }

        AGE_API virtual bool CollideProbePoint(phSegment& a1, lvlIntersectionPoint* a2, float a3)
        {
            return hook::Thunk<0x4482F0>::Call<bool>(this, &a1, a2, a3);
        }

        AGE_API virtual bool CollideAIPoint(phSegment& a1, lvlIntersectionPoint* a2)
        {
            return hook::Thunk<0x448300>::Call<bool>(this, &a1, a2);
        }
    };

    class lvlSDL : public sdlCommon {
    public:
        AGE_API virtual int CollideEdge(phSegment& a1, lvlIntersection* a2, int a3, int* a4, int a5)
        {
            return hook::Thunk<0x45BBA0>::Call<int>(this, &a1, a2, a3, a4, a5);
        }

        AGE_API virtual bool CollideProbe(phSegment& a1, lvlIntersection* a2, float a3)
        {
            return hook::Thunk<0x45BBB0>::Call<bool>(this, &a1, a2, a3);
        }

        AGE_API virtual bool CollideAI(phSegment& a1, lvlIntersection* a2, int* a3, int a4)
        {
            return hook::Thunk<0x45BE40>::Call<bool>(this, &a1, a2, a3, a4);
        }
    };

    struct sdlPoly : phPolygon {};

    struct sdlPolyCached : public sdlPoly
    {
        Vector3 Verts[4];
    };
    ASSERT_SIZEOF(sdlPolyCached, 0x90);


    // Lua initialization

}