#pragma once
#include <modules\vehicle.h>
#include <mm2_audio.h>

namespace MM2
{
    // Forward declarations
    class vehSurfaceAudio;

    // External declarations
    extern class vehSurfaceAudioData;

    // Class definitions

    class vehSurfaceAudio {
    private:
        int LastSurfaceSound;
        int field_4;
        int IsPlayingSkidSound;
        int SurfaceDataCount;
        int field_10;
        int TireWobbleSampleIndex;
        int field_18;
        int field_1c;
        float MaxSuspensionVolume;
        float MinSuspensionVolume;
        float VolumeMultiplier;
        float MinSuspensionVelocity;
        float MaxSuspensionVelocity;
        int field_34;
        float TireWobbleMinVolume;
        float TireWobbleMaxVolume;
        float TireWobbleMinPitch;
        float TireWobbleMaxPitch;
        float TireWobblePitchDivisor;
        float field_4c;
        float field_50;
        int field_54;
        int field_58;
        vehSurfaceAudioData** SurfaceDatas;
        AudSoundBase* SuspensionSound;
        AudSoundBase* WobbleSoundPtr;
        vehWheel* WHL0;
        vehWheel* WHL1;
        vehWheel* WHL2;
        vehWheel* WHL3;
        Vector3* PositionPtr;
        vehCarDamage* CarDamage;
        int field_80;
    public:
        inline vehSurfaceAudioData * getSurfaceDatas() {
            return this->SurfaceDatas[this->LastSurfaceSound];
        };
    };
    ASSERT_SIZEOF(vehSurfaceAudio, 0x84);

    // Lua initialization

}