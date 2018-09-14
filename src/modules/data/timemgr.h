#pragma once
#include <modules\data.h>

namespace MM2
{
    // Forward declarations
    class datTimeManager;

    // External declarations


    // Class definitions

    class datTimeManager {
    public:
        static ageHook::Type<float> Seconds;
        static ageHook::Type<Timer> Time;

        AGE_API static void Reset(void) {
            ageHook::StaticThunk<0x4C6300>::Call<void>();
        };

        AGE_API static void Update(void) {
            ageHook::StaticThunk<0x4C6340>::Call<void>();
        };
    };

    // Lua initialization

}