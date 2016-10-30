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

    static AGEHook<0x544150>::Func<void> $aiPath_UpdatePedestrians;
    static AGEHook<0x54B9C0>::Func<void> $aiPedestrian_Update;

    static AGEHook<0x550EF0>::Func<void> $aiPoliceForce_Reset;
}