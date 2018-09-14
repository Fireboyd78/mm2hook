#pragma once
#include <modules\ai.h>

namespace MM2
{
    // Forward delcarations
    class aiPoliceForce;

    // External declarations


    // Class definitions

    class aiPoliceForce {
    private:
        byte _buffer[0x3C];
    public:
        aiPoliceForce(void)                                 DONOTCALL;
        aiPoliceForce(const aiPoliceForce &&)               DONOTCALL;
    };

    // Lua initialization

}