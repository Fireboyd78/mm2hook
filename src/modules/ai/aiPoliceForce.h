#pragma once
#include <modules\ai.h>

namespace MM2
{
    // Forward declarations
    class aiPoliceForce;

    // External declarations


    // Class definitions

    class aiPoliceForce {
    private:
        byte _buffer[0x3C];
    public:
        aiPoliceForce(void)                                       DONOTCALL;
        aiPoliceForce(const aiPoliceForce &&)                     DONOTCALL;

        AGE_API int UnRegisterCop(vehCar *perp, vehCar *cop)      { return hook::Thunk<0x551040>::Call<int>(this, perp, cop); }
    };

    // Lua initialization

}