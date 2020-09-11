#pragma once
#include <modules\input.h>

namespace MM2
{
    // Forward declarations


    // External declarations


    // Class definitions
    class ioKeyboard {
    public:
        AGE_API static void Update() {
            hook::StaticThunk<0x4BABA0>::Call<void>();
        }

        static void ClearStates() {
            for (int i = 0; i < 256; i++) {
                SetKeyState(i, 0);
            }
        }

        //todo: don't use these evil magic numbers
        static void SetKeyState(byte keycode, byte state) {
            int activeFlag = *(int*)0x6A387C; //ioKeyboard::m_Active
            byte* keyStatesPtr = (byte*)0x6A3674; //m_Keys
            keyStatesPtr[(256 * activeFlag) + keycode] = state;
        }

        static byte GetKeyState(byte keycode) {
            int activeFlag = *(int*)0x6A387C; //ioKeyboard::m_Active
            byte* keyStatesPtr = (byte*)0x6A3674; //m_Keys
            return keyStatesPtr[(256 * activeFlag) + keycode];
        }

        //lua
        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<ioKeyboard>("ioKeyboard")
                .addStaticFunction("GetKeyState", &GetKeyState)
                .endClass();
        }
    };

    // Lua initialization

}