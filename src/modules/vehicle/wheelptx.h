#pragma once
#include <modules\vehicle.h>
#include <..\mm2_particle.h>

namespace MM2
{
    // Forward declarations
    class vehWheelPtx;

    // External declarations
    extern class asBirthRule;
    extern class asParticles;
    extern class vehWheel;
    extern class vehCarSim;

    // Class definitions
    class vehWheelPtx : public asNode {
    private:
        vehCarSim* m_CarSimPtr;
        asParticles m_Particles;
        float m_PtxTimers[2];
    public:
        AGE_API vehWheelPtx()                                  { hook::Thunk<0x4D1C00>::Call<void>(this); }
        AGE_API ~vehWheelPtx()                                 { hook::Thunk<0x4D1C60>::Call<void>(this); }

        AGE_API void UpdateWheel(vehWheel *a1)                 { hook::Thunk<0x4D1E90>::Call<void>(this, a1); }
        AGE_API void Blast(vehWheel *a1, float a2, int a3, int a4)
                                                               { hook::Thunk<0x4D1EF0>::Call<void>(this, a1, a2, a3, a4); }
        /*
            asNode virtuals
        */

        AGE_API void Reset() override                          { hook::Thunk<0x4D1DC0>::Call<void>(this); }
        AGE_API void Update() override                         { hook::Thunk<0x4D1E40>::Call<void>(this); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<vehWheelPtx, asNode>("vehWheelPtx")
                //functions
                .addFunction("UpdateWheel", &UpdateWheel)
                .addFunction("Blast", &Blast)
            .endClass();
        }
    };

    ASSERT_SIZEOF(vehWheelPtx, 0x78);

    // Lua initialization

}