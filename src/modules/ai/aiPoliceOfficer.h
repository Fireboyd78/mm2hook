#pragma once
#include <modules\ai.h>

#include "aiVehiclePhysics.h"

namespace MM2
{
    // Forward declarations
    class aiPoliceOfficer;

    // External declarations
    extern class aiVehiclePhysics;

    // Class definitions

    class aiPoliceOfficer {
    private:
        int unk_00;
        aiVehiclePhysics physics;
        byte _buffer[0xFC];
    public:
        aiPoliceOfficer(void)                               DONOTCALL;
        aiPoliceOfficer(const aiPoliceOfficer &&)           DONOTCALL;

        inline aiVehiclePhysics * getVehiclePhysics() {
            return &this->physics;
        }

        AGE_API void StopSiren()                            { hook::Thunk<0x53DC40>::Call<void>(this); }
    };

    ASSERT_SIZEOF(aiPoliceOfficer, 0x9870);

    // Lua initialization

}