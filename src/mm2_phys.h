#pragma once
#include "mm2_common.h"
#include "mm2_level.h"
#include "mm2_base.h"

namespace MM2
{
    // Forward declarations
    class dgPhysManager;
    class dgPhysEntity;
    class dgBangerInstance;
    struct phCollider;
    class phInterialCs;
    class phBound;

    // External declarations
    extern class lvlInstance;
    
    class dgPhysManager {
    public:
        static ageHook::Type<dgPhysManager *> Instance;

        void DeclareMover(lvlInstance* instance, int a2, int a3) { ageHook::Thunk<0x468370>::Call<void>(this, instance, a2, a3); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<dgPhysManager>("dgPhysManager")
                //statics
                .addStaticFunction("Instance", [] {return (dgPhysManager *)Instance; })

                //functions
                .addFunction("DeclareMover", &DeclareMover)

                .endClass();
        }
    };

    struct phCollider {
        //todo: figure the rest of this out!
    private:
        byte unknown[0x30];
    public:
        int colliderId; //?
    };

    class phInertialCs {

    };

    class phMaterial {

    };

    class phIntersectionPoint {

    };

    class phIntersection {

    };

    class phSegment {

    };

    class phImpactBase {

    };

    class dgPhysEntity : public Base {
    public:
        virtual AGE_API void PreUpdate()                    { ageHook::Thunk<0x42CBE0>::Call<void>(this); }
        virtual AGE_API void Update()                       { ageHook::Thunk<0x46A120>::Call<void>(this); }
        virtual AGE_API void PostUpdate() PURE;
        virtual AGE_API phInertialCs * GetICS() PURE;
        virtual AGE_API phCollider * GetCollider()          { return ageHook::Thunk<0x42CBF0>::Call<phCollider *>(this); }
        virtual AGE_API lvlInstance * GetInst() PURE;
        virtual AGE_API void DetachMe()                     { ageHook::Thunk<0x42CC00>::Call<void>(this); }
        virtual AGE_API bool RequiresTerrainCollision()     { return ageHook::Thunk<0x42CC10>::Call<bool>(this); }
        virtual AGE_API void FirstImpactCallback()          { ageHook::Thunk<0x42CC20>::Call<void>(this); }
 
    };

    //TODO : find out inheritance / virtual stuff!!!
    class dgBangerInstance {
    public:
        void Draw(int a1) { ageHook::Thunk<0x4415E0>::Call<void>(this, a1); }
    };

    template<>
    void luaAddModule<module_phys>(LuaState L) {
        luaBind<dgPhysManager>(L);
    }
}