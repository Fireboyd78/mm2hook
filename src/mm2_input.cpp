#include "mm2_input.h"
using namespace MM2;

/*
    ioMouse
*/
DECLARE_HOOK(ioMouse::InvWidth)(0x6A38EC);
DECLARE_HOOK(ioMouse::InvHeight)(0x6A38D4);