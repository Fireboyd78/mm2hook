#pragma once
#include "mm2_common.h"

namespace MM2
{
    // Forward declarations
    extern class vehCarAudioContainer;

    namespace $
    {
        namespace vehCarAudioContainer
        {
            HOOK_API AGEHook<0x4D0C80>::Func<void> SetSirenCSVName;
        }
    }

    class vehCarAudioContainer {
    public:
        AGE_API static void SetSirenCSVName(LPCSTR name) {
            $::vehCarAudioContainer::SetSirenCSVName(name);
        };
    };

    HOOK_API AGEHook<0x6AFFE4>::Type<int> vehPoliceCarAudio_iNumCopsPursuingPlayer;
}