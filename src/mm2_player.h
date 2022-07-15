#pragma once
#include "mm2_common.h"

namespace MM2
{
    // Forward declarations
    class mmInfoBase;
    class mmPlayerData;

    // External declarations
    extern class datParser;

    // Class definitions
    class mmInfoBase {
    private:
        char IOPath[128];
        int Type;
    public:
        AGE_API mmInfoBase()                                     { hook::Thunk<0x528230>::Call<void>(this); }
        AGE_API ~mmInfoBase()                                    { hook::Thunk<0x41AB90>::Call<void>(this); }

        AGE_API int Load(char* a1)                               { return hook::Thunk<0x528260>::Call<int>(this, a1); }
        AGE_API int Save(char* a1)                               { return hook::Thunk<0x528310>::Call<int>(this, a1); }
        AGE_API void SetIOPath(char* a1)                         { hook::Thunk<0x5283C0>::Call<void>(this, a1); }

        virtual AGE_API void FileIO(datParser& parser)           { hook::Thunk<0x41AB90>::Call<void>(this, &parser); }
    };
    ASSERT_SIZEOF(mmInfoBase, 0x88);

    class mmPlayerData : public mmInfoBase {
    public:
        AGE_API char* GetName()                                  { return hook::Thunk<0x527CC0>::Call<char*>(this); }
    };

    declhook(0x6B19F8, _Type<mmPlayerData>, MMCURRPLAYER);
}