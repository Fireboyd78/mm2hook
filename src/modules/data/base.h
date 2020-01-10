#pragma once
#include <modules\data.h>

namespace MM2
{
    // Forward declarations
    class Base;

    // External declarations


    // Class definitions

    class Base {
    public:
        virtual AGE_API ~Base() {
            scoped_vtable x(this);
            ageHook::Thunk<0x4C8200>::Call<void>(this);
        };

        //lua
        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<Base>("Base")
                .endClass();
        }

    };

    // Lua initialization

}