#pragma once
#include <modules\ai.h>
#include <modules\vehicle.h>

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

        AGE_API void Reset()                                { hook::Thunk<0x53DAA0>::Call<void>(this); }
        AGE_API void StartSiren()
        {
            auto trailer = this->getVehiclePhysics()->getCar()->getTrailer();

            if (trailer != nullptr) {
                auto siren = trailer->getSiren();

                if (siren != nullptr)
                    siren->Active = true;
            }

            hook::Thunk<0x53DBF0>::Call<void>(this);
        }

        AGE_API void StopSiren()
        {
            auto trailer = this->getVehiclePhysics()->getCar()->getTrailer();

            if (trailer != nullptr) {
                auto siren = trailer->getSiren();

                if (siren != nullptr)
                    siren->Active = false;
            }

            hook::Thunk<0x53DC40>::Call<void>(this);
        }

        AGE_API void DetectPerpetrator()                    { hook::Thunk<0x53DFD0>::Call<void>(this); }
        AGE_API void FollowPerpetrator()                    { hook::Thunk<0x53E410>::Call<void>(this); }
        AGE_API void ApprehendPerpetrator()                 { hook::Thunk<0x53E580>::Call<void>(this); }
        
    };

    ASSERT_SIZEOF(aiPoliceOfficer, 0x9870);

    // Lua initialization

}