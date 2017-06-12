/*
Depending on where we want our symbols to be defined,
this allows us to reuse the references as many times we want.

This will only work for hooks in a namespace, NOT a class!

Once a .cpp file declares this, hook declarations will be created.

Multiple declarations will cause compiler errors!
*/
#ifdef MM2_API_EXPORT
# define HOOK_API
# define HOOK_EXPORT(addr, type, name)  HOOK_API type name (addr)
#else
# define HOOK_API extern
# define HOOK_EXPORT(addr, type, name) HOOK_API type name
#endif
#pragma once

//
// MM2 uses DirectX 7
//
#define DIRECTX_VERSION 0x0700

#pragma warning(disable:4483)   // disable warning/error about __identifier(<string>)

#include "common.h"

#include "AGE.h"

#define HOOK_SET(addr, type, name) type name (addr)

template <typename TType>
using _Type = ageHook::Type<TType>;

template <typename ...TArgs>
using _Func = ageHook::Func<TArgs...>;

template <typename ...TArgs>
using _MemberFunc = ageHook::MemberFunc<TArgs...>;

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

    HOOK_EXPORT(0x402180, _Func<void>, $CreateGameMutex);

    HOOK_EXPORT(0x4C9510, _Func<void>, $DefaultPrintString);
    HOOK_EXPORT(0x4C95F0, _Func<void>, $DefaultPrinter);

    HOOK_EXPORT(0x5346B0, _Func<int>::StdCall, $MyLoadStringA);

    HOOK_EXPORT(0x5CECF0, _Type<void(*)(LPCSTR)>, StringPrinter);
    HOOK_EXPORT(0x5CED24, _Type<void(*)(int, LPCSTR, va_list)>, Printer);

    HOOK_EXPORT(0x6A3D38, _Type<void(*)(void)>, FatalErrorHandler);

    HOOK_EXPORT(0x5E0CC4, _Type<void(*)(void)>, __VtResumeSampling);
    HOOK_EXPORT(0x5E0CD8, _Type<void(*)(void)>, __VtPauseSampling);

    HOOK_EXPORT(0x6B1708, _Type<BOOL>, gameClosing);

    // think this is actually some sort of game state
    // not sure how exactly it's supposed to work
    HOOK_EXPORT(0x6B17C8, _Type<int>, gameState); // -1 = ???, 0 = main menu, 1 = race

    HOOK_EXPORT(0x6A3AA8, _Type<int>, joyDebug);
    HOOK_EXPORT(0x6A3C0C, _Type<int>, assetDebug);
    HOOK_EXPORT(0x683104, _Type<int>, gfxDebug);
    HOOK_EXPORT(0x6B4C24, _Type<int>, audDebug);
};