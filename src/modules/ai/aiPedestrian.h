#pragma once
#include <modules\ai.h>

#include "aiPedAudio.h"

namespace MM2
{
    // Forward declarations
    class aiPedestrian;

    // External declarations
    extern class aiPedestrianInstance;

    // Class definitions

    class aiPedestrian {
    protected:
        ageHook::Field<0xC0, aiPedAudio> _audio;
        ageHook::Field<0x9C, aiPedestrianInstance *> _instance;
    public:
        inline aiPedestrianInstance * getInstance(void) const {
            return _instance.get(this);
        }

        inline aiPedAudio * getAudio(void) const {
            return _audio.ptr(this);
        };
    };

    // Lua initialization

}