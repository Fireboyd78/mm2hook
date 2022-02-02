#pragma once
#include <modules\banger.h>
#include <modules\phys.h>
#include <mm2_particle.h>

namespace MM2
{
    // Forward declarations
    class dgBangerActive;
    class dgBangerActiveManager;

    // External declarations


    // Class definitions
    class dgBangerActive : public dgPhysEntity {
    private:
        int ID;
        phInertialCS ICS;
        phSleep Sleep;
        lvlInstance* Instance;
        asParticles Particles;
        float Timer;
    protected:
    public:
        /*
            dgPhysEntity virtuals
        */
        virtual AGE_API void Update() override              { hook::Thunk<0x440050>::Call<void>(this); }
        virtual AGE_API void PostUpdate() override          { hook::Thunk<0x4400E0>::Call<void>(this); }
        virtual AGE_API phInertialCS * GetICS() override    { return hook::Thunk<0x4405C0>::Call<phInertialCS*>(this); }
        virtual AGE_API lvlInstance * GetInst() override    { return hook::Thunk<0x43FDB0>::Call<lvlInstance*>(this); }
        virtual AGE_API void DetachMe() override            { hook::Thunk<0x440030>::Call<void>(this); }

        /*
            dgBangerActive
        */
        AGE_API void Detach() { hook::Thunk<0x43FFF0>::Call<void>(this); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<dgBangerActive, dgPhysEntity>("dgBangerActive")
                .endClass();
        }
    };
    ASSERT_SIZEOF(dgBangerActive, 0x318);

    class dgBangerActiveManager : public asNode {
    private:
        dgBangerActive** ActiveInstances[32];
        dgBangerActive BangerPool[32];
        int NumActive;
    protected:
        static hook::Type<dgBangerActiveManager*> Instance;
    public:
        inline static dgBangerActiveManager* GetInstance()
        {
            return Instance.get();
        }

        inline int GetNumActive() const {
            return this->NumActive;
        }

    };
    ASSERT_SIZEOF(dgBangerActiveManager, 0x639C);

    // Lua initialization

}