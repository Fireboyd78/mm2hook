#pragma once
#include "mm2_common.h"
#include "mm2_inst.h"

namespace MM2
{
    // Forward declarations
    class vehCar;
    class vehCarAudioContainer;
    class vehPoliceCarAudio;

    class vehCar {
        // vehCarSim: 0xB8 (size: ~0x1560)
    public:
        void SetDrivable(bool unk, bool unk1) {
            ageHook::Thunk<0x42C2C0>::Call<void>(this, unk, unk1);
        }

        virtual lvlInstance* GetInst() {
            return ageHook::Thunk<0x42CA80>::Call<lvlInstance *>(this);
        }
    private:
        byte _buffer[0x25C];
    };

    class vehCarAudioContainer {
    public:
        AGE_API static void SetSirenCSVName(LPCSTR name) {
            ageHook::StaticThunk<0x4D0C80>::Call<void>(name);
        };
    };

    class vehPoliceCarAudio {
    public:
        static ageHook::Type<int> iNumCopsPursuingPlayer;
    };
}