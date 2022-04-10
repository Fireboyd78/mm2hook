#pragma once
#include <modules\phys.h>
#include <modules\phys\phbound.h>

namespace MM2
{
    // Forward declarations
    class lvlLevelBound;

    // External declarations


    // Class definitions
    class lvlLevelBound : public phBound {
    public:
        /*
            phBound virtuals
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
        AGE_API virtual bool TestAI(phSegment& a1, phIntersectionPoint* a2) const override
                                                                            { return hook::Thunk<0x46A6B0>::Call<bool>(this, &a1, a2); }
        AGE_API virtual int TestEdgePoint(phSegment& a1, phIntersectionPoint* a2, int a3) const override
                                                                            { return hook::Thunk<0x46A6C0>::Call<int>(this, &a1, a2, a3); }
        AGE_API virtual float GetEdgeCosine(int a1) const override          { return hook::Thunk<0x46A700>::Call<float>(this, a1); }
        AGE_API virtual Vector3 const & GetEdgeNormal(int a1, Vector3& a2) const override
                                                                            { return hook::Thunk<0x46A710>::Call<Vector3 const&>(this, a1, &a2); }
    };

    // Lua initialization

}