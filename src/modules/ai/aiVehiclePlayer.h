#pragma once
#include <modules\ai.h>

#include "aiPath.h"
#include "aiVehicle.h"

namespace MM2
{
    // Forward declarations
    class aiVehiclePlayer;

    // External declarations
    extern class vehCar;

    // Class definitions

    class aiVehiclePlayer : public aiVehicle {
    private:
        byte _buffer[0x2C];
    protected:
        static hook::Field<0x14, class vehCar *> _vehCar;
    public:
        aiVehiclePlayer(void)                               DONOTCALL;
        aiVehiclePlayer(const aiVehiclePlayer &&)           DONOTCALL;

        inline class vehCar * getCar() {
            return _vehCar.get(this);
        }

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
    };

    ASSERT_SIZEOF(aiVehiclePlayer, 0x30);

    // Lua initialization

}