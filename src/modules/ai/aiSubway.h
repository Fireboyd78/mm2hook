#pragma once
#include <modules\ai.h>

#include "aiVehicle.h"

namespace MM2
{
    // Forward declarations
    class aiSubway;

    // External declarations


    // Class definitions

    class aiSubway : public aiVehicle {
    private:
        byte _buffer[0x148];
    };

    ASSERT_SIZEOF(aiSubway, 0x14C);

    // Lua initialization

}