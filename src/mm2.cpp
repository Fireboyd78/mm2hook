#define AGE_API_EXPORT
#define MM2_API_EXPORT

#include "mm2.h"

using namespace MM2;

#define DEFINE_PRINT_HOOK(x) \
NAKED void x(LPCSTR str, ...) { \
    __asm jmp dword ptr ds:$##x \
}

HOOK_EXPORT(0x4C9720, _Func<void>, $Printf);
HOOK_EXPORT(0x4C9750, _Func<void>, $Messagef);
HOOK_EXPORT(0x4C9780, _Func<void>, $Displayf);
HOOK_EXPORT(0x4C97B0, _Func<void>, $Warningf);
HOOK_EXPORT(0x4C97E0, _Func<void>, $Errorf);
HOOK_EXPORT(0x4C9810, _Func<void>, $Quitf);
HOOK_EXPORT(0x4C9850, _Func<void>, $Abortf);

HOOK_EXPORT(0x534790, _Func<char *>, $AngelReadString);

namespace MM2
{
    DEFINE_PRINT_HOOK(Printf);
    DEFINE_PRINT_HOOK(Messagef);
    DEFINE_PRINT_HOOK(Displayf);
    DEFINE_PRINT_HOOK(Warningf);
    DEFINE_PRINT_HOOK(Errorf);
    DEFINE_PRINT_HOOK(Quitf);
    DEFINE_PRINT_HOOK(Abortf);

    char * AngelReadString(UINT stringId) {
        return $AngelReadString(stringId);
    }
}