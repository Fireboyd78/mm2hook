#pragma once
#include "mm2_common.h"

namespace MM2
{
    // Forward declarations
    extern class ioEventQueue;
    extern class ioMouse;

    namespace $
    {
        namespace ioEventQueue
        {
            HOOK_EXPORT(0x4BA930, _Func<bool>, Pop);
            HOOK_EXPORT(0x4BA980, _Func<bool>, Peek);
            HOOK_EXPORT(0x4BA9D0, _Func<void>, Queue);
            HOOK_EXPORT(0x4BAA50, _Func<void>, Command);
        }
        namespace ioInput
        {
            HOOK_EXPORT(0x4BA910, _Func<void>, Update);
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

    class ioMouse {
    public:
        static ageHook::Type<float> InvWidth;
        static ageHook::Type<float> InvHeight;
    };
}