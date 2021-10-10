#pragma once
#include <modules\vehicle.h>

#include "caraudio.h"

namespace MM2
{
    // Forward declarations
    class vehNitroCarAudio;

    // External declarations


    // Class definitions

    class vehNitroCarAudio : public vehCarAudio {
    private:
        float NitroVolume;
        int field_134;
        AudSoundBase* NitroSound;
    public:
        inline float getNitroVolume() {
            return this->NitroVolume;
        }

        inline void setNitroVolume(float volume) {
            this->NitroVolume = volume;
        }

        inline AudSoundBase * getNitroSound() {
            return this->NitroSound;
        }
    };
    ASSERT_SIZEOF(vehNitroCarAudio, 0x13C);

    // Lua initialization

}