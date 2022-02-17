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

        AGE_API BOOL Fov(vehCar *perpCar)
        {
            auto policeMtx = &this->getVehiclePhysics()->GetMatrix();
            auto perpMtx = perpCar->getCarSim()->getICS()->GetMatrix();

            lvlSegment segment;
            lvlIntersection isect;

            Vector3 vec;
            Vector3 vec2;

            vec.X = perpMtx->m30 - policeMtx->m30;
            vec.Y = perpMtx->m31 - policeMtx->m31;
            vec.Z = perpMtx->m32 - policeMtx->m32;

            float distX = vec.X * policeMtx->m00 + vec.Y * policeMtx->m01 + vec.Z * policeMtx->m02;
            float distZ = vec.X * -policeMtx->m20 + vec.Y * -policeMtx->m21 + vec.Z * -policeMtx->m22;

            float dist = atan2(distX, distZ);
            if (dist <= -1.57f || dist >= 1.57f)
                return FALSE;

            vec.X = perpMtx->m30;
            vec.Y = perpMtx->m31;
            vec.Z = perpMtx->m32;

            vec2.X = policeMtx->m30;
            vec2.Y = policeMtx->m31;
            vec2.Z = policeMtx->m32;

            vec.Y += 1.f;
            vec2.Y += 1.f;

            segment.Set(vec, vec2, 0, nullptr);
            return dgPhysManager::Instance->Collide(segment, &isect, 0, nullptr, 0x20, 0) == FALSE;
        }

        AGE_API BOOL Collision(vehCar *perpCar)
        {
            return *getPtr<int>(perpCar->getCarSim()->getICS(), 0xDC) != 0;
        }

        AGE_API BOOL HitMe(vehCar *perpCar)
        {
            return (this->getCar()->getModel()->GetFlags() >> 15) & 1;
        }

        AGE_API BOOL IsPerpACop(vehCar *perpCar)
        {
            char* vehName = perpCar->getCarDamage()->GetName(); // we can't use vehCarSim because the game forces vpcop to vpmustang99...
            return perpCar->getAudio()->IsPolice(vehName);
        }

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