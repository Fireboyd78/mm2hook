#include "mm2_input.h"
using namespace MM2;

/*
    ioMouse
*/
HOOK_DECLARE(0x6A38EC, ioMouse::InvWidth);
HOOK_DECLARE(0x6A38D4, ioMouse::InvHeight);