#pragma once
#include <modules\vehicle.h>

namespace MM2
{
    // Forward declarations
    class vehAero;

    // External declarations


    // Class definitions
    class vehAero : public asNode {
    private:
        BOOL EnableAero;
        vehCarSim* m_CarSimPtr;
        Vector3 AngCDamp;
        Vector3 AngVelDamp;
        Vector3 AngVel2Damp;
        float Drag;
        float Down;
    public:
        bool getEnabled() {
            return this->EnableAero == TRUE;
        }

        void setEnabled(bool enabled) {
            this->EnableAero = (enabled) ? TRUE : FALSE;
        }
    public:
        AGE_API vehAero() { hook::Thunk<0x4D9320>::Call<void>(this); }

        /*
            asNode virtuals
        */

        AGE_API void Update() override                  { hook::Thunk<0x4D9360>::Call<void>(this); }
        AGE_API void FileIO(datParser& parser) override { hook::Thunk<0x4D96E0>::Call<void>(this); }
        AGE_API char* GetClassName() override           { return hook::Thunk<0x4D9790>::Call<char*>(this); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<vehAero, asNode>("vehAero")
                //properties
                .addProperty("Enabled", &getEnabled, &setEnabled)
                .addVariableRef("AngCDamp", &vehAero::AngCDamp)
                .addVariableRef("AngVelDamp", &vehAero::AngVelDamp)
                .addVariableRef("AngVel2Damp", &vehAero::AngVel2Damp)
                .addVariableRef("Drag", &vehAero::Drag)
                .addVariableRef("Down", &vehAero::Down)

                .endClass();
        }
    };

    ASSERT_SIZEOF(vehAero, 0x4C);

    // Lua initialization

}