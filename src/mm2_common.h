/*
    Depending on where we want our symbols to be defined,
    this allows us to reuse the references as many times we want.

    This will only work for hooks in a namespace, NOT a class!
    
    Once a .cpp file declares this, hook declarations will be created.

    Multiple declarations will cause compiler errors!
*/
#ifdef __MM2_DECLARE_HOOKS
#define HOOK_API
#else
#define HOOK_API extern
#endif

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

    HOOK_API AGEHook<0x402180>::Func<void> $CreateGameMutex;

    HOOK_API AGEHook<0x4C9510>::Func<void> $DefaultPrintString;
    HOOK_API AGEHook<0x4C95F0>::Func<void> $DefaultPrinter;

    HOOK_API AGEHook<0x5346B0>::Func<int>::StdCall $MyLoadStringA;
    
    HOOK_API AGEHook<0x5CECF0>::Type<void(*)(LPCSTR)> StringPrinter;
    HOOK_API AGEHook<0x5CED24>::Type<void(*)(int, LPCSTR, va_list)> Printer;
    HOOK_API AGEHook<0x6A3D38>::Type<void(*)(void)> FatalErrorHandler;

    HOOK_API AGEHook<0x5E0CC4>::Type<void(*)(void)> __VtResumeSampling;
    HOOK_API AGEHook<0x5E0CD8>::Type<void(*)(void)> __VtPauseSampling;

    HOOK_API AGEHook<0x6B1708>::Type<BOOL> gameClosing;

    // think this is actually some sort of game state
    // not sure how exactly it's supposed to work
    HOOK_API AGEHook<0x6B17C8>::Type<int> gameState; // -1 = ???, 0 = main menu, 1 = race

    HOOK_API AGEHook<0x6A3AA8>::Type<int> joyDebug;
    HOOK_API AGEHook<0x6A3C0C>::Type<int> assetDebug;
    HOOK_API AGEHook<0x683104>::Type<int> gfxDebug;
    HOOK_API AGEHook<0x6B4C24>::Type<int> audDebug;
};