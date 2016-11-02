#pragma once
#include "mm2_common.h"

namespace MM2
{
    // Forward declarations
    extern class aiMap;

    namespace $
    {
        namespace aiMap
        {
            HOOK_API AGEHook<0x538840>::MemberFunc<void> Dump;
        }

        HOOK_API AGEHook<0x6B2E10>::Type<MM2::aiMap> AIMAP;
    }

    class aiMap {
    public:
        AGE_API void Dump(void) {
            $::aiMap::Dump(this);
        };

        static aiMap* Instance(void) {
            return &$::AIMAP;
        };
    };

    HOOK_API AGEHook<0x544150>::MemberFunc<void> $aiPath_UpdatePedestrians;
    HOOK_API AGEHook<0x54B9C0>::MemberFunc<void> $aiPedestrian_Update;

    HOOK_API AGEHook<0x550EF0>::MemberFunc<void> $aiPoliceForce_Reset;
}