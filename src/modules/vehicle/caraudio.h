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
        ageHook::Field<0x118, vehCarSim *> _sim;
    public:
        inline vehCarSim * getCarSim(void) const {
            return _sim.get(this);
        };
    };

    // Lua initialization

}