#pragma once
#include "mm2_common.h"

namespace MM2
{
    // Forward declarations
    class aiMap;

    namespace $
    {
        HOOK_EXPORT(0x6B2E10, _Type<MM2::aiMap>, AIMAP);

        namespace aiMap
        {
            HOOK_EXPORT(0x538840, _MemberFunc<void>, Dump);
        }
    }

    class aiMap {
    public:
        AGE_API void Dump(void) {
            $::aiMap::Dump(this);
        };

        static aiMap* Instance(void) {
            return &($::AIMAP);
        };
    };
}