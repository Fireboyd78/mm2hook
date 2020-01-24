#pragma once
#include "mm2_common.h"
#include "mm2_base.h"
#include "mm2_vehicle.h"
#include "mm2_ui.h"

#include <modules/node/camera.h>

namespace MM2
{
    // Forward declarations
    class mmGame;
    class mmGameManager;
    class mmGameMusicData;
    class mmCDPlayer;
    class mmArrow;
    class mmHUD;
    class mmPlayer;
    
    // External declarations
    extern class asNode;
    extern class dgPhysEntity;
    extern class mmPopup;
    extern class vehCar;

    extern class camAICS;
    extern class camCarCS;
    extern class camPointCS;
    extern class camPovCS;
    extern class camTrackCS;
    extern class camViewCS;

    namespace $
    {
        namespace mmMultiCR
        {
            declhook(0x426D10, _MemberFunc<void>, UpdateTimeWarning);
            declhook(0x4272E0, _MemberFunc<void>, CycleInterest);
        }
    }


    class mmGame : public asNode {
    protected:
        ageHook::Field<0x48, mmPlayer *> _player;
        ageHook::Field<0x94, mmPopup *> _popup;
    public:
        AGE_API mmGame(void) {
            scoped_vtable x(this);
            ageHook::Thunk<0x412550>::Call<void>(this);
        };

        virtual AGE_API ~mmGame(void) {
            scoped_vtable x(this);
            ageHook::Thunk<0x413940>::Call<void>(this);
        };

        inline mmPlayer * getPlayer(void) const {
            return _player.get(this);
        };

        inline mmPopup* getPopup(void) const {
            return _popup.get(this);
        };

        AGE_API void InitWeather(void)                      { ageHook::Thunk<0x413370>::Call<void>(this); }

        /*
            asNode virtuals
        */

        virtual AGE_API void Update(void)                   { ageHook::Thunk<0x413E90>::Call<void>(this); };
        virtual AGE_API void Reset(void)                    { ageHook::Thunk<0x413D40>::Call<void>(this); };
        virtual AGE_API void UpdatePaused(void)             { ageHook::Thunk<0x4144A0>::Call<void>(this); };

        /*
            mmGame virtuals
        */

        virtual AGE_API int Init(void)                      { return ageHook::Thunk<0x412710>::Call<int>(this); };
        virtual AGE_API void InitGameStrings(void)          { ageHook::Thunk<0x413650>::Call<void>(this); };
        virtual AGE_API void InitMyPlayer(void)             PURE;
        virtual AGE_API void InitOtherPlayers(void)         { ageHook::Thunk<0x4133F0>::Call<void>(this); };
        virtual AGE_API void InitGameObjects(void)          PURE;
        virtual AGE_API void InitHUD(void)                  PURE;
        virtual AGE_API void UpdateGameInput(int)           PURE;
        virtual AGE_API void UpdateDebugKeyInput(int)       PURE;
        virtual AGE_API void UpdateGame(void)               PURE;
        virtual AGE_API void NextRace(void)                 PURE;
        virtual AGE_API void HitWaterHandler(void)          { ageHook::Thunk<0x414290>::Call<void>(this); };
        virtual AGE_API void DropThruCityHandler(void)      { ageHook::Thunk<0x414280>::Call<void>(this); };
        virtual AGE_API void SendChatMessage(char *message) { ageHook::Thunk<0x414E50>::Call<void>(this, message); };
        virtual AGE_API void SwitchState(int)               PURE;
        virtual AGE_API void BeDone(int p1)                 { ageHook::Thunk<0x414D30>::Call<void>(this, p1); };
        virtual AGE_API void * GetWaypoints(void)           PURE;

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<mmGame, asNode>("mmGame")
                //properties
                .addPropertyReadOnly("Player", &getPlayer)
                .addPropertyReadOnly("Popup", &getPopup)
            .endClass();
        }
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
        static ageHook::Type<mmGameManager *> Instance;

        inline mmGame * getGame(void) const {
            return _game.get(this);
        };

        inline mmPlayer * getPlayerSafe(void) const {
            auto game = _game.get(this);
            if (game == nullptr)
                return nullptr;
            auto player = game->getPlayer();
            return player;
        }

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

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<mmGameManager>("mmGameManager")
                //properties
                .addPropertyReadOnly("Game", &getGame)
                
                //statics
                .addStaticFunction("Instance", [] {return (mmGameManager *)Instance; })
            .endClass();
        }
    };

    class mmGameMusicData {
    public:
        AGE_API bool LoadAmbientSFX(LPCSTR name)            { return ageHook::Thunk<0x434060>::Call<bool>(this, name); }
    };

    class mmCDPlayer : public asNode {
    private:
        byte buffer[0x138];
    protected:
        ageHook::Field<0x1C, int> _currentTrack;
        ageHook::Field<0x24, int> _isPlaying;
    public:
        inline int getCurrentTrack(void) const {
            return _currentTrack.get(this);
        };

        inline bool getIsPlaying(void) const {
            return (_isPlaying.get(this) > 0);
        };

        AGE_API void Toggle()                               { ageHook::Thunk<0x4334D0>::Call<void>(this); }
        AGE_API void PrevTrack()                            { ageHook::Thunk<0x433460>::Call<void>(this); }
        AGE_API void PlayStop()                             { ageHook::Thunk<0x433370>::Call<void>(this); }
        AGE_API void NextTrack()                            { ageHook::Thunk<0x4333F0>::Call<void>(this); }

        /*
            asNode virtuals
        */
        AGE_API void Cull()  override                       { ageHook::Thunk<0x433170>::Call<void>(this); }
        AGE_API void Update()  override                     { ageHook::Thunk<0x433100>::Call<void>(this); }
        AGE_API void Reset()  override                      { ageHook::Thunk<0x4330F0>::Call<void>(this); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<mmCDPlayer, asNode>("mmCDPlayer")
                .addPropertyReadOnly("CurrentTrack", &getCurrentTrack)
                .addPropertyReadOnly("IsPlaying", &getIsPlaying)
                .addFunction("Toggle", &Toggle)
                .addFunction("PrevTrack", &PrevTrack)
                .addFunction("NextTrack", &NextTrack)
                .addFunction("PlayStop", &PlayStop)
            .endClass();
        }
    };

    class mmArrow : public asNode {
    private: 
        byte _buffer[0x68];
    public:
        //lua helpers
        inline void luaClearInterest(void) {
            this->SetInterest(nullptr);
        }

        //asNode overrides
        AGE_API void Reset() override                       { ageHook::Thunk<0x42E780>::Call<void>(this); }
        AGE_API void Update() override                      { ageHook::Thunk<0x42E7F0>::Call<void>(this); }

        //mmArrow
        AGE_API void SetInterest(Vector3 *interestPoint)    { ageHook::Thunk<0x42E790>::Call<void>(this, interestPoint); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<mmArrow, asNode>("mmArrow")
                .addFunction("SetInterest", &SetInterest)
                .addFunction("ClearInterest", &luaClearInterest)
                .endClass();
        }
                                                            
    };

    class mmHUD : public asNode {
    private:
        byte _buffer[0xB9C]; // unconfirmed
    protected:
        ageHook::Field<0x0B94, mmCDPlayer*> _cdplayer;
        ageHook::Field<0x09BC, mmArrow> _arrow;
    public:
        inline mmCDPlayer* getCdPlayer(void) const {
            return _cdplayer.get(this);
        };

        inline mmArrow* getArrow(void) const {
            return _arrow.ptr(this);
        };

        //asNode overrides
        AGE_API void Update() override                      { ageHook::Thunk<0x42DBC0>::Call<void>(this); }
        AGE_API void UpdatePaused() override                { ageHook::Thunk<0x42DF10>::Call<void>(this); }
        AGE_API void Cull() override                        { ageHook::Thunk<0x42DF40>::Call<void>(this); }
        AGE_API void Reset() override                       { ageHook::Thunk<0x42DB20>::Call<void>(this); }
        AGE_API void ResChange(int width, int height) override
                                                            { ageHook::Thunk<0x42D2E0>::Call<void>(this, width, height); }

        //mmHud
        AGE_API void Init(LPCSTR vehName, mmPlayer* player, BOOL useAltTimingMethod)
                                                            { ageHook::Thunk<0x42D5E0>::Call<void>(this); }
        AGE_API void StartTimers()                          { ageHook::Thunk<0x42D560>::Call<void>(this); }
        AGE_API void StopTimers()                           { ageHook::Thunk<0x42D580>::Call<void>(this); }
        AGE_API void ResetTimers()                          { ageHook::Thunk<0x42D5A0>::Call<void>(this); }
        AGE_API void Enable()                               { ageHook::Thunk<0x42D910>::Call<void>(this); }
        AGE_API void Disable(BOOL a2)                       { ageHook::Thunk<0x42D970>::Call<void>(this, a2); }
        AGE_API void Toggle()                               { ageHook::Thunk<0x42D9D0>::Call<void>(this); }
        AGE_API void SetDash(BOOL active)                   { ageHook::Thunk<0x42DA90>::Call<void>(this, active); }
        AGE_API void ActivateDash()                         { ageHook::Thunk<0x42DAB0>::Call<void>(this); }
        AGE_API void DeactivateDash()                       { ageHook::Thunk<0x42DAE0>::Call<void>(this); }
        AGE_API void ActivateGold()                         { ageHook::Thunk<0x42E300>::Call<void>(this); }
        AGE_API void DeactivateGold()                       { ageHook::Thunk<0x42E310>::Call<void>(this); }
        AGE_API BOOL IsDashActive()                         { return ageHook::Thunk<0x42DB10>::Call<BOOL>(this); }
        AGE_API void SetTransparency(bool transparency)     { ageHook::Thunk<0x42DB60>::Call<void>(this, transparency); }
        AGE_API void ShowSplitTime()                        { ageHook::Thunk<0x42E040>::Call<void>(this); }
        AGE_API void SetStandings(int lhs, int rhs)         { ageHook::Thunk<0x42E0A0>::Call<void>(this, lhs, rhs); }
        AGE_API void PostLapTime(bool finalLap)             { ageHook::Thunk<0x42E0E0>::Call<void>(this, finalLap); }
        AGE_API void SetLapTime(int lap, int a3, bool stopTimer) 
                                                            { ageHook::Thunk<0x42E140>::Call<void>(this, lap, a3, stopTimer); }
        AGE_API void SetMessage(LPCSTR message, float duration, int p2)
                                                            { ageHook::Thunk<0x42E1F0>::Call<void>(this, message, duration, p2); };
        AGE_API void SetMessage(LPCSTR message)             { ageHook::Thunk<0x42E240>::Call<void>(this, message); };
        AGE_API void PostChatMessage(LPCSTR message)        { ageHook::Thunk<0x42D280>::Call<void>(this, message); };
        AGE_API void PlayNetAlert()                         { ageHook::Thunk<0x42E340>::Call<void>(this); }
        AGE_API void TogglePositionDisplay(BOOL enable)     { ageHook::Thunk<0x42E360>::Call<void>(this, enable); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<mmHUD, asNode>("mmHUD")
                .addPropertyReadOnly("CDPlayer", &getCdPlayer)
                .addPropertyReadOnly("Arrow", &getArrow)

                .addFunction("Init", &Init, LUA_ARGS(LPCSTR, mmPlayer*, BOOL))
                .addFunction("SetMessage", static_cast<void (mmHUD::*)(LPCSTR, float, int)>(&SetMessage))
                .addFunction("StartTimers", &StartTimers)
                .addFunction("StopTimers", &StopTimers)
                .addFunction("ResetTimers", &ResetTimers)
                .addFunction("Enable", &Enable)
                .addFunction("Disable", &Disable, LUA_ARGS(bool))
                .addFunction("Toggle", &Toggle)
                .addFunction("SetDash", &SetDash, LUA_ARGS(bool))
                .addFunction("ActivateDash", &ActivateDash)
                .addFunction("DeactivateDash", &DeactivateDash)
                .addFunction("SetTransparency", &SetTransparency)
                .addFunction("ShowSplitTime", &ShowSplitTime)
                .addFunction("SetStandings", &SetStandings)
                .addFunction("PostLapTime", &PostLapTime)
                .addFunction("SetLapTime", &SetLapTime)
                .addFunction("PostChatMessage", &PostChatMessage)
                .addFunction("PlayNetAlert", &PlayNetAlert)
                .addFunction("TogglePositionDisplay", &TogglePositionDisplay, LUA_ARGS(bool))
            .endClass();
        }
    };

    class mmHudMap : public asNode {
    protected:
        AGE_API int GetCurrentMapMode()                     { return ageHook::Thunk<0x42EF20>::Call<int>(this); }
        AGE_API int GetNextMapMode()                        { return ageHook::Thunk<0x42EF00>::Call<int>(this); }
        AGE_API void SetMapMode(int a1)                     { ageHook::Thunk<0x42EF30>::Call<void>(this, a1); }
    public:
        AGE_API void Activate()                             { ageHook::Thunk<0x42EEE0>::Call<void>(this); }
        AGE_API void Deactivate()                           { ageHook::Thunk<0x42EEF0>::Call<void>(this); }
        AGE_API void SetOrient(bool a1)                     { ageHook::Thunk<0x42FA40>::Call<void>(this, a1); }
        AGE_API void SetZoomIn(bool a1)                     { ageHook::Thunk<0x42FA20>::Call<void>(this, a1); }
        AGE_API void ToggleMapOrient()                      { ageHook::Thunk<0x42FA10>::Call<void>(this); }
        AGE_API void ToggleMapRes()                         { ageHook::Thunk<0x42FA00>::Call<void>(this); }
        AGE_API bool GetOrient()                            { return ageHook::Thunk<0x42FA50>::Call<bool>(this); }
        AGE_API bool GetZoomIn()                            { return ageHook::Thunk<0x42FA30>::Call<bool>(this); }

        /*
            asNode virtuals
        */

        AGE_API void Cull() override                { ageHook::Thunk<0x42F1B0>::Call<void>(this); }
        AGE_API void Update() override              { ageHook::Thunk<0x42F1A0>::Call<void>(this); }
        AGE_API void Reset() override               { ageHook::Thunk<0x42EE90>::Call<void>(this); }
        AGE_API void FileIO(datParser &parser) override 
                                                    { ageHook::Thunk<0x42FA60>::Call<void>(this, &parser); }
        AGE_API char * GetClassName() override      { return ageHook::Thunk<0x42FD60>::Call<char*>(this); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<mmHudMap, asNode>("mmHudMap")
                .addFunction("Activate", &Activate)
                .addFunction("Deactivate", &Deactivate)
                .addFunction("SetOrient", &SetOrient)
                .addFunction("SetZoomIn", &SetZoomIn)
                .addFunction("ToggleMapOrient", &ToggleMapOrient)
                .addFunction("ToggleMapRes", &ToggleMapRes)
                .addFunction("GetOrient", &GetOrient)
                .addFunction("GetZoomIn", &GetZoomIn)
                .addFunction("GetCurrentMapMode", &GetCurrentMapMode)
                .addFunction("GetNextMapMode", &GetNextMapMode)
                .addFunction("SetMapMode", &SetMapMode)
            .endClass();  
        }
    };

    class mmPlayer : public asNode {
    private:
        byte _buffer[0x23A4];
    protected:
        ageHook::Field<0x2C, vehCar> _car;
        ageHook::Field<0x288, mmHUD> _hud;
        
        ageHook::Field<0xE28, mmHudMap *> _hudmap;
        ageHook::Field<0xE2C, camViewCS *> _camView;

        ageHook::Field<0xE60, camTrackCS> _nearCam;
        ageHook::Field<0x10F8, camTrackCS> _farCam;
        ageHook::Field<0x1390, camTrackCS> _indCam;
        ageHook::Field<0x1628, camPovCS> _povCam;
        ageHook::Field<0x1770, camPovCS> _dashCam;
        ageHook::Field<0x18B8, camCarCS> _polarCam1;
        ageHook::Field<0x19E0, camCarCS> _polarCam2;
        ageHook::Field<0x1B08, camAICS> _freeCam;
        ageHook::Field<0x1C2C, camPointCS> _pointCam;
        ageHook::Field<0x1D70, camCarCS> _preCam;
        ageHook::Field<0x1E98, camCarCS> _postCam;
        ageHook::Field<0x1FBC, camCarCS> _polarCam3;

    public:
        inline vehCar * getCar(void) const                  { return _car.ptr(this); }
        inline mmHUD * getHUD(void) const                   { return _hud.ptr(this); }

        inline mmHudMap * getHudmap(void) const             { return _hudmap.get(this); }
        inline camViewCS * getCamView(void) const           { return _camView.get(this); }

        inline camTrackCS * getNearCam(void) const          { return _nearCam.ptr(this); }
        inline camTrackCS * getFarCam(void) const           { return _farCam.ptr(this); }
        inline camTrackCS * getIndCam(void) const           { return _indCam.ptr(this); }
        inline camPovCS * getPovCam(void) const             { return _povCam.ptr(this); }
        inline camPovCS * getDashCam(void) const            { return _dashCam.ptr(this); }
        inline camAICS * getFreecam(void) const             { return _freeCam.ptr(this); }
        inline camPointCS * getPointCam(void) const         { return _pointCam.ptr(this); }
        inline camCarCS * getPreCam(void) const             { return _preCam.ptr(this); }
        inline camCarCS * getPostCam(void) const            { return _postCam.ptr(this); }
  
        inline camCarCS * getPolarCamOne(void) const        { return _polarCam1.ptr(this); }
        inline camCarCS * getPolarCamTwo(void) const        { return _polarCam2.ptr(this); }
        inline camCarCS * getPolarCamThree(void) const      { return _polarCam3.ptr(this); }

        AGE_API void EnableRegen(bool a1)                   { ageHook::Thunk<0x406160>::Call<void>(this, a1); }
        AGE_API float FilterSteering(float a1)              { return ageHook::Thunk<0x404C90>::Call<float>(this, a1); }
        AGE_API bool IsMaxDamaged()                         { return ageHook::Thunk<0x406140>::Call<bool>(this); }
        AGE_API void ResetDamage()                          { ageHook::Thunk<0x406180>::Call<void>(this); }
        AGE_API void SetCamera(int a1, int a2)              { ageHook::Thunk<0x404710>::Call<void>(this, a1, a2); }
        AGE_API void SetMPPostCam(Matrix34 *a1, float a2)   { ageHook::Thunk<0x404460>::Call<void>(this, a1, a2); }
        AGE_API void SetPostRaceCam()                       { ageHook::Thunk<0x404350>::Call<void>(this); }
        AGE_API void SetPreRaceCam()                        { ageHook::Thunk<0x404250>::Call<void>(this); }
        AGE_API void SetSteering(float a1)                  { ageHook::Thunk<0x404C50>::Call<void>(this, a1); }
        AGE_API void SetWideFOV(bool a1)                    { ageHook::Thunk<0x404580>::Call<void>(this, a1); }
        AGE_API void ReInit(const char *basename)           { ageHook::Thunk<0x4039B0>::Call<void>(this, basename); }

        /*
            asNode virtuals
        */

        virtual AGE_API void BeforeSave()                   { ageHook::Thunk<0x403990>::Call<void>(this); }
        virtual AGE_API void AfterLoad()                    { ageHook::Thunk<0x4039A0>::Call<void>(this); }
        virtual AGE_API void FileIO(datParser &parser)      { ageHook::Thunk<0x406320>::Call<void>(this, &parser); }
        virtual AGE_API void Reset()                        { ageHook::Thunk<0x404A60>::Call<void>(this); }
        virtual AGE_API void Update()                       { ageHook::Thunk<0x405760>::Call<void>(this); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<mmPlayer, asNode>("mmPlayer")
                //properties
                .addPropertyReadOnly("Car", &getCar)
                .addPropertyReadOnly("HUD", &getHUD)
                .addPropertyReadOnly("Hudmap", &getHudmap)
                .addPropertyReadOnly("CamView", &getCamView)
                .addPropertyReadOnly("FreeCam", &getFreecam)
                .addPropertyReadOnly("FarCam", &getFarCam)
                .addPropertyReadOnly("NearCam", &getNearCam)
                .addPropertyReadOnly("IndCam", &getIndCam)
                .addPropertyReadOnly("DashCam", &getDashCam)
                .addPropertyReadOnly("PovCam", &getPovCam)
                .addPropertyReadOnly("PointCam", &getPointCam)

                //.addPropertyReadOnly("PreCam", &getPreCam)
                //.addPropertyReadOnly("PostCam", &getPostCam)
                //.addPropertyReadOnly("PolarCamOne", &getPolarCamOne)
                //.addPropertyReadOnly("PolarCamTwo", &getPolarCamTwo)
                //.addPropertyReadOnly("PolarCamThree", &getPolarCamThree)

                //functions
                .addFunction("EnableRegen", &EnableRegen)
                .addFunction("FilterSteering", &FilterSteering)
                .addFunction("IsMaxDamaged", &IsMaxDamaged)
                .addFunction("ReInit", &ReInit)
                .addFunction("ResetDamage", &ResetDamage)
                .addFunction("SetCamera", &SetCamera)
                .addFunction("SetMPPostCam", &SetMPPostCam)
                .addFunction("SetPostRaceCam", &SetPostRaceCam)
                .addFunction("SetPreRaceCam", &SetPreRaceCam)
                .addFunction("SetSteering", &SetSteering)
                .addFunction("SetWideFOV", &SetWideFOV)
            .endClass();
        }
    };

    template<>
    void luaAddModule<module_game>(LuaState L) {
        luaBind<mmGame>(L);
        luaBind<mmGameManager>(L);
        luaBind<mmArrow>(L);
        luaBind<mmHUD>(L);
        luaBind<mmCDPlayer>(L);
        luaBind<mmHudMap>(L);
        luaBind<mmPlayer>(L);
    }
}