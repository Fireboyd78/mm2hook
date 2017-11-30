/*
Depending on where we want our symbols to be defined,
this allows us to reuse the references as many times we want.

This will only work for hooks in a namespace, NOT a class!

Once a .cpp file declares this, hook declarations will be created.

Multiple declarations will cause compiler errors!
*/
#ifdef MM2_API_EXPORT
# define HOOK_API
# define declhook(addr, type, name) HOOK_API type name (addr)
#else
# define HOOK_API extern
# define declhook(addr, type, name) HOOK_API type name
#endif
#pragma once

#define declfield(t) decltype(t) t

//
// MM2 uses DirectX 7
//
#define DIRECTX_VERSION 0x0700

#include "common.h"
#include "AGE.h"

template <typename TType>
using _Type = ageHook::Type<TType>;

template <typename TType>
using _TypeProxy = ageHook::TypeProxy<TType>;

template <typename ...TArgs>
using _Func = ageHook::Func<TArgs...>;

template <typename ...TArgs>
using _MemberFunc = ageHook::MemberFunc<TArgs...>;

template <int address>
using _Thunk = ageHook::Thunk<address>;

template <int address>
using _StaticThunk = ageHook::StaticThunk<address>;

#include "mm2_utils.h"
#include "mm2_vector.h"

enum mm2_module {
    module_ai,
    module_audio,
    module_base,
    module_bound,
    module_breakable,
    module_city,
    module_common,
    module_creature,
    module_data,
    module_game,
    module_gfx,
    module_input,
    module_inst,
    module_level,
    module_model,
    module_network,
    module_phys,
    module_rgl,
    module_stream,
    module_ui,
    module_vehicle,

    module_count, // number of Lua modules
};

template<class TNode>
using luaClassBinder = CppBindClass<TNode, LuaBinding>;

namespace MM2 {
    template <class T>
    static inline void luaBind(LuaState L) {
        T::BindLua(L);
    }

    template <class T>
    static inline void luaBind(LuaState L, LPCSTR name) {
        luaClassBinder<T> lc = LuaBinding(L).beginClass<T>(name);
        T::BindLua(&lc);
        lc.endClass();
    }

    template <enum mm2_module>
    static inline void luaAddModule(LuaState L) { /* do nothing */ }

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

    class string {
    public:
        AGE_API string(const char *str)                     { ageHook::Thunk<0x505070>::Call<void>(this, str); }

        AGE_API string(char *str, int len) {
            buffer = str;
            length = len;
        }

        AGE_API void operator+=(char c)                     { ageHook::Thunk<0x49F4C0>::Call<void>(this, c); }
        AGE_API void operator+=(const char *str)            { ageHook::Thunk<0x49F3E0>::Call<void>(this, str); }
        AGE_API void operator-=(const char *str)            { ageHook::Thunk<0x49F6D0>::Call<void>(this, str); }
        AGE_API void operator=(const char *str)             { ageHook::Thunk<0x4A0A90>::Call<void>(this, str); }

        AGE_API string operator+(const string &str) const   { return ageHook::Thunk<0x4A0B00>::Call<string>(this, &str); }
        AGE_API string operator+(const char *str) const     { return ageHook::Thunk<0x49F180>::Call<string>(this, str); }
        AGE_API string operator-(const char *str) const     { return ageHook::Thunk<0x49F560>::Call<string>(this, str); }

        AGE_API int NumSubStrings(void) const               { return ageHook::Thunk<0x4A0A20>::Call<int>(this); }
        
        AGE_API string SubString(int index) const           { return ageHook::Thunk<0x4A0910>::Call<string>(this, index); }

        inline operator char *(void) const {
            return buffer;
        }

        char *buffer;
        int length;
    };

    extern char * AngelReadString(UINT stringId);

    struct Timer {
        DWORD StartTime;
    };

    declhook(0x402180, _Func<void>, $CreateGameMutex);

    declhook(0x4C9510, _Func<void>, $DefaultPrintString);
    declhook(0x4C95F0, _Func<void>, $DefaultPrinter);

    declhook(0x5346B0, _Func<int>::StdCall, $MyLoadStringA);

    declhook(0x5CECF0, _Type<void(*)(LPCSTR)>, StringPrinter);
    declhook(0x5CED24, _Type<void(*)(int, LPCSTR, va_list)>, Printer);

    declhook(0x6A3D38, _Type<void(*)(void)>, FatalErrorHandler);

    declhook(0x5E0CC4, _Type<void(*)(void)>, __VtResumeSampling);
    declhook(0x5E0CD8, _Type<void(*)(void)>, __VtPauseSampling);

    declhook(0x6B1708, _Type<BOOL>, gameClosing);

    // think this is actually some sort of game state
    // not sure how exactly it's supposed to work
    declhook(0x6B17C8, _Type<int>, gameState); // -1 = ???, 0 = main menu, 1 = race

    declhook(0x6B1614, _Type<int>, raceId);

    declhook(0x6B167C, _Type<char[40]>, cityName);
    declhook(0x6B16A4, _Type<char[40]>, cityName2);

    declhook(0x6B048C, _Type<char[32]>, vehicleName);

    declhook(0x6B1610, _Type<int>, gameMode); //0 = Cruise, 1 = Checkpoint, 2 = Cops N' Robbers, 3 = Circuit, 4 = Blitz, 6 = Crash Course

    declhook(0x6B19B4, _Type<bool>, unlockRewards);

    declhook(0x6A3AA8, _Type<int>, joyDebug);
    declhook(0x6A3C0C, _Type<int>, assetDebug);
    declhook(0x683104, _Type<int>, gfxDebug);
    declhook(0x6B4C24, _Type<int>, audDebug);

    template<>
    void luaAddModule<module_common>(LuaState L) {
        typedef void(__cdecl *printer_type)(LPCSTR);

        LuaBinding(L)
            .addFunction("Printf", (printer_type)&Printf)
            .addFunction("Messagef", (printer_type)&Messagef)
            .addFunction("Displayf", (printer_type)&Displayf)
            .addFunction("Warningf", (printer_type)&Warningf)
            .addFunction("Errorf", (printer_type)&Errorf)
            .addFunction("Quitf", (printer_type)&Quitf)
            .addFunction("Abortf", (printer_type)&Abortf)

            .addFunction("AngelReadString", &AngelReadString);
    }
};