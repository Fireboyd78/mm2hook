#pragma once
#include <modules\ai.h>

namespace MM2
{
    // Forward declarations
    class aiGoal;
    class aiGoalAvoidPlayer;

    // External declarations


    // Class definitions
    class aiGoal {
    private:
        int field_0;
        int UpdateCount;
    };
    ASSERT_SIZEOF(aiGoal, 8);

    class aiGoalAvoidPlayer : public aiGoal {
    private:
        int field_8;
        float Side;
        float Heading;
        float PlayerSideReactDist;
        aiRailSet* RailSet;
        aiVehicleAmbient* VehicleAmbient;
        int field_20;
    };
    ASSERT_SIZEOF(aiGoalAvoidPlayer, 0x24);

    // Lua initialization

}