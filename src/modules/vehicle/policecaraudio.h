#pragma once
#include <modules\vehicle.h>

#include "caraudio.h"

namespace MM2
{
    // Forward declarations
    class vehPoliceCarAudio;

    // External declarations


    // Class definitions

    class vehPoliceCarAudio : public vehCarAudio {
    private:
        byte _buffer[0x118];
    public:
        static hook::Type<int> iNumCopsPursuingPlayer;

        AGE_API void StartSiren(int a1)                   { hook::Thunk<0x4D4B20>::Call<void>(this, a1); }
        AGE_API void StopSiren()                          { hook::Thunk<0x4D4C20>::Call<void>(this); }
        AGE_API void PlayExplosion()                      { hook::Thunk<0x4D4C90>::Call<void>(this); }
    };
    ASSERT_SIZEOF(vehPoliceCarAudio, 0x248);

    // Lua initialization

}