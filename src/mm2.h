#pragma once
#include "common.h"
#include "hook.h"

#include "AGE.h"

#include "mm2_utils.h"
#include "mm2_stream.h"

static const AGEGameInfo g_mm2_info[] = {
    { 0x5AB7F8, MM2_BETA_1, 3323, true, "Angel: 3323 / Jun 29 2000 11:52:28" },
    { 0x5C18EC, MM2_BETA_2, 3366, true, "Angel: 3366 / Aug  8 2000 10:08:04" },

    { 0x5C28FC, MM2_RETAIL, 3393, true, "Angel: 3393 / Nov  3 2000 14:34:22" },

    // PEtite'd Beta 2...
    { 0x6B602D, MM2_BETA_2_PETITE, 3366, false, "ERROR!" },
};

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
    protected:
        static MM2FnHook<void>     $Dump;
        static MM2PtrHook<aiMap>   $AIMAP;
    public:
        static void Dump(void) {
            const aiMap* This = $AIMAP;
            $Dump(&*This);
        };
    };

    class datArgParser {
    protected:
        static MM2FnHook<bool> $Get_$1;
        static MM2FnHook<bool> $Get_$2;
        static MM2FnHook<bool> $Get_$3;
        static MM2FnHook<bool> $Get_$4;
    public:
        static bool Get(LPCSTR arg) {
            return $Get_$1(arg);
        };

        static bool Get(LPCSTR arg, UINT index, int *out) {
            return $Get_$2(arg, index, out);
        };

        static bool Get(LPCSTR arg, UINT index, float *out) {
            return $Get_$3(arg, index, out);
        };

        static bool Get(LPCSTR arg, UINT index, LPCSTR *out) {
            return $Get_$4(arg, index, out);
        };
    };

    class datAssetManager {
    protected:
        static MM2FnHook<void> $FullPath_$1;
        static MM2FnHook<void> $FullPath_$2;
        static MM2FnHook<bool> $Exists_$1;
        static MM2FnHook<bool> $Exists_$2;
    public:
        /* TODO?
        static Stream * Open(char const *,char const *,bool,bool);
        static Stream * Open(char const *,char const *,char const *,bool,bool);
        static Stream * Create(char const *,char const *,bool);
        static Stream * Create(char const *,char const *,char const *,bool);
        */

        static void FullPath(char *buffer, int length, LPCSTR directory, LPCSTR filename) {
            $FullPath_$1(buffer, length, directory, filename);
        };

        static void FullPath(char *buffer, int length, LPCSTR directory, LPCSTR filename, LPCSTR extension) {
            $FullPath_$2(buffer, length, directory, filename, extension);
        };

        // these don't work for files outside of archives
        static bool Exists(LPCSTR directory, LPCSTR filename) {
            return $Exists_$1(directory, filename);
        };

        static bool Exists(LPCSTR directory, LPCSTR filename, LPCSTR extension) {
            return $Exists_$2(directory, filename, extension);
        };
    };

    class datOutput {
    protected:
        static MM2FnHook<void> $CloseLog;
        static MM2FnHook<bool> $OpenLog;
        static MM2FnHook<void> $SetOutputMask;
    public:
        static bool OpenLog(LPCSTR filename) {
            return $OpenLog(filename);
        };

        static void CloseLog(void) {
            $CloseLog();
        };

        /* TODO: Add these?
        static void SetBeforeMsgBoxFunction(void(__cdecl *lpFunc)(void));
        static void SetAfterMsgBoxFunction(void(__cdecl *lpFunc)(void));
        */

        static void SetOutputMask(UINT mask) {
            $SetOutputMask(mask);
        };
    };

    class datTimeManager {
    protected:
        static MM2FnHook<void> $Reset;
        static MM2FnHook<void> $Update;
    public:
        static void Reset(void) {
            $Reset();
        };

        static void Update(void) {
            $Update();
        };
    };

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
    protected:
        static MM2FnHook<bool> $Pop;
        static MM2FnHook<bool> $Peek;
        static MM2FnHook<void> $Queue;
        static MM2FnHook<void> $Command;
    public:
        static bool Pop(ioEvent *outEvent) {
            $Pop(outEvent);
        };

        static bool Peek(ioEvent *outEvent, int *idx) {
            $Peek(outEvent, idx);
        };

        static void Queue(ioEvent::ioEventType type, int x, int y, int value) {
            $Queue(type, x, y, value);
        };

        static void Command(void *command) {
            $Command(command);
        };
    };

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
    protected:
        static MM2FnHook<bool> $LoadAmbientSFX;
    public:
        bool LoadAmbientSFX(LPCSTR name) {
            return $LoadAmbientSFX(this, name);
        };
    };

    class mmHUD {
    private:
        byte _buffer[0xB9C]; // unconfirmed
    protected:
        static MM2FnHook<void> $SetMessage;
        static MM2FnHook<void> $SetMessage2;
        static MM2FnHook<void> $PostChatMessage;
    public:
        void SetMessage(THIS_ LPCSTR message, float duration, int p2) {
            $SetMessage(this, message, duration, p2);
        };

        void SetMessage2(THIS_ LPCSTR message) {
            $SetMessage2(this, message);
        };

        void PostChatMessage(THIS_ LPCSTR message) {
            $PostChatMessage(this, message);
        };
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
    protected:
        static MM2FnHook<int>  $IsEnabled;
        static MM2FnHook<void> $Lock;
        static MM2FnHook<void> $Unlock;
        static MM2FnHook<void> $ProcessChat;
    public:
        inline mmGame* getGame(void) const {
            return *getPtr<mmGame*>(this, 0x18);
        };

        int IsEnabled(THIS_ void) {
            return $IsEnabled(this);
        };

        void Lock(THIS_ void) {
            $Lock(this);
        };

        void Unlock(THIS_ void) {
            $Unlock(this);
        };
        
        void ProcessChat(THIS_ void) {
            $ProcessChat(this);
        };
    };

    extern MM2PtrHook<char>  szCityName;
    extern MM2PtrHook<char>  szCityName2;

    class vehCarAudioContainer {
    protected:
        static MM2FnHook<void> $SetSirenCSVName;
    public:
        static void SetSirenCSVName(LPCSTR name) {
            $SetSirenCSVName(name);
        };
    };

    struct Matrix34 {
    public:
        float m11;
        float m12;
        float m13;
        float m14;

        float m21;
        float m22;
        float m23;
        float m24;

        float m31;
        float m32;
        float m33;
        float m34;
    };

    struct Matrix44 {
    public:
        float m11;
        float m12;
        float m13;
        float m14;

        float m21;
        float m22;
        float m23;
        float m24;

        float m31;
        float m32;
        float m33;
        float m34;

        float m41;
        float m42;
        float m43;
        float m44;
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

    struct cityTimeWeatherLighting {
        /*0x00*/float KeyHeading;
        /*0x04*/float KeyPitch;
        /*0x08*/Vector3 KeyColor;

        /*0x14*/float Fill1Heading;
        /*0x18*/float Fill1Pitch;
        /*0x1C*/Vector3 Fill1Color;

        /*0x28*/float Fill2Heading;
        /*0x2C*/float Fill2Pitch;
        /*0x30*/Vector3 Fill2Color;

        // "ambient light levels" that get calculated using the Ambient color?
        // doesn't care for any of the Key/Pitch values
        /*0x3C*/int AmbientLevel3; // fullbright
        /*0x40*/int AmbientLevel2;
        /*0x44*/int AmbientLevel1;

        /*0x48*/int Ambient; // looks like the alpha is ignored
    };
};

class CMidtownMadness2 : public CAGEGame {
private:
    MM2Version m_gameVersion;
protected:
    MM2PtrHook<HWND> hwndParent = { 0x64993C, 0x681B00, 0x6830B8 };
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