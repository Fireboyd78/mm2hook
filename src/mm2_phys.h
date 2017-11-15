#pragma once
#include "mm2_common.h"
#include "mm2_level.h"
#include "mm2_base.h"

namespace MM2
{
    // Forward declarations
    class phCollider;
    class phInterialCs;
    class dgPhysEntity;
    class dgBangerInstance;
    class phBound;

    // External declarations
    extern class lvlInstance;
    
    class phCollider {

    };

    class phInertialCs {

    };

    class dgPhysEntity : public Base {
    public:
        virtual AGE_API void FirstImpactCallback() { ageHook::Thunk<0x42CC20>::Call<void>(this); }
        virtual AGE_API bool RequiresTerrainCollision() { return ageHook::Thunk<0x42CC10>::Call<bool>(this); }
        virtual AGE_API void DetachMe() { ageHook::Thunk<0x42CC00>::Call<void>(this); }

        virtual AGE_API lvlInstance * GetInst() PURE;

        virtual AGE_API phCollider * GetCollider() { return ageHook::Thunk<0x42CBF0>::Call<phCollider *>(this); }

        virtual AGE_API phInertialCs * GetICS() PURE;
        virtual AGE_API void PostUpdate() PURE;

        virtual AGE_API void Update() { ageHook::Thunk<0x46A120>::Call<void>(this); }
        virtual AGE_API void PreUpdate() { ageHook::Thunk<0x42CBE0>::Call<void>(this); }
    };

    //TODO : find out inheritance / virtual stuff!!!
    class dgBangerInstance {
    public:
        void Draw(int a1) { ageHook::Thunk<0x4415E0>::Call<void>(this, a1); }
    };

    class phBound {
    public:
        void SetOffset(Vector3& offset) {
            ageHook::Thunk<0x4872C0>::Call<void>(this, offset);
        }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<phBound>("phBound")
                .addFunction("SetOffset", &SetOffset)
            .endClass();
        }
    };
}