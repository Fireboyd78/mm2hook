#include "mm2.h"

using namespace MM2;

#define MAX_HOOK_PTRS 4096

#define DEFINE_PRINT_HOOK(x) \
NAKED void x(LPCSTR str, ...) { \
    __asm jmp dword ptr ds:$##x \
}

MM2FnHook<void>     $Printf                 ( NULL, NULL, 0x4C9720 );
MM2FnHook<void>     $Messagef               ( NULL, NULL, 0x4C9750 );
MM2FnHook<void>     $Displayf               ( NULL, NULL, 0x4C9780 );
MM2FnHook<void>     $Warningf               ( NULL, NULL, 0x4C97B0 );
MM2FnHook<void>     $Errorf                 ( NULL, NULL, 0x4C97E0 );
MM2FnHook<void>     $Quitf                  ( NULL, NULL, 0x4C9810 );
MM2FnHook<void>     $Abortf                 ( NULL, NULL, 0x4C9850 );

MM2FnHook<char *>   $AngelReadString        ( NULL, NULL, 0x534790 );

MM2PtrHook<HWND> 
            CMidtownMadness2::$hwndMain     ( 0x64993C, 0x681B00, 0x6830B8 );

namespace MM2
{
    MM2PtrHook<char[40]>    szCityName      ( NULL, NULL, 0x6B167C );
    MM2PtrHook<char[40]>        szCityName2     ( NULL, NULL, 0x6B16A4 );

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