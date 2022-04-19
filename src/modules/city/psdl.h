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

        AGE_API static void UpdateLighting()                   { hook::StaticThunk<0x448090>::Call<void>(); }
        AGE_API static bool BACKFACE(const Vector3& vec1, const Vector3& vec2)
                                                               { return hook::StaticThunk<0x45CBC0>::Call<bool>(&vec1, &vec2); }

        /*
            sdlCommon virtuals
        */

        AGE_API virtual int CollideEdgePoint(lvlSegment& a1, lvlIntersectionPoint* a2, int a3) const override
                                                               { return hook::Thunk<0x4482E0>::Call<int>(this, &a1, a2, a3); }
        AGE_API virtual bool CollideProbePoint(lvlSegment& a1, lvlIntersectionPoint* a2, float a3) const override
                                                               { return hook::Thunk<0x4482F0>::Call<bool>(this, &a1, a2, a3); }
        AGE_API virtual bool CollideAIPoint(lvlSegment& a1, lvlIntersectionPoint* a2) const override
                                                               { return hook::Thunk<0x448300>::Call<bool>(this, &a1, a2); }
        AGE_API virtual char* AllocateState() const override   { return hook::Thunk<0x448310>::Call<char*>(this); }
    };

    class lvlSDL : public sdlCommon {
    public:

        /*
            lvlSDL virtuals
        */

        AGE_API virtual Vector3 const& GetVertex(int a1) const override
                                                               { return hook::Thunk<0x45CB90>::Call<Vector3 const&>(this, a1); }
        AGE_API virtual int CollideEdge(lvlSegment& a1, lvlIntersection* a2, int a3, int* a4, int a5) const override
                                                               { return hook::Thunk<0x45BBA0>::Call<int>(this, &a1, a2, a3, a4, a5); }
        AGE_API virtual bool CollideProbe(lvlSegment& a1, lvlIntersection* a2, float a3) const override
                                                               { return hook::Thunk<0x45BBB0>::Call<bool>(this, &a1, a2, a3); }
        AGE_API virtual bool CollideAI(lvlSegment& a1, lvlIntersection* a2, int* a3, int a4) const override
                                                               { return hook::Thunk<0x45BE40>::Call<bool>(this, &a1, a2, a3, a4); }
        AGE_API virtual int CollidePolyToLevel(phBoundPolygonal const* a1, int* a2, int a3, phColliderBase* a4, Matrix34 const* a5, Matrix34 const* a6, lvlIntersection* a7, int a8, int* a9, bool a10) const override
                                                               { return hook::Thunk<0x45B060>::Call<int>(this, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10); }
    };

    struct sdlPoly : phPolygon {};

    struct sdlPolyCached : public sdlPoly
    {
        Vector3 Verts[4];
    };
    ASSERT_SIZEOF(sdlPolyCached, 0x90);


    // Lua initialization

}