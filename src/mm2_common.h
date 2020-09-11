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

#define VIRTUAL_THUNK(addr, rtype, ...) { return hook::Thunk<addr>::Call<rtype> WRAP(this, __VA_ARGS__); }

//
// Allocator
// Calls MM2's internal operator new
//
#define ANGEL_ALLOCATOR void* operator new(size_t size) { return hook::StaticThunk<0x577360>::Call<void*>(size); }

//
// MM2 uses DirectX 7
//
#define DIRECTX_VERSION 0x0700

#include "common.h"
#include "AGE.h"
#include "hook.h"

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
    module_particle,
    module_camera,

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
        AGE_API string(const char *str)                     { hook::Thunk<0x505070>::Call<void>(this, str); }

        AGE_API string(char *str, int len) {
            buffer = str;
            length = len;
        }

        AGE_API void operator+=(char c)                     { hook::Thunk<0x49F4C0>::Call<void>(this, c); }
        AGE_API void operator+=(const char *str)            { hook::Thunk<0x49F3E0>::Call<void>(this, str); }
        AGE_API void operator-=(const char *str)            { hook::Thunk<0x49F6D0>::Call<void>(this, str); }
        AGE_API void operator=(const char *str)             { hook::Thunk<0x4A0A90>::Call<void>(this, str); }

        AGE_API string operator+(const string &str) const   { return hook::Thunk<0x4A0B00>::Call<string>(this, &str); }
        AGE_API string operator+(const char *str) const     { return hook::Thunk<0x49F180>::Call<string>(this, str); }
        AGE_API string operator-(const char *str) const     { return hook::Thunk<0x49F560>::Call<string>(this, str); }

        AGE_API int NumSubStrings(void) const               { return hook::Thunk<0x4A0A20>::Call<int>(this); }
        
        AGE_API string SubString(int index) const           { return hook::Thunk<0x4A0910>::Call<string>(this, index); }

        inline operator char *(void) const {
            return buffer;
        }

        char *buffer;
        int length;
    };

    extern char * AngelReadString(UINT stringId);

    class Timer {
    public:
        static float TicksToSeconds;
        static float TicksToMilliseconds;

        DWORD StartTime;

        AGE_API Timer()                                     { hook::Thunk<0x4C7840>::Call<void>(this); }

        AGE_API void BeginBenchmark()                       { hook::Thunk<0x4C7980>::Call<void>(this); }
        AGE_API void EndBenchmark()                         { hook::Thunk<0x4C79F0>::Call<void>(this); }
        AGE_API uint QuickTicks()                           { return hook::Thunk<0x4C7810>::Call<uint>(this); }
        AGE_API ulong Ticks()                               { return hook::Thunk<0x4C77E0>::Call<ulong>(this); }
    };

    class NetStartArray {
        ulong Slots[10];
    public:
        AGE_API void Clear(void)                            { return hook::Thunk<0x5235C0>::Call<void>(this); }
        AGE_API int GetIndex(ulong playerId)                { return hook::Thunk<0x5235D0>::Call<int>(this, playerId); }
        AGE_API void ClearIndex(ulong playerId)             { return hook::Thunk<0x5235F0>::Call<void>(this, playerId); }
        AGE_API int AssignOpenIndex(ulong playerId)         { return hook::Thunk<0x523610>::Call<int>(this, playerId); }
        AGE_API void Init(ulong *playerIds)                 { return hook::Thunk<0x523650>::Call<void>(this, playerIds); }
    };

    enum dgGameMode {
        Cruise,
        Checkpoint,
        CnR,
        Circuit,
        Blitz,
        CRoam, // unused
        CrashCourse,
    };

    enum dgSkillLevel {
        Amateur,
        Professional,
    };

    /*
        !!! IMPORTANT !!!

        Regarding dgStatePack and mmStatePack:

        There's quite a bit of unused stuff unfortunately.
        Some things are leftover from MM1 and others are just never actually used.

        You should verify what you're doing is actually used by the game.
    */


    template<typename T>
    CppBindClass<T, LuaBinding> beginStatePackLua(LuaState L, const char* name) {
        auto binder = LuaBinding(L).beginClass<T>(name);
        binder
            .addProperty("GameMode", &dgStatePack::getGameMode, &dgStatePack::setGameMode)
            .addVariableRef("RaceId", &dgStatePack::RaceId)

            .addVariableRef("TrafficDensity", &dgStatePack::TrafficDensity)
            .addVariableRef("PedestrianDensity", &dgStatePack::PedestrianDensity)
            .addVariableRef("CopDensity", &dgStatePack::CopDensity)
            .addVariableRef("OpponentDensity", &dgStatePack::OpponentDensity)

            .addVariableRef("MaxAmbientVehicles", &dgStatePack::MaxAmbientVehicles)

            .addProperty("EnableCableCars", &dgStatePack::getEnableCableCars, &dgStatePack::setEnableCableCars)
            .addProperty("EnableSubways", &dgStatePack::getEnableSubways, &dgStatePack::setEnableSubways)

            .addVariableRef("NumLaps", &dgStatePack::NumLaps)

            .addVariableRef("TextureQuality", &dgStatePack::TextureQuality)

            .addVariableRef("TimeOfDay", &dgStatePack::TimeOfDay)
            .addVariableRef("WeatherType", &dgStatePack::WeatherType)
            .addProperty("SkillLevel", &dgStatePack::getSkillLevel, &dgStatePack::setSkillLevel)
            .addVariableRef("AudioFlags", &dgStatePack::AudioFlags)

            .addProperty("EnablePedestrians", &dgStatePack::getEnablePedestrians, &dgStatePack::setEnablePedestrians);
        return binder;
    }

    struct dgStatePack {
        //FUNCS
        AGE_API dgStatePack(void) {
            scoped_vtable x(this);
            hook::Thunk<0x443110>::Call<void>(this);
        }

        AGE_API ~dgStatePack(void) {
            scoped_vtable x(this);
            hook::Thunk<0x443180>::Call<void>(this);
        }

        //FIELDS
        static hook::Type<dgStatePack *> Instance;

        dgGameMode GameMode;

        int RaceId;

        float TrafficDensity;
        float PedestrianDensity;
        float CopDensity;
        float OpponentDensity;

        /* ---- MC1 leftovers ---- */
        float unk_18; // unused (default: 0.5)
        float unk_1C; // unused (default: 0.5)
        float unk_20; // unused (default: 0.5)
        bool unk_24;  // unused (default: 1)

        int NumberOfCTFRacers; // default: 0
        /* ----------------------- */

        BOOL DisableAI;

        int unk_30; // unused
        int MaxAmbientVehicles;
        int MaxAmbientPedestrians; // unused -- assumed name (default: 100)
        int unk_3C; // unused

        BOOL EnableCableCars;
        BOOL EnableSubways;

        int NumLaps;

        int TextureQuality;
        int unk_50; // unused (default: 99)

        int TimeOfDay;
        int WeatherType;
        dgSkillLevel SkillLevel;
        int AudioFlags;

        BOOL EnablePedestrians;
        void *unk_68; // related to hookmen

        //lua helpers
        inline int getSkillLevel(void) {
            return (int)SkillLevel;
        }
        inline void setSkillLevel(int level) {
            SkillLevel = (dgSkillLevel)level;
        }

        inline int getGameMode(void) {
            return (int)GameMode;
        }
        inline void setGameMode(int mode) {
            GameMode = (dgGameMode)mode;
        }

        inline bool getEnableCableCars(void) {
            return !!EnableCableCars;
        }
        inline void setEnableCableCars(bool value) {
            EnableCableCars = (value) ? TRUE : FALSE;
        }

        inline bool getEnableSubways(void) {
            return !!EnableSubways;
        }
        inline void setEnableSubways(bool value) {
            EnableSubways = (value) ? TRUE : FALSE;
        }

        inline bool getEnablePedestrians(void) {
            return !!EnablePedestrians;
        }
        inline void setEnablePedestrians(bool value) {
            EnablePedestrians = (value) ? TRUE : FALSE;
        }

        //LUA
        static void BindLua(LuaState L) {
            beginStatePackLua<dgStatePack>(L, "dgStatePack")
                .endClass();
        }
    };

    class mmStatePack : public dgStatePack {
    private:
        //lua helper for portals
        bool getUsePortals() {
            return this->UsePortals == TRUE;
        }

        void setUsePortals(bool usePortals) {
            this->UsePortals = (usePortals) ? TRUE : FALSE;
        }

        LPCSTR getNetName() {
            return (LPCSTR)NetName;
        }

        void setNetName(LPCSTR name) {
            memcpy(&NetName, name, sizeof(NetName) - 1); //copy all but last char to avoid overflow
        }
    public:
        char CityName[40];
        char CityLocale[40]; // same as city name, referred to as 'locale'?
        char ReplayName[40];

        int VehicleId;

        int InputDevice;
        int unk_EC; // seems heavily tied into multiplayer, but also has singleplayer uses?

        BOOL CopsChaseAI; // cops will chase after opponents
        int unk_F4; // unused

        BOOL Shutdown; // stop the process

        /*
            Player settings
        */

        float Difficulty; // based on the SkillLevel
        int TransmissionType;
        float PhysicsRealism; // unused :(
        BOOL UseForceFeedback;
        BOOL DisableProfile;

        char VehicleName[80];
        int VehiclePaintjob;

        char NetName[80];
        
        /*
            Race settings?
        */

        float TimeLimit;

        int SplashScreen; // -1 = ???, 0 = main menu, 1 = race
        BOOL DisableRegen; // educated guess based on a skipped call to mmPlayer::UpdateRegen if true

        /*
            Audio settings
        */

        float SoundFXVolume;
        float AudioBalance;
        float MusicVolume;
        short NumAudioChannels;
        short unk_1CE[10]; // unused -- not sure what this might've been
        char AudioDeviceName[200];
        bool HasMusicCD; // allow use of music player if true

        /*
            Cops 'n Robbers settings
        */

        int CnRMode;
        int CnRLimitType;
        int CnRTeam;
        float CnRTimeLimit;
        int CnRPointLimit;
        int CnRGoldMass;

        /*
            Unknown settings (unused)
        */

        int unk_2C4; // default: 0
        int unk_2C8; // default: 0
        char unk_2CC[160];

        /*
            View settings (NAMES NEED CONFIRMATION!)
        */

        bool unk_36C;
        char MapMode;
        bool UseWideFOV;
        bool ShowDash;

        bool ShowMirror;
        bool unk_371;
        bool unk_372;
        bool ShowIcons;

        bool unk_374;
        bool unk_375;
        bool HudOrient;
        bool HudZoomIn;

        /*
            Graphics settings
        */

        BOOL BestTextureFilter; // MM1 - unused
        BOOL InterlacedRendering; // MM1 - unused
        float FarClip;
        BOOL EnableSky;
        float LightingQuality;
        BOOL UsePortals;
        int ObjectDetail;
        BOOL EnableReflections; // vehicle reflections
        int CloudShadowQuality;

        /*
            Cheat settings?
        */

        BOOL IsCheating; // true if player entered a cheatcode
        float TimeLimitOverride; // overrides TimeLimit when > 0
        BOOL UnlockRewards; // only works on menu loading

        /*
            Multiplayer settings
        */

        bool AllowCheaters; // don't check tuning CRC
        bool ShowMultiplayerResults; // display results at end of multiplayer
        bool IsHost; // unconfirmed
        bool UseDialup;

        /*
            Global settings?
        */

        BOOL UseIME;
        HIMC IMEContext;

        bool InCrashCourse;

        NetStartArray NetStartArray;

        //LUA
        static void BindLua(LuaState L) {
            beginStatePackLua<mmStatePack>(L, "mmStatePack")

            //mmStatePack specifics
            .addStaticProperty("Instance", [] { return (mmStatePack*)0x6B1610; }) //HACK but it should work

            .addProperty("NetName", &getNetName, &setNetName)
            .addVariableRef("InCrashCourse", &mmStatePack::InCrashCourse)
            .addVariableRef("FarClip", &mmStatePack::FarClip)
            .addVariableRef("TimeLimitOverride", &mmStatePack::TimeLimitOverride)
            .addProperty("UsePortals", &getUsePortals, &setUsePortals)
            .endClass();
        }
    };

    /*
        Make sure all of our ducks are in a row --
        The size of this stuff is EXTREMELY important!
    */
    ASSERT_SIZEOF(NetStartArray, 0x28);
    ASSERT_SIZEOF(dgGameMode, 4);
    ASSERT_SIZEOF(dgSkillLevel, 4);
    ASSERT_SIZEOF(dgStatePack, 0x6C);
    ASSERT_SIZEOF(mmStatePack, 0x3E0);

    declhook(0x402180, _Func<void>, $CreateGameMutex);

    declhook(0x4C9510, _Func<void>, $DefaultPrintString);
    declhook(0x4C95F0, _Func<void>, $DefaultPrinter);

    declhook(0x5346B0, _Func<int>::StdCall, $MyLoadStringA);

    declhook(0x5CED24, _Type<void(*)(int, LPCSTR, va_list)>, Printer);

    declhook(0x5E0CC4, _Type<void(*)(void)>, __VtResumeSampling);
    declhook(0x5E0CD8, _Type<void(*)(void)>, __VtPauseSampling);

    declhook(0x6B1610, _TypeProxy<mmStatePack>, MMSTATE);

    declhook(0x6A3AA8, _Type<int>, joyDebug);
    declhook(0x6A3C0C, _Type<int>, assetDebug);
    declhook(0x683104, _Type<int>, gfxDebug);
    declhook(0x6B4C24, _Type<int>, audDebug);

    declhook(0x6A3AC0, _Type<int>, gRandSeed);

    template<>
    void luaAddModule<module_common>(LuaState L) {
        typedef void(__cdecl *printer_type)(LPCSTR);

        luaBind<dgStatePack>(L);
        luaBind<mmStatePack>(L);

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