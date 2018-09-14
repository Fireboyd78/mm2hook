#pragma once
#include <modules\ai.h>

namespace MM2
{
    // Forward declarations
    class aiVehicleManager;

    // External declarations
    extern class asNode;

    // Class definitions

    class aiVehicleManager : public asNode {
    public:
        static ageHook::Type<aiVehicleManager *> Instance;
    };

    // Lua initialization

}