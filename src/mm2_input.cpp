#include "mm2_input.h"
using namespace MM2;

DECLARE_HOOK(ioEventQueue::$Pop);
DECLARE_HOOK(ioEventQueue::$Peek);
DECLARE_HOOK(ioEventQueue::$Queue);
DECLARE_HOOK(ioEventQueue::$Command);