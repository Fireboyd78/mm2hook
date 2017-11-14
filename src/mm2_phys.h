#pragma once
#include "mm2_common.h"


namespace MM2
{
    // Forward declarations
    class dgPhysEntity {
    public:
    };

    //TODO : find out inheritance / virtual stuff!!!
    class dgBangerInstance {
    public:
        void Draw(int a1) { ageHook::Thunk<0x4415E0>::Call<void>(this, a1); }
    };
}