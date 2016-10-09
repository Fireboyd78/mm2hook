#pragma once
#include "common.h"
#include "hook.h"

#include "AGE.h"

#include "mm2_utils.h"
#include "mm2_vector.h"

namespace MM2 {
    extern void Printf(LPCSTR str, ...);
    extern void Messagef(LPCSTR str, ...);
    extern void Displayf(LPCSTR str, ...);
    extern void Warningf(LPCSTR str, ...);
    extern void Errorf(LPCSTR str, ...);
    extern void Quitf(LPCSTR str, ...);
    extern void Abortf(LPCSTR str, ...);

    extern char * AngelReadString(UINT stringId);

    extern MM2PtrHook<char[40]> szCityName;
    extern MM2PtrHook<char[40]> szCityName2;
};