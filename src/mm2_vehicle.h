#pragma once
#include "mm2_common.h"

namespace MM2
{
    // Forward declarations
    extern class vehCarAudioContainer;

    class vehCarAudioContainer {
    protected:
        static AGEHook<0x4D0C80>::Func<void> $SetSirenCSVName;
    public:
        AGE_API static void SetSirenCSVName(LPCSTR name) {
            $SetSirenCSVName(name);
        };
    };

    HOOK_API AGEHook<0x6AFFE4>::Type<int> vehPoliceCarAudio_iNumCopsPursuingPlayer;
}