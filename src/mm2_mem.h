#pragma once

#include <cstdint>
#include "mm2_utils.h"

namespace MM2
{
    struct memMemoryAllocator;
    struct memSafeHeap;

    static MM2FnHook<void>
        $memSafeHeapInit (NULL, NULL, 0x577210);

    struct memMemoryAllocator
    {
        memMemoryAllocator *PreviousAllocator;
        int field_4;
        int MemoryLeft;
        int Alignment;
        int field_10;
        bool field_14;
        bool field_15;
        bool CheckAlloc;
        bool field_17;
        int Array1[32];
        int Array2[16];
        float fieldD8;
    };

    struct memSafeHeap
    {
        memMemoryAllocator *MemMemoryAllocator;
        void* LPAlloc;
        void *lpAlloc;
        std::uint32_t SomeAlignmentThingy;
        std::uint32_t AllocSize;
        bool bool14;
        bool bool15;
        bool CheckAlloc;
        bool byte17;

        void Init(memMemoryAllocator*memAllocator, int heapSize, bool p3, bool p4, bool checkAlloc);
    };
}