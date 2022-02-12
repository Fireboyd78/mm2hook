#pragma once
#include <modules\phys.h>

namespace MM2
{
    // Forward declarations
    class phBound;

    // External declarations
    extern class phMaterial;
    extern struct phSegment;
    extern struct phIntersectionPoint;
    extern struct phIntersection;
    extern class phImpactBase;

    // Class definitions
    class phBound {
    public:
        enum class BoundType
        {
            Sphere = 0,
            Geometry = 1,
            Box = 2,
            ForceSphere = 3,
            Terrain = 4,
            TerrainLocal = 5,
            Hotdog = 6,
            Level = 7
        };
    protected:
        int MaterialCount;
        BoundType Type;
        Vector3 Min;
        Vector3 Max;
        bool IsOffset;
        Vector3 Offset;
        float Radius;
        float Flexibility;
        bool IsFlexible;
        float o16;
        float Penetration;
        float PenetrationBarelyMoved;
    public:
        inline BoundType getType()
        {
            return this->Type;
        }

        inline int getMaterialCount()
        {
            return this->MaterialCount;
        }

        inline Vector3 getMin()
        {
            return this->Min;
        }

        inline Vector3 getMax()
        {
            return this->Max;
        }

        inline bool getIsOffset()
        {
            return this->IsOffset == TRUE;
        }

        inline Vector3 getOffset()
        {
            return this->Offset;
        }

        inline float getRadius()
        {
            return this->Radius;
        }
    public:
        phBound(BoundType type)
        {
            hook::Thunk<0x4871B0>::Call<void>(this, static_cast<int>(type));
        }

        void SetOffset(Vector3& offset) { hook::Thunk<0x4872C0>::Call<void>(this, &offset); }

        /*
            phBound virtuals
        */
        AGE_API virtual void CenterBound() { hook::Thunk<0x4872A0>::Call<void>(this); }
        AGE_API virtual phMaterial const* GetMaterial(int a1)     PURE;
        AGE_API virtual int GetNumMaterials() { return hook::Thunk<0x45CF30>::Call<int>(this); }
        AGE_API virtual float SetFriction() { return hook::Thunk<0x45CF60>::Call<float>(this); }
        AGE_API virtual void SetFriction(float a1) { hook::Thunk<0x45CF40>::Call<void>(this, a1); }
        AGE_API virtual float SetElasticity() { return hook::Thunk<0x45CF70>::Call<float>(this); }
        AGE_API virtual void SetElasticity(float a1) { hook::Thunk<0x45CF50>::Call<void>(this, a1); }
        AGE_API virtual bool TestProbePoint(phSegment& a1, phIntersectionPoint* a2, float a3)
            PURE;
        AGE_API virtual bool TestAIPoint(phSegment& a1, phIntersectionPoint* a2)
            PURE;
        AGE_API virtual int  TestEdge(phSegment& a1, phIntersection* a2, int a3)
            PURE;
        AGE_API virtual bool  TestProbe(phSegment& a1, phIntersection* a2, float a3)
            PURE;
        AGE_API virtual bool TestSphere(Vector3 const& a1, float a2, phImpactBase& a3)
        {
            return hook::Thunk<0x4876C0>::Call<bool>(this, &a1, a2, &a3);
        }
        AGE_API virtual bool TestSphere(Vector3 const& a1, float a2, Vector3& a3, Vector3& a4, float& a5)
        {
            return hook::Thunk<0x4876B0>::Call<bool>(this, &a1, a2, &a3, &a4, &a5);
        }
        AGE_API virtual void ModifyInvMassMatrix(Matrix34* a1, int a2, Vector3 const& a3, Vector3 const& a4)
        {
            hook::Thunk<0x45CF90>::Call<void>(this, a1, a2, &a3, &a4);
        }
        AGE_API virtual void ModifyInvMassMatrix(Matrix34* a1, int a2, Vector3 const& a3)
        {
            hook::Thunk<0x45CF80>::Call<void>(this, a1, a2, &a3);
        }
        AGE_API virtual float EffectiveMass(Vector3 const& a1, Vector3 const& a2, int a3, float a4)
        {
            return hook::Thunk<0x45CFA0>::Call<float>(this, &a1, &a2, a3, a4);
        }
        AGE_API virtual Vector3 const& GetVertex(int a1) { return hook::Thunk<0x487730>::Call<Vector3 const&>(this, a1); }


        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<phBound>("phBound")
                .addFunction("GetVertex", &GetVertex)
                .addFunction("SetOffset", &SetOffset)
                .addFunction("GetMaterial", &GetMaterial)
                .addPropertyReadOnly("Radius", &getRadius)
                .addPropertyReadOnly("Type", &getType)
                .addPropertyReadOnly("NumMaterials", &getMaterialCount)
                .addPropertyReadOnly("IsOffset", &getIsOffset)
                .addPropertyReadOnly("Offset", &getOffset)
                .addPropertyReadOnly("Min", &getMin)
                .addPropertyReadOnly("Max", &getMax)
                .endClass();
        }
    };
    ASSERT_SIZEOF(phBound, 0x4C);

    // Lua initialization

}