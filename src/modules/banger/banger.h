#pragma once
#include <modules\banger.h>

namespace MM2
{
    // Forward declarations
    class dgBangerInstance;
    class dgUnhitBangerInstance;

    // External declarations
    extern class lvlInstance;

    // Class definitions

    //TODO : find out inheritance / virtual stuff!!!
    class dgBangerInstance {
    public:
        void Draw(int a1) { ageHook::Thunk<0x4415E0>::Call<void>(this, a1); }
    };

    class dgUnhitBangerInstance : public lvlInstance {
    public:
        /*
            lvlInstance virtuals
        */

        virtual AGE_API void SetVariant(int a1)             { ageHook::Thunk<0x442A10>::Call<void>(this); }
        virtual AGE_API dgPhysEntity* GetEntity()           { return ageHook::Thunk<0x441AD0>::Call<dgPhysEntity*>(this); }
        virtual AGE_API dgPhysEntity* AttachEntity()        { return ageHook::Thunk<0x441AE0>::Call<dgPhysEntity*>(this); }
        virtual AGE_API Vector3 const * GetVelocity()       { return ageHook::Thunk<0x441B20>::Call<Vector3 const *>(this); }
        virtual AGE_API void Draw(int a1)                   { ageHook::Thunk<0x4415E0>::Call<void>(this, a1); }
        virtual AGE_API void DrawShadow()                   { ageHook::Thunk<0x441990>::Call<void>(this); }
        virtual AGE_API void DrawShadowMap()                { ageHook::Thunk<0x4419A0>::Call<void>(this); }
        virtual AGE_API void DrawGlow()                     { ageHook::Thunk<0x441840>::Call<void>(this); }
        virtual AGE_API void DrawReflected(float a1)        { ageHook::Thunk<0x4417B0>::Call<void>(this, a1); }

        AGE_API static dgUnhitBangerInstance* RequestBanger(char* a1, BOOL a2)
                                                            { return ageHook::StaticThunk<0x441D80>::Call<dgUnhitBangerInstance*>(a1, a2); }
    };

    // Lua initialization

}