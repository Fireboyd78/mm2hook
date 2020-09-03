#pragma once
#include <modules\vehicle.h>

namespace MM2
{
    // Forward declarations
    class vehWheel;

    // External declarations
    extern class vehCarSim;

    // Class definitions
    class vehWheel : public asNode {
    private:
        vehCarSim *VehCarSimPtr;
        byte _buffer[0x204];
        float SuspensionExtent;
        float SuspensionLimit;
        float SuspensionFactor;
        float SuspensionDampCoef;
        float SteeringLimit;
        float SteeringOffset;
        float BrakeCoef;
        float HandbrakeCoef;
        float CamberLimit;
        float WobbleLimit;
        float TireDispLimitLong;
        float TireDampCoefLong;
        float TireDragCoefLong;
        float TireDispLimitLat;
        float TireDampCoefLat;
        float TireDragCoefLat;
        float OptimumSlipPercent;
        float StaticFric;
        float SlidingFric;
    public:
        AGE_API vehWheel()                                     { ageHook::Thunk<0x4D2190>::Call<void>(this); }

        AGE_API void CopyVars(vehWheel *copyFrom)              { ageHook::Thunk<0x4D4110>::Call<void>(this, copyFrom); }

        AGE_API void ComputeConstants()                        { ageHook::Thunk<0x4D23F0>::Call<void>(this); }
        AGE_API void AddNormalLoad(float a1)                   { ageHook::Thunk<0x4D2490>::Call<void>(this, a1); }
        AGE_API void SetNormalLoad(float a1)                   { ageHook::Thunk<0x4D24C0>::Call<void>(this, a1); }
        AGE_API void SetInputs(float a1, float a2, float a3)
                                                               { ageHook::Thunk<0x4D3F80>::Call<void>(this, a1, a2, a3); }
        AGE_API int GetSurfaceSound()                          { return ageHook::Thunk<0x4D3F60>::Call<int>(this); }
        AGE_API float GetVisualDispVert()                      { return ageHook::Thunk<0x4D4030>::Call<float>(this); }
        AGE_API float GetVisualDispLat()                       { return ageHook::Thunk<0x4D4090>::Call<float>(this); }
        AGE_API float GetVisualDispLong()                      { return ageHook::Thunk<0x4D40D0>::Call<float>(this); }

        /*
            asNode virtuals
        */

        AGE_API void Reset() override                          { ageHook::Thunk<0x4D22E0>::Call<void>(this); }
        AGE_API void Update() override                         { ageHook::Thunk<0x4D34E0>::Call<void>(this); }
        AGE_API void FileIO(datParser &parser) override        { ageHook::Thunk<0x4D41C0>::Call<void>(this); }
        AGE_API char * GetClassName() override                 { return ageHook::Thunk<0x4D43C0>::Call<char *>(this); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<vehWheel, asNode>("vehWheel")
                //properties
                .addVariableRef("SuspensionExtent", &vehWheel::SuspensionExtent)
                .addVariableRef("SuspensionLimit", &vehWheel::SuspensionLimit)
                .addVariableRef("SuspensionFactor", &vehWheel::SuspensionFactor)
                .addVariableRef("SuspensionDampCoef", &vehWheel::SuspensionDampCoef)
                .addVariableRef("SteeringLimit", &vehWheel::SteeringLimit)
                .addVariableRef("SteeringOffset", &vehWheel::SteeringOffset)
                .addVariableRef("BrakeCoef", &vehWheel::BrakeCoef)
                .addVariableRef("HandbrakeCoef", &vehWheel::HandbrakeCoef)
                .addVariableRef("CamberLimit", &vehWheel::CamberLimit)
                .addVariableRef("WobbleLimit", &vehWheel::WobbleLimit)
                .addVariableRef("TireDispLimitLong", &vehWheel::TireDispLimitLong)
                .addVariableRef("TireDampCoefLong", &vehWheel::TireDampCoefLong)
                .addVariableRef("TireDragCoefLong", &vehWheel::TireDragCoefLong)
                .addVariableRef("TireDispLimitLat", &vehWheel::TireDispLimitLat)
                .addVariableRef("TireDampCoefLat", &vehWheel::TireDampCoefLat)
                .addVariableRef("TireDragCoefLat", &vehWheel::TireDragCoefLat)
                .addVariableRef("OptimumSlipPercent", &vehWheel::OptimumSlipPercent)
                .addVariableRef("StaticFric", &vehWheel::StaticFric)
                .addVariableRef("SlidingFric", &vehWheel::SlidingFric)

                //functions
                .addFunction("CopyVars", &CopyVars)
                .addFunction("ComputeConstants", &ComputeConstants)
                .addFunction("AddNormalLoad", &AddNormalLoad)
                .addFunction("SetNormalLoad", &SetNormalLoad)
                .addFunction("SetInputs", &SetInputs)
                .addFunction("GetSurfaceSound", &GetSurfaceSound)
                .addFunction("GetVisualDispVert", &GetVisualDispVert)
                .addFunction("GetVisualDispLat", &GetVisualDispLat)
                .addFunction("GetVisualDispLong", &GetVisualDispLong)
            .endClass();
        }
    };

    ASSERT_SIZEOF(vehWheel, 0x26C);

    // Lua initialization

}