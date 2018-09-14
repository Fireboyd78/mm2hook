#pragma once
#include <modules\node.h>

namespace MM2
{
    // Forward delcarations
    class asCullable;

    // External declarations
    extern class Base;

    // Class definitions

    class asCullable : public Base {
    public:
        virtual AGE_API ~asCullable() {
            PUSH_VTABLE();
            ageHook::Thunk<0x460EA0>::Call<void>(this);
            POP_VTABLE();
        };

        virtual AGE_API void Cull(void) {
            ageHook::Thunk<0x4A3440>::Call<void>(this);
        };
    };

    // Lua initialization

}