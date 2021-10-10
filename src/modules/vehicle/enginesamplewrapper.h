#pragma once
#include <modules\vehicle.h>

namespace MM2
{
    // Forward declarations
    class vehEngineSampleWrapper;

    // External declarations


    // Class definitions

    class vehEngineSampleWrapper {
    private:
        AudSoundBase* EngineSound;
        float MinVolume;
        float MaxVolume;
        float MinVolumeBackup;
        float MaxVolumeBackup;
        int field_14;
        int field_18;
        float FadeInStartRPM;
        float FadeOutStartRPM;
        float FadeInEndRPM;
        float FadeOutEndRPM;
        float MinPitch;
        float MaxPitch;
        float PitchShiftStartRPM;
        float PitchShiftEndRPM;
        float PitchDivisor;
        bool EchoOn;
        bool UseOldMode;
        byte gap2;
        byte gap3;
        float VolumeDivisor;
        float VolInverseRpm;
        int SoundIndex;
    };
    ASSERT_SIZEOF(vehEngineSampleWrapper, 0x50);

    // Lua initialization

}