#pragma once
#include "common.h"
#include "hook.h"

#include "AGE.h"

enum MM2Version
{
    MM2_INVALID = -1,

    MM2_BETA_1  = 0,
    MM2_BETA_2  = 1,
    MM2_RETAIL  = 2,

    MM2_NUM_VERSIONS
};

static const AGEGameInfo g_mm2_info[MM2_NUM_VERSIONS] = {
    // TODO: fill in values
    { 0, MM2_BETA_1, 3323, false, "NOT DEFINED" },
    { 0, MM2_BETA_2, 3366, false, "NOT DEFINED" },

    { 0x5C28FC, MM2_RETAIL, 3393, true, "Angel: 3393 / Nov  3 2000 14:34:22" },
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
        return (*(TRet(__cdecl *)(TArgs...))lpAddr)(args...);
    };

    template<typename ...TArgs>
    inline TRet ThisCall(LPVOID This, TArgs ...args) {
        return (*(TRet(__thiscall *)(_THIS_ TArgs...))lpAddr)(This, args...);
    };

    template<class TThis, typename ...TArgs>
    inline TRet ThisCall(TThis &This, TArgs ...args) {
        return ThisCall((LPVOID)This, args...);
    };
};

template<typename TType>
class MM2PtrHook : public IMM2HookPtr {
public:
    NOTHROW inline MM2PtrHook(const MM2AddressData &addressData)
        : IMM2HookPtr(addressData) {};

    NOTHROW inline MM2PtrHook(MM2Version gameVersion, DWORD dwAddress)
        : IMM2HookPtr(gameVersion, dwAddress) {};

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

namespace MM2 {
    void Printf(LPCSTR str, ...);
    void Messagef(LPCSTR str, ...);
    void Displayf(LPCSTR str, ...);
    void Warningf(LPCSTR str, ...);
    void Errorf(LPCSTR str, ...);
    void Quitf(LPCSTR str, ...);
    void Abortf(LPCSTR str, ...);

    class aiMap {
    public:
        static void Dump(void);
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
        static bool Pop(ioEvent &outEvent);
        static bool Peek(ioEvent &outEvent, int &idx);
        static void Queue(ioEvent::ioEventType type, int x, int y, int value);
        static void Command(void *command);
    };
#endif

    class mmGameManager {
    public:
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

    class mmPopup {
    public:
        int IsEnabled(THIS_ void);

        void Lock(THIS_ void);
        void Unlock(THIS_ void);
        
        void ProcessChat(THIS_ void);
    };

    class Stream {
    public:
        static void DumpOpenFiles(void);

        static Stream* Open(LPCSTR filename, bool p1);
        static Stream* Create(LPCSTR filename);

        int Read(THIS_ LPVOID dstBuf, int size);
        int Write(THIS_ const LPVOID srcBuf, int size);
        int GetCh(THIS_ void);
        int PutCh(THIS_ unsigned char ch);
        int Seek(THIS_ int offset);
        int Tell(THIS_ void);
        int Close(THIS_ void);
        int Size(THIS_ void);
        int Flush(THIS_ void);
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