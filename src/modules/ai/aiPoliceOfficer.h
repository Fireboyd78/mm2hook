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
    public:
        static float DefaultSpeedLimit;
        static float SpeedLimitTolerance;
        static float SkidPlayTime;
        static float HornPlayTime;
        static int MaximumNumCops;
        static bool FlyingCopFix;
    private:
        hook::Field<0x04, aiVehiclePhysics> _physics;
        hook::Field<0x9774, vehCar*> _followedCar;
        hook::Field<0x9778, unsigned short> _id;
        hook::Field<0x977A, unsigned short> _policeState;
        hook::Field<0x977E, unsigned short> _apprehendState;
        byte _buffer[0x986B];
    public:
        aiPoliceOfficer(void)                               DONOTCALL;
        aiPoliceOfficer(const aiPoliceOfficer &&)           DONOTCALL;

        inline aiVehiclePhysics * getVehiclePhysics()
        {
            return _physics.ptr(this);
        }

        inline int getId()
        {
            return _id.get(this);
        }

        inline int getState()
        {
            return getVehiclePhysics()->getState();
        }

        inline int getApprehendState()
        {
            return _apprehendState.get(this);
        }

        inline vehCar * getFollowedCar()
        {
            return _followedCar.get(this);
        }

        inline vehCar * getCar()
        {
            return getVehiclePhysics()->getCar();
        }

        /// <summary>
        /// The state from aiPoliceForce::State
        /// </summary>        
        inline int getPoliceState()
        {
            return _policeState.get(this);
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
        AGE_API BOOL Fov(vehCar *perpCar)                   { return hook::Thunk<0x53E2A0>::Call<BOOL>(this, perpCar); }
        AGE_API BOOL Collision(vehCar *perpCar)             { return hook::Thunk<0x53E370>::Call<BOOL>(this, perpCar); }
        AGE_API BOOL HitMe(vehCar *perpCar)                 { return hook::Thunk<0x53E390>::Call<BOOL>(this, perpCar); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<aiPoliceOfficer>("aiPoliceOfficer")
                .addPropertyReadOnly("Car", &getCar)
                .addPropertyReadOnly("FollowedCar", &getFollowedCar)
                .addPropertyReadOnly("State", &getState)
                .addPropertyReadOnly("PoliceState", &getPoliceState)
                .addPropertyReadOnly("ApprehendState", &getApprehendState)
                .addPropertyReadOnly("ID", &getId)

                .addFunction("StartSiren", &StartSiren)
                .addFunction("StopSiren", &StopSiren)

                .endClass();
        }
    };

    ASSERT_SIZEOF(aiPoliceOfficer, 0x9870);

    // Lua initialization

}