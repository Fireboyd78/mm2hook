#pragma once
#include <modules\banger.h>

namespace MM2
{
    // Forward declarations
    class dgBangerInstance;
    class dgUnhitBangerInstance;
    class dgUnhitYBangerInstance;

    // External declarations
    extern class lvlInstance;

    // Class definitions

    class dgBangerInstance : public lvlInstance {
    public:
        static hook::Type<int> sm_RefAlpha;

        /*
            lvlInstance virtuals
        */

        virtual AGE_API void Draw(int lod)                  { hook::Thunk<0x4415E0>::Call<void>(this, lod); }
        virtual AGE_API void DrawReflected(float a1)        { hook::Thunk<0x4417B0>::Call<void>(this, a1); }
        virtual AGE_API void DrawTree(int lod)              { hook::Thunk<0x4417C0>::Call<void>(this, lod); }
        virtual AGE_API void DrawGlow()                     { hook::Thunk<0x441840>::Call<void>(this); }
        virtual AGE_API void DrawShadow()                   { hook::Thunk<0x441990>::Call<void>(this); }
        virtual AGE_API void DrawShadowMap()                { hook::Thunk<0x4419A0>::Call<void>(this); }
        virtual AGE_API dgPhysEntity* GetEntity()           { return hook::Thunk<0x441AD0>::Call<dgPhysEntity*>(this); }
        virtual AGE_API dgPhysEntity* AttachEntity()        { return hook::Thunk<0x441AE0>::Call<dgPhysEntity*>(this); }
        virtual AGE_API Vector3 const * GetVelocity()       { return hook::Thunk<0x441B20>::Call<Vector3 const *>(this); }

        AGE_API dgBangerData * GetData()                    { return hook::Thunk<0x441AB0>::Call<dgBangerData*>(this); }
    };

    class dgUnhitBangerInstance : public dgBangerInstance {
    public:
        AGE_API static dgUnhitBangerInstance* RequestBanger(char* a1, BOOL a2)
                                                            { return hook::StaticThunk<0x441D80>::Call<dgUnhitBangerInstance*>(a1, a2); }
    };

    class dgUnhitYBangerInstance : public dgBangerInstance {
    public:

        /*
            lvlInstance virtuals
        */

        virtual AGE_API const Matrix34 & GetMatrix(Matrix34* a1)
                                                            { return hook::Thunk<0x441F70>::Call<Matrix34 const&>(this, a1); }
        virtual AGE_API void SetMatrix(const Matrix34* a1)  { hook::Thunk<0x441F40>::Call<void>(this, a1); }
        virtual AGE_API const Vector3 & GetPosition()       { return hook::Thunk<0x441FC0>::Call<Vector3 const&>(this); }
    };

    // Lua initialization

}