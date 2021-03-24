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

        AGE_API void StopSiren()                          { hook::Thunk<0x4D4C20>::Call<void>(this); }
        AGE_API void PlayExplosion()                      { hook::Thunk<0x4D4C90>::Call<void>(this); }
    };

    // Lua initialization

}