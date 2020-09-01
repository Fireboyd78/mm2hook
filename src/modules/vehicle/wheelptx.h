#pragma once
#include <modules\vehicle.h>

namespace MM2
{
    // Forward declarations
    class vehWheelPtx;

    // External declarations
    extern class asBirthRule;
    extern class asParticles;
    extern class vehWheel;

    // Class definitions
    class vehWheelPtx : public asNode {
    public:
        AGE_API vehWheelPtx()                                  { ageHook::Thunk<0x4D1C00>::Call<void>(this); }
        AGE_API ~vehWheelPtx()                                 { ageHook::Thunk<0x4D1C60>::Call<void>(this); }

        AGE_API void UpdateWheel(vehWheel *a1)                 { ageHook::Thunk<0x4D1E90>::Call<void>(this, a1); }
        AGE_API void Blast(vehWheel *a1, float a2, int a3, int a4)
                                                               { ageHook::Thunk<0x4D1EF0>::Call<void>(this, a1, a2, a3, a4); }
        /*
            asNode virtuals
        */

        AGE_API void Reset() override                          { ageHook::Thunk<0x4D1DC0>::Call<void>(this); }
        AGE_API void Update() override                         { ageHook::Thunk<0x4D1E40>::Call<void>(this); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<vehWheelPtx, asNode>("vehWheelPtx")
                //functions
                .addFunction("UpdateWheel", &UpdateWheel)
                .addFunction("Blast", &Blast)
            .endClass();
        }
    };

    // Lua initialization

}