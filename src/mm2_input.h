#pragma once
#include "mm2_common.h"

namespace MM2
{
    // Forward declarations
    extern class ioEventQueue;

    namespace $
    {
        namespace ioEventQueue
        {
            HOOK_API AGEHook<0x4BA930>::Func<bool> Pop;
            HOOK_API AGEHook<0x4BA980>::Func<bool> Peek;
            HOOK_API AGEHook<0x4BA9D0>::Func<void> Queue;
            HOOK_API AGEHook<0x4BAA50>::Func<void> Command;
        }
    }

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
        AGE_API static bool Pop(ioEvent *outEvent) {
            return $::ioEventQueue::Pop(outEvent);
        };

        AGE_API static bool Peek(ioEvent *outEvent, int *idx) {
            return $::ioEventQueue::Peek(outEvent, idx);
        };

        AGE_API static void Queue(ioEvent::ioEventType type, int x, int y, int value) {
            $::ioEventQueue::Queue(type, x, y, value);
        };

        AGE_API static void Command(void *command) {
            $::ioEventQueue::Command(command);
        };
    };

    HOOK_API AGEHook<0x4BA910>::Func<void> $ioInput_Update;

    HOOK_API AGEHook<0x6A38EC>::Type<float> ioMouse_InvWidth;
    HOOK_API AGEHook<0x6A38D4>::Type<float> ioMouse_InvHeight;
}