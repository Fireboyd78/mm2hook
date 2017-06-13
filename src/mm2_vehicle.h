#pragma once
#include "mm2_common.h"
#include "mm2_inst.h"

namespace MM2
{
    // Forward declarations
    extern class vehCarAudioContainer;

    namespace $
    {
        namespace vehCar {
            HOOK_EXPORT(0x0042C2C0, _MemberFunc<void>, SetDrivable);
            HOOK_EXPORT(0x0042CA80, _MemberFunc<MM2::lvlInstance *>, GetInst);
        }
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

    class vehCar {
        // vehCarSim: 0xB8 (size: ~0x1560)
    public:
        void SetDrivable(bool unk, bool unk1) {
            $::vehCar::SetDrivable(this, unk, unk1);
        }
        virtual lvlInstance* GetInst() {
            return $::vehCar::GetInst(this);
        }
    private:
        byte _buffer[0x25C];
    };

    class vehPoliceCarAudio {
    public:
        static ageHook::Type<int> iNumCopsPursuingPlayer;
    };
}