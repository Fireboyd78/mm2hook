#pragma once
#include <modules\vehicle.h>

namespace MM2
{
    // Forward declarations
    class vehAxle;

    // External declarations
    extern class vehCarSim;
    extern class vehWheel;

    // Class definitions
    class vehAxle : public asNode {
    private:
        vehCarSim *m_CarSimPtr;
        Matrix34 AxlePivot;
        Matrix34 AxleMatrix;
        vehWheel *LeftWheel;
        vehWheel *RightWheel;
        float unknown132; //something to do with angle from wheel to pivot
        float unknown136; //something to do with angle from wheel to pivot
        float ScaledTorqueCoef;
        float ScaledDampCoef;
        float TorqueCoef;
        float DampCoef;
    public:
        vehWheel * getLeftWheel() {
            return this->LeftWheel;
        }

        vehWheel * getRightWheel() {
            return this->RightWheel;
        }
    public:
        AGE_API vehAxle()                                   { hook::Thunk<0x4D9990>::Call<void>(this); }

        AGE_API void Init(vehCarSim *carSim, const char *vehicleBasename, const char *axleName, vehWheel *wheelL, vehWheel *wheelR)                     
                                                            { hook::Thunk<0x4D9A50>::Call<void>(this, carSim, vehicleBasename, axleName, wheelL, wheelR); }
        AGE_API void ComputeConstants()                     { hook::Thunk<0x4D9A20>::Call<void>(this); }

        /*
            asNode virtuals
        */

        AGE_API void Update() override                      { hook::Thunk<0x4D9B20>::Call<void>(this); }
        AGE_API void FileIO(datParser& parser) override     { hook::Thunk<0x4D9CA0>::Call<void>(this); }
        AGE_API char * GetClassName() override              { return hook::Thunk<0x4D9D20>::Call<char*>(this); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<vehAxle, asNode>("vehAxle")
                //properties
                .addVariableRef("TorqueCoef", &vehAxle::TorqueCoef)
                .addVariableRef("DampCoef", &vehAxle::DampCoef)

                .addFunction("Init", &Init)
                .addFunction("ComputeConstants", &ComputeConstants)

                .endClass();
        }
    };

    ASSERT_SIZEOF(vehAxle, 0x9C);

    // Lua initialization

}