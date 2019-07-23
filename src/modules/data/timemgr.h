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
        static ageHook::Type<float> InvSeconds;
        static ageHook::Type<float> FPS;
        static ageHook::Type<float> Seconds;
        static ageHook::Type<float> ActualSeconds;
        static ageHook::Type<float> ElapsedTime;
        static ageHook::Type<unsigned int> FrameCount;
        static ageHook::Type<Timer> Time;

        AGE_API static void Reset(void) {
            ageHook::StaticThunk<0x4C6300>::Call<void>();
        };

        AGE_API static void Update(void) {
            ageHook::StaticThunk<0x4C6340>::Call<void>();
        };

        //lua
        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<datTimeManager>("datTimeManager")
                .addStaticProperty("Seconds", [] {return Seconds.get(); })
                .addStaticProperty("InvSeconds", [] {return InvSeconds.get(); })
                .addStaticProperty("FPS", [] {return FPS.get(); })
                .addStaticProperty("ActualSeconds", [] {return ActualSeconds.get(); })
                .addStaticProperty("FrameCount", [] (){return FrameCount.get(); })
                .addStaticProperty("ElapsedTime", []() {return ElapsedTime.get(); })
                .endClass();
        }
    };

    // Lua initialization

}