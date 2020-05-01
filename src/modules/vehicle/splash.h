#pragma once
#include <modules\vehicle.h>

namespace MM2
{
    // Forward declarations
    class vehSplash;

    // External declarations


    // Class definitions

    class vehSplash : public asNode {
    protected:
        //Lua helpers
        void Deactivate() {
            this->setActive(false);
        }
    public:
        AGE_API vehSplash()                                 { ageHook::Thunk<0x4D6A00>::Call<void>(this); }
        AGE_API ~vehSplash()                                { ageHook::Thunk<0x4D6F30>::Call<void>(this); }

        /*
            asNode virtuals
        */

        AGE_API void Update() override                      { ageHook::Thunk<0x4D6BF0>::Call<void>(this); }
        AGE_API void Reset() override                       { ageHook::Thunk<0x4D6A70>::Call<void>(this); }

        /*
            vehSplash members
        */

        AGE_API void Activate(float waterHeight)            { ageHook::Thunk<0x4D6BD0>::Call<void>(this, waterHeight); }

        /*
            Lua Bindings
        */
        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<vehSplash, asNode>("vehSplash")
                .addFunction("Activate", &Activate)
                .addFunction("Deactivate", &Deactivate)
                .endClass();
        }
    };


    // Lua initialization

}