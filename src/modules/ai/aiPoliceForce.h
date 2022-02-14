#pragma once
#include <modules\ai.h>

namespace MM2
{
    // Forward declarations
    class aiPoliceForce;

    // External declarations
    extern class vehCar;

    // Class definitions

    class aiPoliceForce {
    private:
        Timer timer;
        short NumPerps;
        short NumChasers[3];
        vehCar* PlayerCars[3];
        vehCar* CopCars[3][3];
    public:
        aiPoliceForce(void)                                       DONOTCALL;
        aiPoliceForce(const aiPoliceForce &&)                     DONOTCALL;

        AGE_API int UnRegisterCop(vehCar *cop, vehCar *perp)      { return hook::Thunk<0x551040>::Call<int>(this, cop, perp); }
        AGE_API int RegisterPerp(vehCar *cop, vehCar *perp)       { return hook::Thunk<0x550FA0>::Call<int>(this, cop, perp); }
        AGE_API int Find(vehCar *cop, vehCar *perp)               { return hook::Thunk<0x550F40>::Call<int>(this, cop, perp); }
        AGE_API int State(vehCar *cop, vehCar *perp, float a3)    { return hook::Thunk<0x551120>::Call<int>(this, cop, perp, a3); }
        AGE_API void Reset()                                      { hook::Thunk<0x550EF0>::Call<void>(this); }
    };

    ASSERT_SIZEOF(aiPoliceForce, 0x3C);

    // Lua initialization

}