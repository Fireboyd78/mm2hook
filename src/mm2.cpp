// Gives "extern" hooks their definitions :)
// (See "mm2_common.h" for more details)
#define __MM2_DECLARE_HOOKS

#include "mm2.h"
using namespace MM2;

#define DEFINE_PRINT_HOOK(x) \
NAKED void x(LPCSTR str, ...) { \
    __asm jmp dword ptr ds:$##x \
}

AGEHook<0x4C9720>::Func<void> $Printf;
AGEHook<0x4C9750>::Func<void> $Messagef;
AGEHook<0x4C9780>::Func<void> $Displayf;
AGEHook<0x4C97B0>::Func<void> $Warningf;
AGEHook<0x4C97E0>::Func<void> $Errorf;
AGEHook<0x4C9810>::Func<void> $Quitf;
AGEHook<0x4C9850>::Func<void> $Abortf;

AGEHook<0x534790>::Func<char *> $AngelReadString;

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