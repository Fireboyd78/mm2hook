#pragma once
#include <modules\vehicle.h>

namespace MM2
{
    // Forward declarations
    class vehSurfaceAudioSubData;

    // External declarations


    // Class definitions

    class vehSurfaceAudioSubData {
    private:
        float MinSlippage;
        float MaxSlippage;
        AudSoundBase* SkidSound;
        int SampleIndex;
    public:
        inline AudSoundBase * getSkidSound(void) {
            return this->SkidSound;
        };
    };
    ASSERT_SIZEOF(vehSurfaceAudioSubData, 0x10);

    // Lua initialization

}