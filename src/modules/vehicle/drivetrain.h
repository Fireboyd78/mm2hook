#pragma once
#include <modules\vehicle.h>

namespace MM2
{
    // Forward declarations
    class vehDriveTrain;

    // External declarations
    extern class vehCarSim;
    extern class vehEngine;

    // Class definitions
    class vehDriveTrain : public asNode {
    private:
        vehCarSim *VehCarSimPtr;
        vehEngine *VehEnginePtr;
    public:
        AGE_API void Attach()                              { ageHook::Thunk<0x4D9E20>::Call<void>(this); }
        AGE_API void Detach()                              { ageHook::Thunk<0x4D9E40>::Call<void>(this); }

        /*
            asNode virtuals
        */

        AGE_API void Reset() override                      { ageHook::Thunk<0x4D9E00>::Call<void>(this); }
        AGE_API void Update() override                     { ageHook::Thunk<0x4D9E90>::Call<void>(this); }
        AGE_API void FileIO(datParser &parser) override
                                                           { ageHook::Thunk<0x4DA570>::Call<void>(this); }
        AGE_API char * GetClassName() override             { return ageHook::Thunk<0x4DA600>::Call<char *>(this); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<vehDriveTrain, asNode>("vehDriveTrain")
                .addFunction("Attach", &Attach)
                .addFunction("Detach", &Detach)
            .endClass();
        }
    };

    ASSERT_SIZEOF(vehDriveTrain, 0x20);

    // Lua initialization

}