#pragma once
#include <modules\input.h>

namespace MM2
{
    // Forward declarations
    class ioEventQueue;

    // External declarations


    // Class definitions

    struct ioEvent {
        enum ioEventType
        {
            IO_EVENT_DESTROY = 0,
            IO_EVENT_CREATE = 1,
            IO_EVENT_LBUTTONUP = 2,
            IO_EVENT_RBUTTONDOWN = 3,
            IO_EVENT_RBUTTONUP = 4,
            IO_EVENT_MBUTTONDOWN = 5,
            IO_EVENT_MBUTTONUP = 6,
            IO_EVENT_KEYDOWN = 7,
            IO_EVENT_CHAR = 8,
            IO_EVENT_INPUT = 9,
            IO_EVENT_COMMAND = 10, /* Not handled by default, maybe implement a custom handler? */
        };
    public:
        ioEventType type;
        int x;
        int y;
        int value;
    };

    class ioEventQueue {
    public:
        AGE_API static bool Pop(ioEvent *outEvent)          { return ageHook::StaticThunk<0x4BA930>::Call<bool>(outEvent); };
        AGE_API static bool Peek(ioEvent *outEvent, int *idx)
                                                            { return ageHook::StaticThunk<0x4BA980>::Call<bool>(outEvent, idx); };
        AGE_API static void Queue(ioEvent::ioEventType type, int x, int y, int value)
                                                            { ageHook::StaticThunk<0x4BA9D0>::Call<void>(type, x, y, value); };
        AGE_API static void Command(void *command)          { ageHook::StaticThunk<0x4BAA50>::Call<void>(command); };
    };

    // Lua initialization

}