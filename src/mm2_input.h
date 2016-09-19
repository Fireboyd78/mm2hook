#pragma once
#include "mm2.h"

namespace MM2
{
    // Forward declarations
    extern class ioEventQueue;

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
    protected:
        static MM2FnHook<bool> $Pop;
        static MM2FnHook<bool> $Peek;
        static MM2FnHook<void> $Queue;
        static MM2FnHook<void> $Command;
    public:
        static bool Pop(ioEvent *outEvent) {
            $Pop(outEvent);
        };

        static bool Peek(ioEvent *outEvent, int *idx) {
            $Peek(outEvent, idx);
        };

        static void Queue(ioEvent::ioEventType type, int x, int y, int value) {
            $Queue(type, x, y, value);
        };

        static void Command(void *command) {
            $Command(command);
        };
    };
}