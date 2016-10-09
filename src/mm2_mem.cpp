#include "mm2_mem.h"
#include "mm2_data.h"

using namespace MM2;

void memSafeHeap::Init(memMemoryAllocator*memAllocator, int heapSize, bool p3, bool p4, bool checkAlloc)
{
    int newHeapSize = 128;

    datArgParser::Get("heapsize", 0, &newHeapSize);

    LogFile::Format("memSafeHeap::Init -- Allocating %dMB heap\n", newHeapSize);

    newHeapSize <<= 20; // Same as *= (1024 * 1024)

    return (this->*$memSafeHeapInit)(memAllocator, newHeapSize, p3, p4, checkAlloc);
};