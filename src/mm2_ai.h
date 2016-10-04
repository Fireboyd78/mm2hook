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
        AGE_API void Dump(void) {
            $Dump(this);
        };

        static aiMap* Instance(void) {
            return $AIMAP;
        };
    };
}