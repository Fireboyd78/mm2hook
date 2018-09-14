#pragma once
#include <modules\phys.h>

namespace MM2
{
    // Forward declarations
    class dgPhysEntity;

    // External declarations
    extern class lvlInstance;

    // Class definitions

    class dgPhysEntity : public Base {
    public:
        virtual AGE_API void PreUpdate()                    { ageHook::Thunk<0x42CBE0>::Call<void>(this); }
        virtual AGE_API void Update()                       { ageHook::Thunk<0x46A120>::Call<void>(this); }
        virtual AGE_API void PostUpdate()                   PURE;
        virtual AGE_API phInertialCS * GetICS()             PURE;
        virtual AGE_API phCollider * GetCollider()          { return ageHook::Thunk<0x42CBF0>::Call<phCollider *>(this); }
        virtual AGE_API lvlInstance * GetInst()             PURE;
        virtual AGE_API void DetachMe()                     { ageHook::Thunk<0x42CC00>::Call<void>(this); }
        virtual AGE_API bool RequiresTerrainCollision()     { return ageHook::Thunk<0x42CC10>::Call<bool>(this); }
        virtual AGE_API void FirstImpactCallback()          { ageHook::Thunk<0x42CC20>::Call<void>(this); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<dgPhysEntity>("dgPhysEntity")
                .addFunction("DetachMe", &DetachMe)
                .addFunction("RequiresTerrainCollision", &RequiresTerrainCollision)
                .addFunction("GetICS", &GetICS)
                .addFunction("GetCollider", &GetCollider)
                .addFunction("GetInst", &GetInst)
            .endClass();
        }
    };

    // Lua initialization

}