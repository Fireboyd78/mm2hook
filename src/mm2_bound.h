#pragma once
#include "mm2_common.h"
#include "mm2_phys.h"

namespace MM2
{
    // Extern declarations
    extern class phMaterial;
    extern class phSegment;
    extern class phIntersection;
    extern class phIntersectionBase;
    extern class phIntersectionPoint;
    extern class phImpactBase;

    // Forward declarations
    class phBoundGeometry;
    class phBound;
    class phBoundBox;

    //TODO
    class phBoundGeometry {

    };

    class phBound {
    public:
        void SetOffset(Vector3& offset)                     { ageHook::Thunk<0x4872C0>::Call<void>(this, &offset); }

        /*
            phBound virtuals
        */
        AGE_API virtual void CenterBound()                  { ageHook::Thunk<0x4872A0>::Call<void>(this); }
        AGE_API virtual phMaterial* GetMaterial(int a1)     PURE;
        AGE_API virtual int GetNumMaterials()               { return ageHook::Thunk<0x45CF30>::Call<int>(this); }
        AGE_API virtual float SetFriction()                 { return ageHook::Thunk<0x45CF60>::Call<float>(this); }
        AGE_API virtual void SetFriction(float a1)          { ageHook::Thunk<0x45CF40>::Call<void>(this, a1); }
        AGE_API virtual float SetElasticity()               { return ageHook::Thunk<0x45CF70>::Call<float>(this); }
        AGE_API virtual void SetElasticity(float a1)        { ageHook::Thunk<0x45CF50>::Call<void>(this, a1); }
        AGE_API virtual bool TestProbePoint(phSegment & a1, phIntersectionPoint * a2, float a3)
                                                            PURE;
        AGE_API virtual bool TestAIPoint(phSegment  & a1, phIntersectionPoint * a2)
                                                            PURE;
        AGE_API virtual int  TestEdge(phSegment & a1, phIntersection * a2, int a3)
                                                            PURE;
        AGE_API virtual int  TestProbe(phSegment & a1, phIntersection * a2, float a3)
                                                            PURE;
        AGE_API virtual bool TestSphere(Vector3 const & a1, float a2, phImpactBase & a3)
                                                            { return ageHook::Thunk<0x4876C0>::Call<bool>(this, &a1, a2, &a3); }
        AGE_API virtual bool TestSphere(Vector3 const & a1, float a2, Vector3 & a3, Vector3 & a4, float & a5)
                                                            { return ageHook::Thunk<0x4876B0>::Call<bool>(this, &a1, a2, &a3, &a4, &a5); }
        AGE_API virtual void ModifyInvMassMatrix(Matrix34 * a1, int a2, Vector3 const & a3, Vector3 const & a4)
                                                            { ageHook::Thunk<0x45CF90>::Call<void>(this, a1, a2, &a3, &a4); }
        AGE_API virtual void ModifyInvMassMatrix(Matrix34 * a1, int a2, Vector3 const & a3)
                                                            { ageHook::Thunk<0x45CF80>::Call<void>(this, a1, a2, &a3); }
        AGE_API virtual float EffectiveMass(Vector3 const & a1, Vector3 const & a2, int a3, float a4)
                                                            { return ageHook::Thunk<0x45CFA0>::Call<float>(this, &a1, &a2, a3, a4); }
        AGE_API virtual Vector3 const & GetVertex(int a1)   { return ageHook::Thunk<0x487730>::Call<Vector3 const &>(this, a1); }
                                                            

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<phBound>("phBound")
                .addFunction("SetOffset", &SetOffset)
                .endClass();
        }
    };

    class phBoundBox : public phBound {

    };

    template<>
    void luaAddModule<module_bound>(LuaState L) {
        luaBind<phBound>(L);
    }
}