#pragma once

//
// MM2 uses DirectX 7
//
#define DIRECTX_VERSION 0x0700

#include "common.h"

#include "AGE.h"

#include "mm2_utils.h"
#include "mm2_vector.h"

namespace MM2 {
    struct LocString {
        char buffer[512];
    };

    extern void Printf(LPCSTR str, ...);
    extern void Messagef(LPCSTR str, ...);
    extern void Displayf(LPCSTR str, ...);
    extern void Warningf(LPCSTR str, ...);
    extern void Errorf(LPCSTR str, ...);
    extern void Quitf(LPCSTR str, ...);
    extern void Abortf(LPCSTR str, ...);

    extern char * AngelReadString(UINT stringId);

    static AGEHook<0x402180>::Func<void> $CreateGameMutex;

    static AGEHook<0x4C9510>::Func<void> $DefaultPrintString;
    static AGEHook<0x4C95F0>::Func<void> $DefaultPrinter;

    static AGEHook<0x5346B0>::Func<int>::StdCall $MyLoadStringA;
    
    static AGEHook<0x5CECF0>::Type<void(*)(LPCSTR)> $PrintString;
    static AGEHook<0x5CED24>::Type<void(*)(int, LPCSTR, va_list)> $Printer;
    static AGEHook<0x6A3D38>::Type<void(*)(void)> $FatalErrorHandler;

    static AGEHook<0x5E0CC4>::Type<void(*)(void)> $__VtResumeSampling;
    static AGEHook<0x5E0CD8>::Type<void(*)(void)> $__VtPauseSampling;

    static AGEHook<0x661738>::Type<asNode> ROOT;

    static AGEHook<0x6B1708>::Type<BOOL> $gameClosing;

    // think this is actually some sort of game state
    // not sure how exactly it's supposed to work
    static AGEHook<0x6B17C8>::Type<int> splashScreen; // -1 = ???, 0 = main menu, 1 = race

    static AGEHook<0x6A3AA8>::Type<int> joyDebug;
    static AGEHook<0x6A3C0C>::Type<int> assetDebug;
    static AGEHook<0x683104>::Type<int> gfxDebug;
    static AGEHook<0x6B4C24>::Type<int> audDebug;
};