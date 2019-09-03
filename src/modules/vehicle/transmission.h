#pragma once
#include <modules\vehicle.h>

namespace MM2
{
    // Forward declarations
    class vehTransmission;

    // External declarations
    extern class vehCarSim;

    // Class definitions

    class vehTransmission : public asNode {
    private:
        vehCarSim *VehCarSimPtr;
        int GearChanged;
        int IsAutomatic;
        int CurrentGear;
        float GearChangeTimer;
        float GearChangeTime;
        float ManualRatios[8];
        int ManualNumGears;
        int AutoNumGears;
        float AutoRatios[8];
        float UpshiftBiases[8];
        float DownshiftMinBiases[8];
        float DownshiftMaxBiases[8];
        float ReverseSpeed;
        float FirstGearSpeed;
        float LastGearSpeed;
        float UpshiftBias;
        float DownshiftBiasMin;
        float DownshiftBiasMax;
        float GearBias;
    public:
        AGE_API void Upshift()                              { ageHook::Thunk<0x4CF570>::Call<void>(this); }
        AGE_API void Downshift()                            { ageHook::Thunk<0x4CF5B0>::Call<void>(this); }
        AGE_API void SetReverse()                           { ageHook::Thunk<0x4CF6C0>::Call<void>(this); }
        AGE_API void SetNeutral()                           { ageHook::Thunk<0x4CF6D0>::Call<void>(this); }
        AGE_API void SetForward()                           { ageHook::Thunk<0x4CF6E0>::Call<void>(this); }
        AGE_API void SetCurrentGear(int a1)                 { ageHook::Thunk<0x4CF700>::Call<void>(this, a1); }
        AGE_API void Automatic(BOOL a1)                     { ageHook::Thunk<0x4CF6B0>::Call<void>(this, a1); }
        AGE_API float GearRatioFromMPH(float a1)            { return ageHook::Thunk<0x4CF530>::Call<float>(this, a1); }
        AGE_API void ComputeConstants()                     { ageHook::Thunk<0x4CF220>::Call<void>(this); }

        /*
            asNode virtuals
        */

        AGE_API void Update() override                      { ageHook::Thunk<0x4CF600>::Call<void>(this); }
        AGE_API void Reset() override                       { ageHook::Thunk<0x4CF200>::Call<void>(this); }
        AGE_API void FileIO(datParser &parser)  override
                                                            { ageHook::Thunk<0x4CF740>::Call<void>(this); }
        AGE_API char* GetClassName() override               { return ageHook::Thunk<0x4CF880>::Call<char*>(this); }

        //lua helpres
        inline int getGear(void) {
            return this->CurrentGear;
        };

        inline float getAutoGearRatio(int gear) {
            return AutoRatios[CLAMPINT(gear, 0, 7)];
        }

        inline float getManualGearRatio(int gear) {
            return ManualRatios[CLAMPINT(gear, 0, 7)];
        }

        inline float getUpshiftBias(int gear) {
            return UpshiftBiases[CLAMPINT(gear, 0, 7)];
        }

        inline float getDownshiftMinBias(int gear) {
            return DownshiftMinBiases[CLAMPINT(gear, 0, 7)];
        }

        inline float getDownshiftMaxBias(int gear) {
            return DownshiftMaxBiases[CLAMPINT(gear, 0, 7)];
        }

        inline void setDownshiftMinBias(int gear, float bias)
        {
            if (gear < 0 || gear > 7)
                return;
            DownshiftMinBiases[gear] = bias;
        }

        inline void setDownshiftMaxBias(int gear, float bias)
        {
            if (gear < 0 || gear > 7)
                return;
            DownshiftMaxBiases[gear] = bias;
        }

        inline void setUpshiftBias(int gear, float bias)
        {
            if (gear < 0 || gear > 7)
                return;
            UpshiftBiases[gear] = bias;
        }

        inline void setAutoGearRatio(int gear, float ratio)
        {
            if (gear < 0 || gear > 7)
                return;
            AutoRatios[gear] = ratio;
        }

        inline void setManualGearRatio(int gear, float ratio)
        {
            if (gear < 0 || gear > 7)
                return;
            ManualRatios[gear] = ratio;
        }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<vehTransmission, asNode>("vehTransmission")
                //properties / lua functions
                .addFunction("SetAutoGearRatio", &setAutoGearRatio)
                .addFunction("GetAutoGearRatio", &getAutoGearRatio)
                .addFunction("SetManualGearRatio", &setManualGearRatio)
                .addFunction("GetManualGearRatio", &getManualGearRatio)
                .addFunction("GetUpshiftBias", &getUpshiftBias)
                .addFunction("SetUpshiftBias", &setUpshiftBias)
                .addFunction("GetDownshiftMinBias", &getDownshiftMinBias)
                .addFunction("GetDownshiftMaxBias", &getDownshiftMaxBias)
                .addFunction("SetDownshiftMinBias", &setDownshiftMinBias)
                .addFunction("SetDownshiftMaxBias", &setDownshiftMaxBias)
                .addVariableRef("GearChangeTime", &vehTransmission::GearChangeTime)
                .addVariableRef("AutoNumGears", &vehTransmission::AutoNumGears)
                .addVariableRef("ManualNumGears", &vehTransmission::ManualNumGears)
                .addVariableRef("Low", &vehTransmission::FirstGearSpeed)
                .addVariableRef("High", &vehTransmission::LastGearSpeed)
                .addVariableRef("Reverse", &vehTransmission::ReverseSpeed)
                .addVariableRef("GearBias", &vehTransmission::GearBias)
                .addVariableRef("DownshiftBiasMax", &vehTransmission::DownshiftBiasMax)
                .addVariableRef("DownshiftBiasMin", &vehTransmission::DownshiftBiasMin)
                .addVariableRef("UpshiftBias", &vehTransmission::UpshiftBias)
                .addProperty("Gear", &getGear, &SetCurrentGear)

                //functions
                .addFunction("Automatic", &Automatic, LUA_ARGS(bool))
                .addFunction("Downshift", &Downshift)
                .addFunction("Upshift", &Upshift)
                .addFunction("SetReverse", &SetReverse)
                .addFunction("SetNeutral", &SetNeutral)
                .addFunction("SetForward", &SetForward)
                .addFunction("SetCurrentGear", &SetCurrentGear)
                .addFunction("GearRatioFromMPH", &GearRatioFromMPH)
                .addFunction("ComputeConstants", &ComputeConstants)
                .endClass();
        }
    };

    ASSERT_SIZEOF(vehTransmission, 0xF4);

    // Lua initialization

}