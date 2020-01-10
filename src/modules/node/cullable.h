#pragma once
#include <modules\node.h>

namespace MM2
{
    // Forward declarations
    class asCullable;

    // External declarations
    extern class Base;

    // Class definitions

    class asCullable : public Base {
    public:
        virtual AGE_API ~asCullable() {
            scoped_vtable x(this);
            ageHook::Thunk<0x460EA0>::Call<void>(this);
        };

        virtual AGE_API void Cull(void) {
            ageHook::Thunk<0x4A3440>::Call<void>(this);
        };

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<asCullable, Base>("asCullable")
                .endClass();
        }
    };

    // Lua initialization

}