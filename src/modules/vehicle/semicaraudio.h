#pragma once
#include <modules\vehicle.h>

#include "caraudio.h"

namespace MM2
{
    // Forward declarations
    class vehSemiCarAudio;

    // External declarations


    // Class definitions

    class vehSemiCarAudio : public vehCarAudio {
    private:
        float ReverseVolume;
        float AirBrakeVolume;
        AudSoundBase* ReverseSound;
        AudSoundBase* AirBrakeSound;
        int field_140;
        int field_144;
    public:
        inline float getReverseVolume() {
            return this->ReverseVolume;
        }

        inline void setReverseVolume(float volume) {
            this->ReverseVolume = volume;
        }

        inline float getAirBrakeVolume() {
            return this->AirBrakeVolume;
        }

        inline void setAirBrakeVolume(float volume) {
            this->AirBrakeVolume = volume;
        }

        inline AudSoundBase * getReverseSound() {
            return this->ReverseSound;
        }

        inline void setReverseSound(AudSoundBase* sound) {
            this->ReverseSound = sound;
        }

        inline AudSoundBase * getAirBrakeSound() {
            return this->AirBrakeSound;
        }

        inline void setAirBrakeSound(AudSoundBase* sound) {
            this->AirBrakeSound = sound;
        }

        inline int getField_140() {
            return this->field_140;
        }

        inline void setField_140(int value) {
            this->field_140 = value;
        }

        inline int getField_144() {
            return this->field_144;
        }

        inline void setField_144(int value) {
            this->field_144 = value;
        }
    };
    ASSERT_SIZEOF(vehSemiCarAudio, 0x148);

    // Lua initialization

}