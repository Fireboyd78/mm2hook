#pragma once
#include <modules\vehicle.h>

namespace MM2
{
    // Forward declarations
    class vehDrivetrain;

    // External declarations
    extern class vehCarSim;
    extern class vehEngine;
    extern class vehWheel;
    extern class vehTransmission;

    // Class definitions
    class vehDrivetrain : public asNode {
    private:
        vehCarSim *m_CarSimPtr;
        vehEngine *m_AttachedEngine;
        vehTransmission *m_AttachedTransmission;
        int WheelCount;
        vehWheel *Wheels[4];
        float DynamicAmount;
        float unknown60;
        float AngInertia;
        float BrakeDynamicCoef;
        float BrakeStaticCoef;
    private:
        //lua helpers
        inline bool addWheelLua(vehWheel* wheel) {
            return this->AddWheel(wheel) == TRUE;
        }
    public:
        inline vehEngine * getAttachedEngine(void) {
            return this->m_AttachedEngine;
        }

        inline vehTransmission * getAttachedTransmission(void) {
            return this->m_AttachedTransmission;
        }

        inline vehWheel * getWheel(int num) {
            if (num > 3)
                return nullptr;
            if (num >= this->WheelCount)
                return nullptr;
            return this->Wheels[num];
        }

        inline int getWheelCount(void) {
            return this->WheelCount;
        }
    public:
        AGE_API BOOL AddWheel(vehWheel *wheel)             { return ageHook::Thunk<0x4D9E50>::Call<BOOL>(this, wheel); }
        
        AGE_API void CopyVars(vehDrivetrain *copyFrom)     { ageHook::Thunk<0x4D9DE0>::Call<void>(this, copyFrom); }

        AGE_API void Attach()                              { ageHook::Thunk<0x4D9E20>::Call<void>(this); }
        AGE_API void Detach()                              { ageHook::Thunk<0x4D9E40>::Call<void>(this); }

        AGE_API void Init(vehCarSim *carSim)               { ageHook::Thunk<0x4D9DD0>::Call<void>(this, carSim); }
        
        
        
        /*
            asNode virtuals
        */

        AGE_API void Reset() override                      { ageHook::Thunk<0x4D9E00>::Call<void>(this); }
        AGE_API void Update() override                     { ageHook::Thunk<0x4D9E90>::Call<void>(this); }
        AGE_API void FileIO(datParser &parser) override
                                                           { ageHook::Thunk<0x4DA570>::Call<void>(this); }
        AGE_API char * GetClassName() override             { return ageHook::Thunk<0x4DA600>::Call<char *>(this); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<vehDrivetrain, asNode>("vehDrivetrain")
                //properties
                .addPropertyReadOnly("AttachedEngine", &getAttachedEngine)
                .addPropertyReadOnly("AttachedTransmission", &getAttachedTransmission)
                .addPropertyReadOnly("WheelCount", &getWheelCount)

                .addVariableRef("AngInertia", &vehDrivetrain::AngInertia)
                .addVariableRef("BrakeDynamicCoef", &vehDrivetrain::BrakeDynamicCoef)
                .addVariableRef("BrakeStaticCoef", &vehDrivetrain::BrakeStaticCoef)

                //functions
                .addFunction("GetWheel", &getWheel)
                .addFunction("AddWheel", &addWheelLua)
                .addFunction("Attach", &Attach)
                .addFunction("Detach", &Detach)
            .endClass();
        }
    };

    ASSERT_SIZEOF(vehDrivetrain, 0x4C);

    // Lua initialization

}