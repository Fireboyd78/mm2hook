#pragma once
#include <modules\vehicle.h>

#include "surfaceaudiosubdata.h"

namespace MM2
{
    // Forward declarations
    class vehSurfaceAudioData;

    // External declarations

    // Class definitions

    class vehSurfaceAudioData {
    private:
        AudSoundBase* SurfaceSound;
        vehSurfaceAudioSubData* SkidWaveData;
        int SampleIndex;
        int NumSkidSamples;
        float MaxSpeed;
        float MinSurfaceVolume;
        float MaxSurfaceVolume;
        float field_1c;
        float MinSkidVolume;
        float MaxSkidVolume;
        float field_28;
        float MinSurfacePitch;
        float MaxSurfacePitch;
        float field_34;
    public:
        inline vehSurfaceAudioSubData * getSkidWaveData(void) {
            return this->SkidWaveData;
        };
    };
    ASSERT_SIZEOF(vehSurfaceAudioData, 0x38);

    // Lua initialization

}