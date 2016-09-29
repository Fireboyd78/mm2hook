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

    class mmCar {
        // vehCarSim: 0xB8 (size: ~0x1560)
    private:
        byte _buffer[0x25C];
    };

    class mmGame : public asNode {
    protected:
        static MM2FnHook<void> $$ctor;
        static MM2FnHook<void> $$dtor;

        /*
            overrides
        */
        static MM2FnHook<void> $Update;
        static MM2FnHook<void> $UpdatePaused;
        static MM2FnHook<void> $Reset;
        
        /*
            virtuals
        */
        static MM2FnHook<int> $Init;

        static MM2FnHook<void> $InitGameStrings;
        static MM2FnHook<void> $InitOtherPlayers;

        static MM2FnHook<void> $HitWaterHandler;

        static MM2FnHook<void> $DropThruCityHandler;
        static MM2FnHook<void> $SendChatMessage;

        static MM2FnHook<void> $BeDone;
    public:
        mmGame(void) {
            PUSH_VTABLE();
            $$ctor(this);
            POP_VTABLE();
        };

        virtual ~mmGame(void) {
            PUSH_VTABLE();
            $$dtor(this);
            POP_VTABLE();
        };

        inline mmPlayer* getPlayer(void) const {
            return *getPtr<mmPlayer*>(this, 0x48);
        };

        inline mmPopup* getPopup(void) const {
            return *getPtr<mmPopup*>(this, 0x94);
        };

        virtual int Init(void) {
            $Init(this);
        };

        virtual void InitGameStrings(void) {
            $InitGameStrings(this);
        };

        virtual void InitMyPlayer(void) PURE;

        virtual void InitOtherPlayers(void) {
            $InitOtherPlayers(this);
        };

        virtual void InitGameObjects(void) PURE;
        virtual void InitHUD(void) PURE;
        virtual void UpdateGameInput(int) PURE;
        virtual void UpdateDebugKeyInput(int) PURE;
        virtual void UpdateGame(void) PURE;
        virtual void NextRace(void) PURE;

        virtual void HitWaterHandler(void) {
            $HitWaterHandler(this);
        };

        virtual void DropThruCityHandler(void) {
            $DropThruCityHandler(this);
        };

        virtual void SendChatMessage(char *message) {
            $SendChatMessage(this, message);
        };

        virtual void SwitchState(int) PURE;

        virtual void BeDone(int) {

        };

        virtual void * GetWaypoints(void) PURE;
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
    protected:
        static MM2PtrHook<mmGameManager*> $Instance;
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

        static MM2PtrHook<mmGameManager*> Instance(void) {
            return $Instance;
        };
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
}