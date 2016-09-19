#include "mm2_input.h"
using namespace MM2;

MM2FnHook<bool> ioEventQueue::$Pop      ( NULL, NULL, 0x4BA930 );
MM2FnHook<bool> ioEventQueue::$Peek     ( NULL, NULL, 0x4BA980 );
MM2FnHook<void> ioEventQueue::$Queue    ( NULL, NULL, 0x4BA9D0 );
MM2FnHook<void> ioEventQueue::$Command  ( NULL, NULL, 0x4BAA50 );