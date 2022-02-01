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
    private:
        byte buffer[0xB0]; // there's a phColliderJointed here but phCollider and phColliderBase 
                           // aren't entirely known yet
    public:
        virtual AGE_API void PreUpdate()                    { hook::Thunk<0x42CBE0>::Call<void>(this); }
        virtual AGE_API void Update()                       { hook::Thunk<0x46A120>::Call<void>(this); }
        virtual AGE_API void PostUpdate()                   PURE;
        virtual AGE_API phInertialCS * GetICS()             PURE;
        virtual AGE_API phCollider * GetCollider()          { return hook::Thunk<0x42CBF0>::Call<phCollider *>(this); }
        virtual AGE_API lvlInstance * GetInst()             PURE;
        virtual AGE_API void DetachMe()                     { hook::Thunk<0x42CC00>::Call<void>(this); }
        virtual AGE_API bool RequiresTerrainCollision()     { return hook::Thunk<0x42CC10>::Call<bool>(this); }
        virtual AGE_API void FirstImpactCallback()          { hook::Thunk<0x42CC20>::Call<void>(this); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<dgPhysEntity, Base>("dgPhysEntity")
                .addFunction("DetachMe", &DetachMe)
                .addFunction("RequiresTerrainCollision", &RequiresTerrainCollision)
                .addFunction("GetICS", &GetICS)
                .addFunction("GetCollider", &GetCollider)
                .addFunction("GetInst", &GetInst)
            .endClass();
        }
    };
    ASSERT_SIZEOF(dgPhysEntity, 0xB4);

    // Lua initialization

}