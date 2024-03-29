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
        hook::Field<0x9778, short> _id;
        hook::Field<0x977A, short> _policeState;
        hook::Field<0x977C, short> _lastPoliceState;
        hook::Field<0x977E, short> _apprehendState;
        hook::Field<0x9794, float> _followCarDistance;
        hook::Field<0x9798, short> _flagCount;
        hook::Field<0x9792, short> _componentType;
        hook::Field<0x97A4, short> _componentId;
        byte _buffer[0x9866];
    public:
        aiPoliceOfficer(void)                               DONOTCALL;
        aiPoliceOfficer(const aiPoliceOfficer &&)           DONOTCALL;

        inline aiVehiclePhysics * getVehiclePhysics()
        {
            return _physics.ptr(this);
        }

        inline short getId()
        {
            return _id.get(this);
        }

        inline short getState()
        {
            return getVehiclePhysics()->getState();
        }

        inline void setState(int state)
        {
            getVehiclePhysics()->setState(state);
        }

        inline short getApprehendState()
        {
            return _apprehendState.get(this);
        }

        inline vehCar * getFollowedCar()
        {
            return _followedCar.get(this);
        }

        inline void setFollowedCar(vehCar* car)
        {
            _followedCar.set(this, car);
        }

        inline vehCar * getCar()
        {
            return getVehiclePhysics()->getCar();
        }

        /// <summary>
        /// The state from aiPoliceForce::State
        /// </summary>        
        inline short getPoliceState()
        {
            return _policeState.get(this);
        }

        inline void setPoliceState(int state)
        {
            _policeState.set(this, state);
        }

        inline bool InPersuit()
        {
            return _policeState.get(this) != 0;
        }

        inline bool Destroyed()
        {
            return _policeState.get(this) == 12;
        }

        inline short getLastPoliceState()
        {
            return _lastPoliceState.get(this);
        }

        inline void setLastPoliceState(int state)
        {
            _lastPoliceState.set(this, state);
        }

        inline float getFollowCarDistance()
        {
            return _followCarDistance.get(this);
        }

        inline void setFollowCarDistance(float distance)
        {
            _followCarDistance.set(this, distance);
        }

        inline short getFlagCount()
        {
            return _flagCount.get(this);
        }

        inline short getComponentType()
        {
            return _componentType.get(this);
        }

        inline short getComponentId()
        {
            return _componentId.get(this);
        }

        AGE_API void Reset()                                { hook::Thunk<0x53DAA0>::Call<void>(this); }
        AGE_API void StartSiren()                           { hook::Thunk<0x53DBF0>::Call<void>(this); }
        AGE_API void StopSiren()                            { hook::Thunk<0x53DC40>::Call<void>(this); }
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