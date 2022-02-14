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
        byte _buffer[0x976C];
    protected:
        static hook::Field<0x10, vehCar> _vehCar;
        static hook::Field<0x27C, unsigned short> _state;
    public:
        aiVehiclePhysics(void)                              DONOTCALL;
        aiVehiclePhysics(const aiVehiclePhysics &&)         DONOTCALL;

        inline vehCar * getCar()
        {
            return _vehCar.ptr(this);
        }

        inline unsigned short getState()
        {
            return _state.get(this);
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