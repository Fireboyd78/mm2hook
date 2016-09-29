#pragma once
#include "mm2_common.h"

namespace MM2
{
    // Forward declarations
    extern class aiMap;

    class aiMap {
    protected:
        static MM2FnHook<void> $Dump;

        static MM2PtrHook<aiMap> $AIMAP;
    public:
        static void Dump(void) {
            const aiMap* This = $AIMAP;
            $Dump(&*This);
        };
    };
}