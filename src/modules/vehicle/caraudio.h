#pragma once
#include <modules\vehicle.h>

namespace MM2
{
    // Forward declarations
    class vehCarAudio;

    // External declarations


    // Class definitions

    class vehCarAudio {
    private:
        byte _buffer[0x130];
    protected:
        hook::Field<0x118, vehCarSim *> _sim;
    public:
        inline vehCarSim * getCarSim(void) const {
            return _sim.get(this);
        };

        AGE_API void PlayHorn()                           { hook::Thunk<0x4DC1D0>::Call<void>(this); }
        AGE_API void StopHorn()                           { hook::Thunk<0x4DC210>::Call<void>(this); }
    };

    // Lua initialization

}