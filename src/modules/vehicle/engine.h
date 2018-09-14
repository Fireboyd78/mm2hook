#pragma once
#include <modules\vehicle.h>

namespace MM2
{
    // Forward declarations
    class vehEngine;

    // External declarations


    // Class definitions

    class vehEngine : public asNode {
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

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<vehEngine, asNode>("vehEngine")
                .addFunction("ComputeConstants", &ComputeConstants)
                .addFunction("CalcTorqueAtFullThrottle", &CalcTorqueAtFullThrottle)
                .addFunction("CalcTorqueAtZeroThrottle", &CalcTorqueAtZeroThrottle)
                .addFunction("CalcTorque", &CalcTorque)
                .addFunction("CalcHPAtFullThrottle", &CalcHPAtFullThrottle)

                .endClass();
        }
    private:
        byte _buffer[0x84];

    };

    // Lua initialization

}