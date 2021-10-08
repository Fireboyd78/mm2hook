#pragma once
#include <modules\vehicle.h>

namespace MM2
{
    // Forward declarations
    class vehNitroCarAudio;

    // External declarations


    // Class definitions

    class vehNitroCarAudio {
    private:
        byte _buffer[0x13C];
    public:
        AGE_API void PlayHorn()
        {
            auto hornSound = *getPtr<AudSoundBase*>(this, 0x10C);

            if (hornSound != nullptr)
            {
                if (!hornSound->IsPlaying())
                {
                    hornSound->SetPlayPosition(0);
                    hornSound->PlayLoop(-1.f, -1.f);
                }
            }
        }

        AGE_API void StopHorn()
        {
            auto hornSound = *getPtr<AudSoundBase*>(this, 0x10C);

            if (hornSound != nullptr)
            {
                if (hornSound->IsPlaying())
                    hornSound->Stop();
            }
        }
    };

    // Lua initialization

}