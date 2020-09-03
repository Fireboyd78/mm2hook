#pragma once
#include <modules\vehicle.h>

namespace MM2
{
    // Forward declarations
    class vehGyro;

    // External declarations


    // Class definitions
    class vehGyro : public asNode
    {
    private:
        vehCarSim* m_CarSimPtr;
        float Drift;
        float Spin180;
        float Reverse180;
        float Pitch;
        float Roll;
    public:
        inline bool getDriftEnabled(void) {
            return (this->flags & 0x20000) != 0;
        }

        inline bool getSpinReverseEnabled(void) {
            return (this->flags & 0x10000) != 0;
        }

        inline bool getPitchRollEnabled(void) {
            return (this->flags & 0x40000) != 0;
        }

        inline void setDriftEnabled(bool enabled) {
            if (enabled)
                this->flags |= 0x20000;
            else
                this->flags &= ~(0x20000);
        }

        inline void setSpinReverseEnabled(bool enabled) {
            if (enabled)
                this->flags |= 0x10000;
            else
                this->flags &= ~(0x10000);
        }

        inline void setPitchRollEnabled(bool enabled) {
            if (enabled)
                this->flags |= 0x40000;
            else
                this->flags &= ~(0x40000);
        }
    public:
        AGE_API vehGyro() { ageHook::Thunk<0x4D5B80>::Call<void>(this); }

        AGE_API void Init(vehCarSim *carSim, const char *name) 
                                                        { ageHook::Thunk<0x4D5BD0>::Call<void>(this, carSim, name); }

        /*
            asNode virtuals
        */

        AGE_API void Update() override                  { ageHook::Thunk<0x4D5C00>::Call<void>(this); }
        AGE_API void FileIO(datParser& parser) override { ageHook::Thunk<0x4D5EE0>::Call<void>(this); }
        AGE_API const char* GetDirName() override       { return ageHook::Thunk<0x4D5BC0 >::Call<const char*>(this); }
        AGE_API char* GetClassName() override           { return ageHook::Thunk<0x4D5FA0 >::Call<char*>(this); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<vehGyro, asNode>("vehGyro")
                //properties
                .addProperty("DriftEnabled", &getDriftEnabled, &setDriftEnabled)
                .addProperty("SpinReverseEnabled", &getSpinReverseEnabled, &setSpinReverseEnabled)
                .addProperty("PitchRollEnabled", &getPitchRollEnabled, &setPitchRollEnabled)

                .addVariableRef("Drift", &vehGyro::Drift)
                .addVariableRef("Spin180", &vehGyro::Spin180)
                .addVariableRef("Reverse180", &vehGyro::Reverse180)
                .addVariableRef("Pitch", &vehGyro::Pitch)
                .addVariableRef("Roll", &vehGyro::Roll)

                .addFunction("Init", &Init)

                .endClass();
        }
    };


    ASSERT_SIZEOF(vehGyro, 0x30);

    // Lua initialization

}