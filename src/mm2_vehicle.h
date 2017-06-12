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
            HOOK_EXPORT(0x4D0C80, _Func<void>, SetSirenCSVName);
        }
    }

    class vehCarAudioContainer {
    public:
        AGE_API static void SetSirenCSVName(LPCSTR name) {
            $::vehCarAudioContainer::SetSirenCSVName(name);
        };
    };

    class vehPoliceCarAudio {
    public:
        static ageHook::Type<int> iNumCopsPursuingPlayer;
    };
}