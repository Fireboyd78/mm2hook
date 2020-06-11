#pragma once
#include "mm2_common.h"
#include "mm2_game.h"

namespace MM2
{
    // Forward declarations
    class mmReplayManager;

    // External declarations
    extern class asNode;

    // Class definitions
    class mmReplayManager : public asNode {
    private:
        byte _buffer[0x828];
    public:
        ANGEL_ALLOCATOR

            static ageHook::Type<mmReplayManager *> Instance;

        AGE_API mmReplayManager(void) {
            scoped_vtable x(this);
            ageHook::Thunk<0x406870>::Call<void>(this);
        }

        virtual AGE_API ~mmReplayManager(void) {
            scoped_vtable x(this);
            ageHook::Thunk<0x406930>::Call<void>(this);
        }

        AGE_API float GetBrakes()                                      { return ageHook::Thunk<0x4072D0>::Call<float>(this); }
        AGE_API float GetHandBrakes()                                  { return ageHook::Thunk<0x407300>::Call<float>(this); }
        AGE_API float GetSteering()                                    { return ageHook::Thunk<0x407330>::Call<float>(this); }
        AGE_API float GetThrottle()                                    { return ageHook::Thunk<0x407360>::Call<float>(this); }

        /*
            asNode virtuals
        */

        AGE_API void Reset() override                                  { ageHook::Thunk<0x406990>::Call<void>(this); }
        AGE_API void Update() override                                 { ageHook::Thunk<0x4069E0>::Call<void>(this); }
        AGE_API void Cull() override                                   { ageHook::Thunk<0x406DF0>::Call<void>(this); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<mmReplayManager, asNode>("mmReplayManager")
                .addFunction("GetBrakes", &GetBrakes)
                .addFunction("GetHandBrakes", &GetHandBrakes)
                .addFunction("GetSteering", &GetSteering)
                .addFunction("GetThrottle", &GetThrottle)
            .endClass();
        }
    };
    ASSERT_SIZEOF(mmReplayManager, 0x840);

    // Lua initialization

}