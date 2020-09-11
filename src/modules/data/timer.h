#pragma once
#include <modules\data.h>

namespace MM2
{
    // Forward declarations
    class Timer;

    // External declarations


    // Class definitions
    class Timer {
        uint ticks;
    public:
        Timer() {
            hook::Thunk<0x4C7840>::ThisCall<void>(this);
        }

        constexpr operator uint() const {
            return ticks;
        }

        inline void Update() {
            ticks = Ticks();
        }

        static uint Ticks() {
            return hook::StaticThunk<0x4C77E0>::Call<uint>();
        }

        static uint QuickTicks() {
            return hook::StaticThunk<0x4C7810>::Call<uint>();
        }
    };

    // Lua initialization

}