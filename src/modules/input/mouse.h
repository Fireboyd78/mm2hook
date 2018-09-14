#pragma once
#include <modules\input.h>

namespace MM2
{
    // Forward delcarations
    class ioMouse;

    // External declarations


    // Class definitions

    class ioMouse {
    public:
        static ageHook::Type<float> InvWidth;
        static ageHook::Type<float> InvHeight;
    };

    // Lua initialization

}