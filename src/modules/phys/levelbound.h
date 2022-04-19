#pragma once
#include <modules\phys.h>
#include <modules\phys\phbound.h>

namespace MM2
{
    // Forward declarations
    class lvlLevelBound;

    // External declarations
    extern class phBoundBox;
    extern class phBoundPolygonal;
    extern class phBoundSphere;
    extern class phBoundHotdog;

    // Class definitions
    class lvlLevelBound : public phBound {
    public:

        /*
            lvlLevelBound virtuals
        */

        AGE_API virtual phMaterial const * GetMaterial(int a1) override     { return hook::Thunk<0x46A720>::Call<phMaterial*>(this, a1); }
        AGE_API virtual bool TestProbePoint(phSegment& a1, phIntersectionPoint* a2, float a3) const override
                                                                            { return hook::Thunk<0x46A6D0>::Call<bool>(this, &a1, a2, a3); }
        AGE_API virtual bool TestAIPoint(phSegment& a1, phIntersectionPoint* a2) const override
                                                                            { return hook::Thunk<0x46A6E0>::Call<bool>(this, &a1, a2); }
        AGE_API virtual int TestEdge(phSegment& a1, phIntersection* a2, int a3) const override
                                                                            { return hook::Thunk<0x46A690>::Call<bool>(this, &a1, a2, a3); }
        AGE_API virtual bool TestProbe(phSegment& a1, phIntersection* a2, float a3) const override
                                                                            { return hook::Thunk<0x46A6A0>::Call<bool>(this, &a1, a2, a3); }
        AGE_API virtual bool TestSphere(Vector3 const& a1, float a2, Vector3& a3, Vector3& a4, float& a5) const override
                                                                            { return hook::Thunk<0x46A6F0>::Call<bool>(this, &a1, a2, &a3, &a4, &a5); }
        AGE_API virtual bool TestAI(phSegment& a1, phIntersectionPoint* a2) const
                                                                            { return hook::Thunk<0x46A6B0>::Call<bool>(this, &a1, a2); }
        AGE_API virtual int TestEdgePoint(phSegment& a1, phIntersectionPoint* a2, int a3) const
                                                                            { return hook::Thunk<0x46A6C0>::Call<int>(this, &a1, a2, a3); }

        /*
            lvlSDL & sdlCommon virtuals
        */

        AGE_API virtual int CollideEdge(lvlSegment& a1, lvlIntersection* a2, int a3, int* a4, int a5) const
                                                                            PURE;
        AGE_API virtual bool CollideProbe(lvlSegment& a1, lvlIntersection* a2, float a3) const
                                                                            PURE;
        AGE_API virtual bool CollideAI(lvlSegment& a1, lvlIntersection* a2, int* a3, int a4) const
                                                                            PURE;
        AGE_API virtual int CollideEdgePoint(lvlSegment& a1, lvlIntersectionPoint* a2, int a3) const
                                                                            PURE;
        AGE_API virtual bool CollideProbePoint(lvlSegment& a1, lvlIntersectionPoint* a2, float a3) const
                                                                            PURE;
        AGE_API virtual bool CollideAIPoint(lvlSegment& a1, lvlIntersectionPoint* a2) const
                                                                            PURE;

        /*
            lvlLevelBound virtuals
        */

        AGE_API virtual char* AllocateState() const                         { return hook::Thunk<0x46A680>::Call<char*>(this); }
        AGE_API virtual float GetEdgeCosine(int a1) const                   { return hook::Thunk<0x46A700>::Call<float>(this, a1); }
        AGE_API virtual Vector3 const & GetEdgeNormal(int a1, Vector3& a2) const
                                                                            { return hook::Thunk<0x46A710>::Call<Vector3 const&>(this, a1, &a2); }
        AGE_API virtual bool TrivialCollideBoxToLevel(phBoundBox const* a1, int a2, phColliderBase* a3, Matrix34 const* a4, Matrix34 const* a5) const
                                                                            { return hook::Thunk<0x46B580>::Call<bool>(this, a1, a2, a3, a4, a5); }
        AGE_API virtual int CollidePolyToLevel(phBoundPolygonal const* a1, int* a2, int a3, phColliderBase* a4, Matrix34 const* a5, Matrix34 const* a6, lvlIntersection* a7, int a8, int* a9, bool a10) const
                                                                            { return hook::Thunk<0x46A750>::Call<int>(this, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10); }
        AGE_API virtual int FindImpactsSphereOneRoom(int a1, phBoundSphere const* a2, Vector3 const& a3, phColliderBase* a4, phColliderBase* a5, phImpactBase* a6, int a7) const
                                                                            { return hook::Thunk<0x45CFB0>::Call<int>(this, a1, a2, &a3, a4, a5, a6, a7); }
        AGE_API virtual int FindImpactsHotdogOneRoom(int a1, Vector3 const * const a2, phBoundHotdog const* a3, Matrix34 const* a4, Matrix34 const* a5, phColliderBase* a6, phColliderBase* a7, phImpactBase* a8, int a9, Vector3 const& a10) const
                                                                            { return hook::Thunk<0x45CFC0>::Call<int>(this, a1, a2, a3, a4, a5, a6, a7, a8, a9, &a10); }
    };

    // Lua initialization

}