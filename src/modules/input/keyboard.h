#pragma once
#include <modules\input.h>

namespace MM2
{
    // Forward declarations


    // External declarations


    // Class definitions
    class ioKeyboard {
    public:
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