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
    public:
        AGE_API mmGame(void);
        AGE_API virtual ~mmGame(void);

        inline mmPlayer* getPlayer(void) const {
            return *getPtr<mmPlayer*>(this, 0x48);
        };

        inline mmPopup* getPopup(void) const {
            return *getPtr<mmPopup*>(this, 0x94);
        };

        AGE_API virtual int Init(void);
        AGE_API virtual void InitGameStrings(void);
        AGE_API virtual void InitMyPlayer(void) PURE;
        AGE_API virtual void InitOtherPlayers(void);
        AGE_API virtual void InitGameObjects(void) PURE;
        AGE_API virtual void InitHUD(void) PURE;
        AGE_API virtual void UpdateGameInput(int) PURE;
        AGE_API virtual void UpdateDebugKeyInput(int) PURE;
        AGE_API virtual void UpdateGame(void) PURE;
        AGE_API virtual void NextRace(void) PURE;
        AGE_API virtual void HitWaterHandler(void);
        AGE_API virtual void DropThruCityHandler(void);
        AGE_API virtual void SendChatMessage(char *message);
        AGE_API virtual void SwitchState(int) PURE;
        AGE_API virtual void BeDone(int p1);
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
        inline mmGame* getGame(void) const {
            return *getPtr<mmGame*>(this, 0x188);
        };

        static mmGameManager * Instance(void);

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
        AGE_API bool LoadAmbientSFX(LPCSTR name);
    };

    class mmHUD : public asNode {
    private:
        byte _buffer[0xB9C]; // unconfirmed
    public:
        AGE_API void SetMessage(LPCSTR message, float duration, int p2);
        AGE_API void SetMessage(LPCSTR message);
        AGE_API void PostChatMessage(LPCSTR message);

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

        AGE_API int IsEnabled(void);
        AGE_API void Lock(void);
        AGE_API void Unlock(void);
        AGE_API void ProcessChat(void);
    };
}