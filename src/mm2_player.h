#pragma once
#include "mm2_common.h"

namespace MM2
{
    // Forward declarations
    class mmInfoBase;
    class mmPlayerData;

    // External declarations


    // Class definitions
    class mmInfoBase {
    private:
        void* vtable;
        char IOPath[128];
        int Type;
    };
    ASSERT_SIZEOF(mmInfoBase, 0x88);

    class mmPlayerData : public mmInfoBase {
    public:
        AGE_API char* GetName()                                  { return hook::Thunk<0x527CC0>::Call<char*>(this); }
    };

    declhook(0x6B19F8, _Type<mmPlayerData>, MMCURRPLAYER);
}