#pragma once
#include <modules\ai.h>

#include "aiVehiclePhysics.h"
#include "aiCTFRacer.h"

namespace MM2
{
    // Forward declarations
    class aiCTFRacer;

    // External declarations
    extern class Base;

    // Class definitions

    class aiCTFRacer : public Base {
    private:
        byte _buffer1[0x28];
        aiVehiclePhysics physics;
        byte _buffer2[0xDC];
    public:
        aiCTFRacer(void)                                    DONOTCALL;
        aiCTFRacer(const aiCTFRacer &&)                     DONOTCALL;
    };

    ASSERT_SIZEOF(aiCTFRacer, 0x9878);

    // Lua initialization

}