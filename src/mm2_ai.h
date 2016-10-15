#pragma once
#include "mm2_common.h"

namespace MM2
{
    // Forward declarations
    extern class aiMap;

    class aiMap {
    protected:
        static AGEHook<0x538840>::Func<void> $Dump;

        static AGEHook<0x6B2E10>::Type<aiMap> $AIMAP;
    public:
        AGE_API void Dump(void) {
            $Dump(this);
        };

        static aiMap* Instance(void) {
            return &$AIMAP;
        };
    };
}