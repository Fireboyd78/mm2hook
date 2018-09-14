#pragma once
#include <modules\level.h>

namespace MM2
{
    // Forward declarations
    class lvlLevel;

    // External declarations


    // Class definitions

    class lvlLevel {
    public:
        static ageHook::Type<lvlLevel> Singleton;

        AGE_API void MoveToRoom(lvlInstance *instance, int room) {
            ageHook::Thunk<0x465480>::Call<void>(this, instance, room);
        }
    };

    // Lua initialization

}