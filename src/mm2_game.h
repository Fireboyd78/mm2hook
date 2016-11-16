#pragma once
#include "mm2_common.h"
#include "mm2_base.h"

namespace MM2
{
    // Forward declarations
    extern class mmCar;
    extern class mmGame;
    extern class mmGameManager;
    extern class mmGameMusicData;
    extern class mmHUD;
    extern class mmPlayer;
    extern class mmPopup;

    // External declarations
    extern class asNode;

    namespace $
    {
        namespace mmGame
        {
            HOOK_API AGEHook<0x412550>::MemberFunc<void> $$ctor;
            HOOK_API AGEHook<0x413940>::MemberFunc<void> $$dtor;

            HOOK_API AGEHook<0x413E90>::MemberFunc<void> Update;
            HOOK_API AGEHook<0x4144A0>::MemberFunc<void> UpdatePaused;
            HOOK_API AGEHook<0x413D40>::MemberFunc<void> Reset;

            HOOK_API AGEHook<0x412710>::MemberFunc<int>  Init;

            HOOK_API AGEHook<0x413650>::MemberFunc<void> InitGameStrings;
            HOOK_API AGEHook<0x4133F0>::MemberFunc<void> InitOtherPlayers;

            HOOK_API AGEHook<0x414290>::MemberFunc<void> HitWaterHandler;

            HOOK_API AGEHook<0x414280>::MemberFunc<void> DropThruCityHandler;
            HOOK_API AGEHook<0x414E50>::MemberFunc<void> SendChatMessage;

            HOOK_API AGEHook<0x414D30>::MemberFunc<void> BeDone;
        }
        namespace mmGameManager
        {
            HOOK_API AGEHook<0x5E0D08>::Type<MM2::mmGameManager *> Instance;
        }
        namespace mmGameMusicData
        {
            HOOK_API AGEHook<0x434060>::MemberFunc<bool> LoadAmbientSFX;
        }
        namespace mmHUD
        {
            HOOK_API AGEHook<0x42E1F0>::MemberFunc<void> SetMessage$1;
            HOOK_API AGEHook<0x42E240>::MemberFunc<void> SetMessage$2;

            HOOK_API AGEHook<0x42D280>::MemberFunc<void> PostChatMessage;
        }
        namespace mmPopup
        {
            HOOK_API AGEHook<0x42A280>::MemberFunc<int>  IsEnabled;
            HOOK_API AGEHook<0x42B4F0>::MemberFunc<void> Lock;
            HOOK_API AGEHook<0x42B500>::MemberFunc<void> Unlock;
            HOOK_API AGEHook<0x42A400>::MemberFunc<void> ProcessChat;
        }
    }

    class mmCar {
        // vehCarSim: 0xB8 (size: ~0x1560)
    private:
        byte _buffer[0x25C];
    };

    class mmGame : public asNode {        
    public:
        AGE_API mmGame(void) {
            PUSH_VTABLE();
            $::mmGame::$$ctor(this);
            POP_VTABLE();
        };

        AGE_API virtual ~mmGame(void) {
            PUSH_VTABLE();
            $::mmGame::$$dtor(this);
            POP_VTABLE();
        };

        inline mmPlayer* getPlayer(void) const {
            return *getPtr<mmPlayer*>(this, 0x48);
        };

        inline mmPopup* getPopup(void) const {
            return *getPtr<mmPopup*>(this, 0x94);
        };

        AGE_API virtual int Init(void) {
            return $::mmGame::Init(this);
        };

        AGE_API virtual void InitGameStrings(void) {
            $::mmGame::InitGameStrings(this);
        };

        AGE_API virtual void InitMyPlayer(void) PURE;

        AGE_API virtual void InitOtherPlayers(void) {
            $::mmGame::InitOtherPlayers(this);
        };

        AGE_API virtual void InitGameObjects(void) PURE;
        AGE_API virtual void InitHUD(void) PURE;
        AGE_API virtual void UpdateGameInput(int) PURE;
        AGE_API virtual void UpdateDebugKeyInput(int) PURE;
        AGE_API virtual void UpdateGame(void) PURE;
        AGE_API virtual void NextRace(void) PURE;

        AGE_API virtual void HitWaterHandler(void) {
            $::mmGame::HitWaterHandler(this);
        };

        AGE_API virtual void DropThruCityHandler(void) {
            $::mmGame::DropThruCityHandler(this);
        };

        AGE_API virtual void SendChatMessage(char *message) {
            $::mmGame::SendChatMessage(this, message);
        };

        AGE_API virtual void SwitchState(int) PURE;

        AGE_API virtual void BeDone(int p1) {
            $::mmGame::BeDone(this, p1);
        };

        AGE_API virtual void * GetWaypoints(void) PURE;
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
        static mmGameManager * Instance(void) {
            return $::mmGameManager::Instance;
        };

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
    };

    class mmGameMusicData {
    public:
        AGE_API bool LoadAmbientSFX(LPCSTR name) {
            return $::mmGameMusicData::LoadAmbientSFX(this, name);
        };
    };

    class mmHUD : public asNode {
    private:
        byte _buffer[0xB9C]; // unconfirmed
    public:
        AGE_API void SetMessage(LPCSTR message, float duration, int p2) {
            $::mmHUD::SetMessage$1(this, message, duration, p2);
        };

        AGE_API void SetMessage(LPCSTR message) {
            $::mmHUD::SetMessage$2(this, message);
        };

        AGE_API void PostChatMessage(LPCSTR message) {
            $::mmHUD::PostChatMessage(this, message);
        };

        static void RegisterLua(luaClassBinder<mmHUD> *lc) {
            asNode::RegisterLua(lc);

            lc->addFunction("SetMessage", static_cast<void (mmHUD::*)(LPCSTR, float, int)>(&mmHUD::SetMessage));
            lc->addFunction("PostChatMessage", &mmHUD::PostChatMessage);
        }
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

        AGE_API int IsEnabled(void) {
            return $::mmPopup::IsEnabled(this);
        };

        AGE_API void Lock(void) {
            $::mmPopup::Lock(this);
        };

        AGE_API void Unlock(void) {
            $::mmPopup::Unlock(this);
        };

        AGE_API void ProcessChat(void) {
            $::mmPopup::ProcessChat(this);
        };
    };
}