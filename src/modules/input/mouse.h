#pragma once
#include <modules\input.h>

namespace MM2
{
    // Forward declarations
    class ioMouse;

    // External declarations


    // Class definitions

    class ioMouse {
    public:
        static hook::Type<float> InvWidth;
        static hook::Type<float> InvHeight;

        static hook::Type<int> m_X;
        static hook::Type<int> m_Y;
        static hook::Type<int> m_dX;
        static hook::Type<int> m_dY;
        static hook::Type<int> m_dZ;
        static hook::Type<int> m_LastButtons;
        static hook::Type<int> m_Buttons;
        static hook::Type<int> m_WindowButtons;
    public:
        AGE_API static void Update() {
            hook::StaticThunk<0x4BB3A0>::Call<void>();
        }

        static bool GetButtonUp(int button) {
            int flag = 1 << button;
            
            bool last = (m_LastButtons.get() & flag) != 0;
            bool current = (m_Buttons.get() & flag) != 0;
            return last && !current;
        }

        static bool GetButtonDown(int button) {
            int flag = 1 << button;

            bool last = (m_LastButtons.get() & flag) != 0;
            bool current = (m_Buttons.get() & flag) != 0;
            return current && !last;
        }

        static bool GetButton(int button) {
            int flag = 1 << button;

            bool current = (m_Buttons.get() & flag) != 0;
            return current;
        }


        static void ClearStates() {
            m_dX.set(0);
            m_dY.set(0);
            m_dZ.set(0);
            m_LastButtons.set(0);
            m_Buttons.set(0);
            m_WindowButtons.set(0);
        }

        //lua
        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<ioMouse>("ioMouse")
                .addStaticFunction("GetButtonUp", &GetButtonUp)
                .addStaticFunction("GetButtonDown", &GetButtonDown)
                .addStaticFunction("GetButton", &GetButton)
                .addStaticProperty("x", [] { return m_X.get(); })
                .addStaticProperty("y", [] { return m_Y.get(); })   
                .endClass();
        }
    };

    // Lua initialization

}