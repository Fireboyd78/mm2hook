#pragma once
#include <modules\ai.h>

#include "aiVehiclePhysics.h"

namespace MM2
{
    // Forward declarations
    class aiRouteRacer;

    // External declarations
    

    // Class definitions

    class aiRouteRacer : public aiVehiclePhysics {
    private:
        byte _buffer[0x24];
    public:
        aiRouteRacer(void)                                  DONOTCALL;
        aiRouteRacer(const aiRouteRacer &&)                 DONOTCALL;
    };

    ASSERT_SIZEOF(aiRouteRacer, 0x9794);

    // Lua initialization

}