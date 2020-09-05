#pragma once
#include <modules\vehicle.h>

namespace MM2
{
    // Forward declarations
    class vehSuspension;

    // External declarations
    extern class vehCarSim;
    extern class vehWheel;

    // Class definitions
    class vehSuspension : public asNode {
    private:
        Matrix34 SuspensionPivot;
        Matrix34 SuspensionMatrix;
        vehCarSim* m_CarSimPtr;
        vehWheel* Wheel;
        float unknown128; //something to do with angle from wheel to pivot
        int Mode;
    public:
        vehWheel* getWheel() {
            return this->Wheel;
        }
    public:
        AGE_API vehSuspension()                             { ageHook::Thunk<0x4D9990>::Call<void>(this); }

        AGE_API void Init(vehCarSim* carSim, const char* vehicleBasename, const char* suspensionName, vehWheel* wheel)
                                                            { ageHook::Thunk<0x4D9810>::Call<void>(this, carSim, vehicleBasename, suspensionName, wheel); }

        /*
            asNode virtuals
        */

        AGE_API void Update() override                      { ageHook::Thunk<0x4D98B0>::Call<void>(this); }
        AGE_API void FileIO(datParser& parser) override     { ageHook::Thunk<0x4D9920>::Call<void>(this); }
        AGE_API char* GetClassName() override               { return ageHook::Thunk<0x4D9980>::Call<char*>(this); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<vehSuspension, asNode>("vehSuspension")
                //properties
                .addFunction("Init", &Init)

                .endClass();
        }
    };

    ASSERT_SIZEOF(vehSuspension, 0x88);

    // Lua initialization

}