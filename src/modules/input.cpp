#include <modules\input.h>
using namespace MM2;

/*
    ioMouse
*/
declfield(ioMouse::InvWidth)        (0x6A38EC);
declfield(ioMouse::InvHeight)       (0x6A38D4);

declfield(ioMouse::m_X)            (0x6A38DC);
declfield(ioMouse::m_Y)            (0x6A38D8);
declfield(ioMouse::m_dX)            (0x6A38CC);
declfield(ioMouse::m_dY)            (0x6A38F0);
declfield(ioMouse::m_dZ)            (0x6A38E8);
declfield(ioMouse::m_LastButtons)   (0x6A38E4);
declfield(ioMouse::m_Buttons)       (0x6A38E0);
declfield(ioMouse::m_WindowButtons) (0x6A38F4);