#pragma once
#include <modules\ai.h>

#include "aiVehicle.h"
#include "aiPath.h"

namespace MM2
{
    // Forward declarations
    class aiCableCar;

    // External declarations


    // Class definitions

    class aiCableCar : public aiVehicle {
    private:
        byte _buffer[0x180];
    public:
        aiCableCar(void)                                    DONOTCALL;
        aiCableCar(const aiCableCar &&)                     DONOTCALL;
        ~aiCableCar(void)                                   DONOTCALL;

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

    ASSERT_SIZEOF(aiCableCar, 0x184);

    // Lua initialization

}