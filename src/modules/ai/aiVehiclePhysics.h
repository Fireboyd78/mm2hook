#pragma once
#include <modules\ai.h>

#include "aiPath.h"
#include "aiVehicle.h"

namespace MM2
{
    // Forward declarations
    class aiVehiclePhysics;
    struct aiRouteNode;

    // External declarations
    extern class vehCar;

    // Class definitions
    struct  aiRouteNode
    {
        int unk_0x00;
        Vector3 Position;
        float Angle;
        int unk_14;
        short unk_18;
        short RoadVertexIndex;
        short unk_1C;
        short unk_1E;
        short unk_20;
        short unk_22;
    };


    class aiVehiclePhysics : public aiVehicle {
    private:
        hook::Field<0x10, vehCar> _vehCar;
        hook::Field<0x27C, short> _state;
        hook::Field<0x9690, float> _brake;
        hook::Field<0x9694, float> _throttle;
        hook::Field<0x9698, float> _steering;
        byte _buffer[0x9764];
    public:
        aiVehiclePhysics(void)                              DONOTCALL;
        aiVehiclePhysics(const aiVehiclePhysics &&)         DONOTCALL;

        inline vehCar * getCar()
        {
            return _vehCar.ptr(this);
        }

        inline short getState()
        {
            return _state.get(this);
        }

        inline void setState(int state)
        {
            _state.set(this, state);
        }

        inline float getBrake()
        {
            return _brake.get(this);
        }

        inline float getThrottle()
        {
            return _throttle.get(this);
        }

        inline float getSteering()
        {
            return _steering.get(this);
        }

        AGE_API void DriveRoute(int a1)                     { hook::Thunk<0x55A8F0>::Call<void>(this, a1); }
        AGE_API void Mirror(vehCar* car)                    { hook::Thunk<0x55A570>::Call<void>(this, car); }

        void Position(Vector3 &a1) override                 FORWARD_THUNK;
        float Speed(void) override                          FORWARD_THUNK;
        int CurrentRoadIdx(aiPath **a1, const bool *a2, int *a3) override
                                                            FORWARD_THUNK;
        int CurrentRdVert(void) override                    FORWARD_THUNK;

        void Update(void) override                          FORWARD_THUNK;
        void Reset(void) override                           FORWARD_THUNK;
        int Type(void) override                             FORWARD_THUNK;
        Matrix34 & GetMatrix(void) override                 FORWARD_THUNK;
        float FrontBumperDistance(void) override            FORWARD_THUNK;
        float BackBumperDistance(void) override             FORWARD_THUNK;
        float LSideDistance(void) override                  FORWARD_THUNK;
        float RSideDistance(void) override                  FORWARD_THUNK;
        int CurrentLane(void) override                      FORWARD_THUNK;
        int CurrentRoadId(void) override                    FORWARD_THUNK;
        void DrawId(void) override                          FORWARD_THUNK;
        void ReplayDebug(void) override                     FORWARD_THUNK;

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<aiVehiclePhysics, aiVehicle>("aiVehiclePhysics")
                .addPropertyReadOnly("Car", &getCar)
                .addPropertyReadOnly("State", &getState)
                .endClass();
        }
    };

    ASSERT_SIZEOF(aiVehiclePhysics, 0x9770);

    // Lua initialization

}