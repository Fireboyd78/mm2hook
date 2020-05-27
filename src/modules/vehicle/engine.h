#pragma once
#include <modules\vehicle.h>

namespace MM2
{
    // Forward declarations
    class vehEngine;

    // External declarations
    extern class vehCarSim;
    extern class vehTransmission;
    extern class vehDriveTrain;

    // Class definitions
    class vehEngine : public asNode {
    private:
        float MaxHorsePower;
        float IdleRPM;
        float OptRPM;
        float MaxRPM;
        float GCL;
        float unknown648;
        int unknown652;
        float AngInertia;
        float MaxTorque;
        float OptTorque;
        float IdleTorque;
        float TorqueGap;
        float unknown676;
        float sq5add1;
        float sq5min1;
        float GearChangedAtRPM;
        float GCLTimer;
        BOOL WaitingOnGCL;
        float ThrottleInput;
        float CurrentTorque;
        float CurrentRPM;
        float unknown712;
        float ThrottleTorque;
        Matrix34 *EngineVisualMatrixRef;
        Matrix34 *EngineVisualMatrixPtr;
        vehCarSim *VehCarSimPtr;
        vehTransmission *VehTransmissionPtr;
        vehDriveTrain *VehDriveTrainPtr;
    public:

        AGE_API void ComputeConstants()                     { ageHook::Thunk<0x4D8DC0>::Call<void>(this); }
        AGE_API float CalcTorqueAtFullThrottle(float a1)    { return ageHook::Thunk<0x4D8E20>::Call<float>(this, a1); }
        AGE_API float CalcTorqueAtZeroThrottle(float a1)    { return ageHook::Thunk<0x4D8EA3>::Call<float>(this, a1); }
        AGE_API float CalcTorque(float a1)                  { return ageHook::Thunk<0x4D8ED0>::Call<float>(this, a1); }
        AGE_API float CalcHPAtFullThrottle(float a1)        { return ageHook::Thunk<0x4D8F10>::Call<float>(this, a1); }

        /*
            asNode virtuals
        */

        AGE_API void Update() override                      { ageHook::Thunk<0x4D8F30>::Call<void>(this); }
        AGE_API void Reset() override                       { ageHook::Thunk<0x4D8CE0>::Call<void>(this); }
        AGE_API void FileIO(datParser &parser)  override
                                                            { ageHook::Thunk<0x4D9240>::Call<void>(this); }
        AGE_API char* GetClassName() override               { return ageHook::Thunk<0x4D9310>::Call<char*>(this); }

        //lua ahelpers
        inline bool getWaitingOnGCL() {
            return WaitingOnGCL == TRUE;
        }

        inline float getThrottleInput() {
            return ThrottleInput;
        }

        inline void setThrottleInput(float input) {
            ThrottleInput = input;
        }

        inline float getCurrentRPM() {
            return CurrentRPM;
        }

        inline void setCurrentRPM(float rpm) {
            CurrentRPM = rpm;
        }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<vehEngine, asNode>("vehEngine")
                //properties
                .addVariableRef("MaxHorsePower", &vehEngine::MaxHorsePower)
                .addVariableRef("IdleRPM", &vehEngine::IdleRPM)
                .addVariableRef("OptRPM", &vehEngine::OptRPM)
                .addVariableRef("MaxRPM", &vehEngine::MaxRPM)
                .addVariableRef("GCL", &vehEngine::GCL)
                .addVariableRef("AngInertia", &vehEngine::AngInertia)
                .addProperty("RPM", &getCurrentRPM, &setCurrentRPM)
                .addPropertyReadOnly("WaitingOnGCL", &getWaitingOnGCL)

                //functions
                .addFunction("ComputeConstants", &ComputeConstants)
                .addFunction("CalcTorqueAtFullThrottle", &CalcTorqueAtFullThrottle)
                .addFunction("CalcTorqueAtZeroThrottle", &CalcTorqueAtZeroThrottle)
                .addFunction("CalcTorque", &CalcTorque)
                .addFunction("CalcHPAtFullThrottle", &CalcHPAtFullThrottle)

                .endClass();
        }
    };
    ASSERT_SIZEOF(vehEngine, 0x88);

    // Lua initialization

}