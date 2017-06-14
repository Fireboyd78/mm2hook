#pragma once
#include "mm2_common.h"
#include "mm2_base.h"
#include "mm2_vehicle.h"
#include "mm2_ui.h"

namespace MM2
{
    // Forward declarations
    extern class mmGame;
    extern class mmGameManager;
    extern class mmGameMusicData;
    extern class mmHUD;
    extern class mmPlayer;
    extern class dgPhysEntity;
    
    // External declarations
    extern class asNode;
    extern class mmPopup;

    namespace $
    {
        namespace mmMultiCR
        {
            HOOK_EXPORT(0x426D10, _MemberFunc<void>, UpdateTimeWarning);
            HOOK_EXPORT(0x4272E0, _MemberFunc<void>, CycleInterest);
        }
        namespace mmGameManager
        {
            HOOK_EXPORT(0x5E0D08, _Type<MM2::mmGameManager *>, Instance);
        }
    }


    class mmGame : public asNode {
    protected:
        ageHook::Field<0x48, mmPlayer *> _player;
        ageHook::Field<0x94, mmPopup *> _popup;
    public:
        AGE_API mmGame(void) {
            PUSH_VTABLE();
            ageHook::Thunk<0x412550>::Call<void>(this);
            POP_VTABLE();
        };

        AGE_API virtual ~mmGame(void) {
            PUSH_VTABLE();
            ageHook::Thunk<0x413940>::Call<void>(this);
            POP_VTABLE();
        };

        inline mmPlayer* getPlayer(void) const {
            return _player.get(this);
        };

        inline mmPopup* getPopup(void) const {
            return _popup.get(this);
        };

        /*
            asNode virtuals
        */

        AGE_API virtual void Update(void) {
            ageHook::Thunk<0x413E90>::Call<void>(this); //Update);
        };

        AGE_API virtual void Reset(void) {
            ageHook::Thunk<0x413D40>::Call<void>(this); //Reset);
        };

        AGE_API virtual void UpdatePaused(void) {
            ageHook::Thunk<0x4144A0>::Call<void>(this); //UpdatePaused);
        };

        /*
            mmGame virtuals
        */

        AGE_API virtual int Init(void) {
            return ageHook::Thunk<0x412710>::Call<int>(this);
        };

        AGE_API virtual void InitGameStrings(void) {
            ageHook::Thunk<0x413650>::Call<void>(this);
        };

        AGE_API virtual void InitMyPlayer(void) PURE;

        AGE_API virtual void InitOtherPlayers(void) {
            ageHook::Thunk<0x4133F0>::Call<void>(this);
        };

        AGE_API virtual void InitGameObjects(void) PURE;
        AGE_API virtual void InitHUD(void) PURE;
        AGE_API virtual void UpdateGameInput(int) PURE;
        AGE_API virtual void UpdateDebugKeyInput(int) PURE;
        AGE_API virtual void UpdateGame(void) PURE;
        AGE_API virtual void NextRace(void) PURE;

        AGE_API virtual void HitWaterHandler(void) {
            ageHook::Thunk<0x414290>::Call<void>(this);
        };

        AGE_API virtual void DropThruCityHandler(void) {
            ageHook::Thunk<0x414280>::Call<void>(this);
        };

        AGE_API virtual void SendChatMessage(char *message) {
            ageHook::Thunk<0x414E50>::Call<void>(this, message);
        };

        AGE_API virtual void SwitchState(int) PURE;

        AGE_API virtual void BeDone(int p1) {
            ageHook::Thunk<0x414D30>::Call<void>(this, p1);
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
    protected:
        ageHook::Field<0x188, mmGame *> _game;
    public:
        static mmGameManager * Instance(void) {
            return $::mmGameManager::Instance;
        };

        inline mmGame* getGame(void) const {
            return _game.get(this);
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
            return ageHook::Thunk<0x434060>::Call<bool>(this, name);
        };
    };

    class mmHUD : public asNode {
    private:
        byte _buffer[0xB9C]; // unconfirmed
    public:
        AGE_API void SetMessage(LPCSTR message, float duration, int p2) {
            ageHook::Thunk<0x42E1F0>::Call<void>(this, message, duration, p2);
        };

        AGE_API void SetMessage(LPCSTR message) {
            ageHook::Thunk<0x42E240>::Call<void>(this, message);
        };

        AGE_API void PostChatMessage(LPCSTR message) {
            ageHook::Thunk<0x42D280>::Call<void>(this, message);
        };

        static void RegisterLua(luaClassBinder<mmHUD> *lc) {
            asNode::RegisterLua(lc);

            lc->addFunction("SetMessage", static_cast<void (mmHUD::*)(LPCSTR, float, int)>(&mmHUD::SetMessage));
            lc->addFunction("PostChatMessage", &mmHUD::PostChatMessage);
        }
    };

    class mmHudMap {

    };

    class mmPlayer {
    private:
        byte _buffer[0x23A4];
    protected:
        ageHook::Field<0x2C, vehCar> _car;
        ageHook::Field<0x288, mmHUD> _hud;

        ageHook::Field<0x38A, mmHudMap *> _hudmap;

    public:
        inline vehCar* getCar(void) const {
            return _car.ptr(this);
        };

        inline mmHUD* getHUD(void) const {
            return _hud.ptr(this);
        };

        inline mmHudMap* getHudmap(void) const {
            return _hudmap.get(this);
        }

        void ReInit(char* basename) {
            ageHook::Thunk<0x4039B0>::Call<void>(this, basename);
        }
    };
}