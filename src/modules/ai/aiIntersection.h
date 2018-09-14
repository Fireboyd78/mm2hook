#pragma once
#include <modules\ai.h>

#include "aiPath.h"
#include "aiVehicle.h"
#include "aiTrafficLight.h"
#include "aiVehicle.h"

namespace MM2
{
    // Forward declarations
    class aiIntersection;

    // External declarations


    // Class definitions

    class aiIntersection {
    public:
        aiPath * *paths;
        int16_t pathCount;
        aiVehicle *prevVeh;
        aiVehicle *nextVeh;
        uint16_t roomId;
        uint16_t id;
        Vector3 center;
        aiTrafficLightSet *trafficLights;
        aiObstacle *vehicles;
        aiObstacle *bangers;
    public:
        aiIntersection(void)                                DONOTCALL;
        aiIntersection(const aiIntersection &&)             DONOTCALL;
    };

    ASSERT_SIZEOF(aiIntersection, 0x2C);

    // Lua initialization

}