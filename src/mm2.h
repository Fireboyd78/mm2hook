#pragma once
#include "common.h"
#include "hook.h"

#include "AGE.h"

#include "stream.h"

enum MM2Version
{
    MM2_INVALID = -1,

    MM2_BETA_1  = 0,
    MM2_BETA_2  = 1,
    MM2_RETAIL  = 2,

    MM2_NUM_VERSIONS,

    // versions that cannot load
    MM2_BETA_2_PETITE
};

static const AGEGameInfo g_mm2_info[] = {
    // TODO: fill in values
    { 0x5AB7F8, MM2_BETA_1, 3323, false, "Angel: 3323 / Jun 29 2000 11:52:28" },
    { 0x5C18EC, MM2_BETA_2, 3366, false, "Angel: 3366 / Aug  8 2000 10:08:04" },

    { 0x5C28FC, MM2_RETAIL, 3393, true, "Angel: 3393 / Nov  3 2000 14:34:22" },

    // PEtite'd Beta 2...
    { 0x6B602D, MM2_BETA_2_PETITE, 3366, false, "ERROR!" },
};

typedef struct MM2AddressData {
    DWORD addresses[MM2_NUM_VERSIONS];
} *LPMM2AddressData;

class IMM2HookPtr : public IHookPtr {
private:
    int hook_idx = -1;
protected:
    MM2AddressData addressData;
public:
    NOTHROW inline IMM2HookPtr(const MM2AddressData &addressData);
    NOTHROW inline IMM2HookPtr(MM2Version gameVersion, DWORD dwAddress) {
        IHookPtr::operator=(addressData.addresses[gameVersion] = dwAddress);
    };
    NOTHROW inline ~IMM2HookPtr();

    inline void set_version(MM2Version gameVersion);
};

template<typename TRet>
class MM2FnHook : public IMM2HookPtr {
public:
    NOTHROW inline MM2FnHook(const MM2AddressData &addressData)
        : IMM2HookPtr(addressData) {};

    NOTHROW inline MM2FnHook(MM2Version gameVersion, DWORD dwAddress)
        : IMM2HookPtr(gameVersion, dwAddress) {};

    template<typename ...TArgs>
    inline TRet operator()(TArgs ...args) {
        return static_cast<TRet(__cdecl *)(TArgs...)>(lpAddr)(args...);
    };

    template<typename ...TArgs, class TThis>
    inline TRet operator()(const TThis &&This, TArgs ...args) {
        return static_cast<TRet(__thiscall *)(const TThis, TArgs...)>(lpAddr)(This, args...);
    };
};

template<typename TType>
class MM2PtrHook : public IMM2HookPtr {
public:
    NOTHROW inline MM2PtrHook(const MM2AddressData &addressData)
        : IMM2HookPtr(addressData) {};

    NOTHROW inline MM2PtrHook(MM2Version gameVersion, DWORD dwAddress)
        : IMM2HookPtr(gameVersion, dwAddress) {};

    inline operator TType*() const {
        return (TType*)lpAddr;
    };

    inline operator TType() const {
        if (lpAddr == NULL)
        {
            return NULL;
        }
        else
        {
            TType ret = *static_cast<TType*>(lpAddr);
            return ret;
        }
    };
};

#define INIT_DATA(b1,b2,r) ( MM2AddressData { b1, b2, r } )

namespace MM2 {
    void Printf(LPCSTR str, ...);
    void Messagef(LPCSTR str, ...);
    void Displayf(LPCSTR str, ...);
    void Warningf(LPCSTR str, ...);
    void Errorf(LPCSTR str, ...);
    void Quitf(LPCSTR str, ...);
    void Abortf(LPCSTR str, ...);

    char * AngelReadString(UINT stringId);

    class aiMap {
    public:
        static void Dump(void);
    };

    class datArgParser {
    public:
        static bool Get(LPCSTR arg);
        static bool Get(LPCSTR arg, UINT index, int *out);
        static bool Get(LPCSTR arg, UINT index, float *out);
        static bool Get(LPCSTR arg, UINT index, LPCSTR *out);
    };

    class datAssetManager {
    public:
        /* TODO?
        static Stream * Open(char const *,char const *,bool,bool);
        static Stream * Open(char const *,char const *,char const *,bool,bool);
        static Stream * Create(char const *,char const *,bool);
        static Stream * Create(char const *,char const *,char const *,bool);
        */

        static void FullPath(char *buffer, int length, LPCSTR directory, LPCSTR filename);
        static void FullPath(char *buffer, int length, LPCSTR directory, LPCSTR filename, LPCSTR extension);

        // these don't work for files outside of archives
        static bool Exists(LPCSTR directory, LPCSTR filename);
        static bool Exists(LPCSTR directory, LPCSTR filename, LPCSTR extension);
    };

    class datOutput {
    public:
        static bool OpenLog(LPCSTR filename);
        static void CloseLog(void);

        /* TODO: Add these?
        static void SetBeforeMsgBoxFunction(void(__cdecl *lpFunc)(void));
        static void SetAfterMsgBoxFunction(void(__cdecl *lpFunc)(void));
        */

        static void SetOutputMask(UINT mask);
    };

    class datTimeManager {
    public:
        static void Reset(void);
        static void Update(void);
    };

#ifdef IO_EVENT_HOOK
    struct ioEvent {
        enum ioEventType
        {
            IO_EVENT_DESTROY        = 0,
            IO_EVENT_CREATE         = 1,
            IO_EVENT_LBUTTONUP      = 2,
            IO_EVENT_RBUTTONDOWN    = 3,
            IO_EVENT_RBUTTONUP      = 4,
            IO_EVENT_MBUTTONDOWN    = 5,
            IO_EVENT_MBUTTONUP      = 6,
            IO_EVENT_KEYDOWN        = 7,
            IO_EVENT_CHAR           = 8,
            IO_EVENT_INPUT          = 9,
            IO_EVENT_COMMAND        = 10, /* Not handled by default, maybe implement a custom handler? */
        };
    public:
        ioEventType type;
        int x;
        int y;
        int value;
    };

    class ioEventQueue {
    public:
        static bool Pop(ioEvent *outEvent);
        static bool Peek(ioEvent *outEvent, int *idx);
        static void Queue(ioEvent::ioEventType type, int x, int y, int value);
        static void Command(void *command);
    };
#endif

    // forward declarations
    extern class mmCar;
    extern class mmGame;
    extern class mmGameManager;
    extern class mmHUD;
    extern class mmPlayer;
    extern class mmPopup;

    class mmCar {
        // vehCarSim: 0xB8 (size: ~0x1560)
    private:
        byte _buffer[0x25C];
    };

    class mmGame {
    public:
        inline mmPlayer* getPlayer(void) const {
            return *getPtr<mmPlayer*>(this, 0x48);
        };

        inline mmPopup* getPopup(void) const {
            return *getPtr<mmPopup*>(this, 0x94);
        };
    };

    class mmGameManager {
        // mmSingleRoam: 0x18C
        // mmSingleRace: 0x190
        // mmSingleStunt: 0x194
        // mmSingleCircuit: 0x198
        // mmSingleBlitz: 0x19C
        // mmMultiRace: 0x1A0
        // mmMultiRoam: 0x1A4
        // mmMultiCR: 0x1A8
        // mmMultiCircuit: 0x1AC
        // mmMultiBlitz: 0x1B0
    private:
        byte _buffer[0x1B8];
    public:
        inline mmGame* getGame(void) const {
            return *getPtr<mmGame*>(this, 0x188);
        };

        /* TODO?
        mmGameManager::mmGameManager(void)
        mmGameManager::~mmGameManager(void)
        virtual void mmGameManager::Cull(void)
        virtual void mmGameManager::Update(void)
        virtual void mmGameManager::Reset(void)
        void mmGameManager::BeDone(void)
        void mmGameManager::ForcePopupUI(void)
        void mmGameManager::ForceReplayUI(void)
        */

        static MM2PtrHook<mmGameManager*> Instance(void);
    };

    class mmGameMusicData {
    public:
        bool LoadAmbientSFX(LPCSTR name);
    };

    class mmHUD {
    private:
        byte _buffer[0xB9C]; // unconfirmed
    public:
        void SetMessage(THIS_ LPCSTR message, float duration, int p2);
        void SetMessage2(THIS_ LPCSTR message);

        void PostChatMessage(THIS_ LPCSTR message);
    };

    class mmPlayer {
    private:
        byte _buffer[0x23A4];
    public:
        inline mmCar* getCar(void) const {
            return getPtr<mmCar>(this, 0x2C);
        };

        inline mmHUD* getHUD(void) const {
            return getPtr<mmHUD>(this, 0x288);
        };
    };

    class mmPopup {
        // PUMain: 0x1C (size: 0xC8)
        // PUQuit: 0x20 (size: 0xC4)
        // PUExit: 0x24 (size: 0xBC)
        // TODO...
    private:
        byte _buffer[0x60];
    public:
        inline mmGame* getGame(void) const {
            return *getPtr<mmGame*>(this, 0x18);
        };

        int IsEnabled(THIS_ void);

        void Lock(THIS_ void);
        void Unlock(THIS_ void);
        
        void ProcessChat(THIS_ void);
    };

    extern MM2PtrHook<char>  szCityName;
    extern MM2PtrHook<char>  szCityName2;

    class vehCarAudioContainer {
    public:
        static void SetSirenCSVName(LPCSTR name);
    };

    struct Vector2 {
    public:
        float X;
        float Y;
    };

    struct Vector3 {
    public:
        float X;
        float Y;
        float Z;
    };

    struct Vector4 {
    public:
        float X;
        float Y;
        float Z;
        float W;
    };
};

class CMidtownMadness2 : public CAGEGame {
private:
    MM2Version m_gameVersion;
protected:
    PtrHook<HWND> hwndParent;

    int Initialize() {
        switch (m_gameVersion) {
            case MM2_BETA_1:
            {

            } return HOOK_INIT_UNSUPPORTED;
            case MM2_BETA_2:
            {

            } return HOOK_INIT_UNSUPPORTED;
            case MM2_RETAIL:
            {
                hwndParent = 0x6830B8;
            } return HOOK_INIT_OK;
        }
        return HOOK_INIT_FAILED;
    };
public:
    CMidtownMadness2(int engineVersion);
    CMidtownMadness2(LPAGEGameInfo gameInfo);

    NOINLINE MM2Version GetGameVersion() const {
        return m_gameVersion;
    };

    NOINLINE static bool GetGameInfo(AGEGameInfo &ppGameInfo) {
        for (auto info : g_mm2_info)
        {
            if (info.offset == 0)
                continue;

            if (strcmp((LPCSTR)info.offset, info.age_string) == 0)
            {
                ppGameInfo = info;
                return true;
            }
        }
        return false;
    };

    NOINLINE HWND GetMainWindowHwnd() const {
        return hwndParent;
    };
};