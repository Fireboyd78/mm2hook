#pragma once
#include <modules\vehicle.h>

#include "surfaceaudio.h"
#include "engineaudio.h"

namespace MM2
{
    // Forward declarations
    class vehCarAudio;

    // External declarations


    // Class definitions

    class vehCarAudio : public Aud3DObject {
    private:
        float field_60;
        float field_64;
        int field_68;
        int field_6c;
        float MinAmpSpeed;
        float field_74;
        float HornVolume;
        float Pan;
        vehSurfaceAudio SurfaceAudio;
        vehEngineAudio EngineAudio;
        AudSoundBase* HornSound;
        AudSoundBase* ClutchSound;
        float ClutchSampleVolume;
        vehCarSim* CarSim;
        int HornSampleIndex;
        int ClutchSampleIndex;
        int LastGear;
        int field_128;
        byte field_12c;
        bool EchoOn;
        byte field_12e;
        byte field_12f;
    public:
        inline float getPan() {
            return this->Pan;
        }

        inline void setPan(float pan) {
            this->Pan = pan;
        }

        inline vehSurfaceAudio * getSurfaceAudio(void) {
            return &this->SurfaceAudio;
        };

        inline AudSoundBase * getHornSound(void) {
            return this->HornSound;
        };

        inline vehCarSim * getCarSim(void) {
            return this->CarSim;
        };

        AGE_API void PlayHorn()                           { hook::Thunk<0x4DC1D0>::Call<void>(this); }
        AGE_API void StopHorn()                           { hook::Thunk<0x4DC210>::Call<void>(this); }
    };
    ASSERT_SIZEOF(vehCarAudio, 0x130);

    // Lua initialization

}