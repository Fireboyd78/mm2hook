#pragma once
#include <modules\vehicle.h>

namespace MM2
{
    // Forward declarations
    class vehPoliceCarAudio;

    // External declarations


    // Class definitions

    class vehPoliceCarAudio {
    public:
        static hook::Type<int> iNumCopsPursuingPlayer;
    };

    // Lua initialization

}