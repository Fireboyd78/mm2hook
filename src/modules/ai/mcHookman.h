#pragma once
#include <modules\ai.h>

#include "aiVehiclePhysics.h"

namespace MM2
{
    // Forward declarations
    class mcHookman;

    // External declarations


    // Class definitions

    class mcHookman : public aiVehiclePhysics {
    private:
        byte _buffer[0x100];
    public:
        mcHookman(void)                                     DONOTCALL;
        mcHookman(const mcHookman &&)                       DONOTCALL;
    };

    ASSERT_SIZEOF(mcHookman, 0x9870);

    // Lua initialization

}