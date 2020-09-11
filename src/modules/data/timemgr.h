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
        static hook::Type<float> InvSeconds;
        static hook::Type<float> FPS;
        static hook::Type<float> Seconds;
        static hook::Type<float> ActualSeconds;
        static hook::Type<float> ElapsedTime;
        static hook::Type<unsigned int> FrameCount;
        static hook::Type<Timer> Time;

        AGE_API static void Reset(void) {
            hook::StaticThunk<0x4C6300>::Call<void>();
        };

        AGE_API static void Update(void) {
            hook::StaticThunk<0x4C6340>::Call<void>();
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