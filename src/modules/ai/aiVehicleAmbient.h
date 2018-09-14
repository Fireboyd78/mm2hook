#pragma once
#include <modules\ai.h>

#include "aiVehicleSpline.h"

namespace MM2
{
    // Forward declarations
    class aiVehicleAmbient;

    // External declarations


    // Class definitions

    class aiVehicleAmbient : public aiVehicleSpline {
    private:
        byte _buffer[0x14];
    public:
        aiVehicleAmbient(void)                              DONOTCALL;
        aiVehicleAmbient(const aiVehicleAmbient &&)         DONOTCALL;

        void Update(void) override                          FORWARD_THUNK;
        void Reset(void) override                           FORWARD_THUNK;
        int Type(void) override                             FORWARD_THUNK;
        void DrawId(void) override                          FORWARD_THUNK;
        void ReplayDebug(void) override                     FORWARD_THUNK;

        void Impact(int a1) override                        FORWARD_THUNK;
    };

    ASSERT_SIZEOF(aiVehicleAmbient, 0x1A0);

    // Lua initialization

}