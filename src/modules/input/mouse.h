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
        static ageHook::Type<float> InvWidth;
        static ageHook::Type<float> InvHeight;

        static ageHook::Type<int> m_dX;
        static ageHook::Type<int> m_dY;
        static ageHook::Type<int> m_dZ;
        static ageHook::Type<int> m_LastButtons;
        static ageHook::Type<int> m_Buttons;
        static ageHook::Type<int> m_WindowButtons;
    public:
        AGE_API static void Update() {
            ageHook::StaticThunk<0x4BB3A0>::Call<void>();
        }

        static void ClearStates() {
            m_dX.set(0);
            m_dY.set(0);
            m_dZ.set(0);
            m_LastButtons.set(0);
            m_Buttons.set(0);
            m_WindowButtons.set(0);
        }
    };

    // Lua initialization

}