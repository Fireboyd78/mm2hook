#pragma once
#include "mm2_common.h"
#include "mm2_base.h"
#include "mm2_vehicle.h"
#include "mm2_ui.h"
#include "mm2_city.h"

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
        ANGEL_ALLOCATOR

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
        AGE_API void SetIconsState(void)                    { ageHook::Thunk<0x414BB0>::Call<void>(this); }

        /*
            asNode virtuals
        */

        virtual AGE_API void Update(void) override          { ageHook::Thunk<0x413E90>::Call<void>(this); };
        virtual AGE_API void Reset(void) override           { ageHook::Thunk<0x413D40>::Call<void>(this); };
        virtual AGE_API void UpdatePaused(void) override    { ageHook::Thunk<0x4144A0>::Call<void>(this); };

        /*
            mmGame virtuals
        */

        virtual AGE_API int Init()                          { return ageHook::Thunk<0x412710>::Call<int>(this); };
        virtual AGE_API void InitGameStrings()              { ageHook::Thunk<0x413650>::Call<void>(this); };
        virtual AGE_API void InitMyPlayer()                 PURE;
        virtual AGE_API void InitOtherPlayers()             { ageHook::Thunk<0x4133F0>::Call<void>(this); };
        virtual AGE_API void InitGameObjects()              PURE;
        virtual AGE_API void InitHUD()                      PURE;
        virtual AGE_API void UpdateGameInput(int)           PURE;
        virtual AGE_API void UpdateDebugKeyInput(int)       PURE;
        virtual AGE_API void UpdateGame()                   PURE;
        virtual AGE_API void NextRace()                     PURE;
        virtual AGE_API void HitWaterHandler()              { ageHook::Thunk<0x414290>::Call<void>(this); };
        virtual AGE_API void DropThruCityHandler()          { ageHook::Thunk<0x414280>::Call<void>(this); };
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

    class mmGameSingle : public mmGame {
    public:
        ANGEL_ALLOCATOR

        AGE_API mmGameSingle(void) {
            scoped_vtable x(this);
            ageHook::Thunk<0x433A60>::Call<void>(this);
        };

        virtual AGE_API ~mmGameSingle(void) {
            scoped_vtable x(this);
            ageHook::Thunk<0x433A90>::Call<void>(this);
        };

        /*
            asNode virtuals
        */

        virtual AGE_API void Update(void) override          { ageHook::Thunk<0x433C80>::Call<void>(this); };
        virtual AGE_API void Reset(void) override           { ageHook::Thunk<0x433AC0>::Call<void>(this); };

        /*
            mmGame overrides
        */

        virtual AGE_API int Init() override                 { return ageHook::Thunk<0x433AA0>::Call<int>(this); };
        virtual AGE_API void InitOtherPlayers() override    { ageHook::Thunk<0x433AB0>::Call<void>(this); };
        virtual AGE_API void UpdateGameInput(int a1) override  
                                                            { ageHook::Thunk<0x433C90>::Call<void>(this, a1); }
        virtual AGE_API void UpdateDebugKeyInput(int a1) override 
                                                            { ageHook::Thunk<0x433F20>::Call<void>(this, a1); }
        virtual AGE_API void UpdateGame() override          { ageHook::Thunk<0x433CA0>::Call<void>(this); }
        virtual AGE_API void HitWaterHandler() override     { ageHook::Thunk<0x433C70>::Call<void>(this); };
        virtual AGE_API void* GetWaypoints() override       { return ageHook::Thunk<0x41ABA0>::Call<void*>(this); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<mmGameSingle, mmGame>("mmGameSingle")
                .endClass();
        }
    };

    class mmGameMulti : public mmGame {
    public:
        ANGEL_ALLOCATOR

        AGE_API mmGameMulti(void) {
            scoped_vtable x(this);
            ageHook::Thunk<0x438CC0>::Call<void>(this);
        };

        virtual AGE_API ~mmGameMulti(void) {
            scoped_vtable x(this);
            ageHook::Thunk<0x438E60>::Call<void>(this);
        };

        /*
            mmGameMulti virtuals
        */
        virtual AGE_API void InitNetworkPlayers()           PURE;
        virtual AGE_API void InitRoster()                   { ageHook::Thunk<0x43A750>::Call<void>(this); }
        virtual AGE_API void SystemMessage(void*)           PURE;
        virtual AGE_API void GameMessage(void*)             PURE;

        /*
            asNode virtuals
        */

        virtual AGE_API void Update(void) override          { ageHook::Thunk<0x43A900>::Call<void>(this); };
        virtual AGE_API void Reset(void) override           { ageHook::Thunk<0x43A5D0>::Call<void>(this); };

        /*
            mmGame overrides
        */

        virtual AGE_API int Init() override                 { return ageHook::Thunk<0x438F40>::Call<int>(this); };
        virtual AGE_API void InitGameStrings() override     { ageHook::Thunk<0x439240>::Call<void>(this); };
        virtual AGE_API void InitOtherPlayers() override    { ageHook::Thunk<0x43A720>::Call<void>(this); };
        virtual AGE_API void UpdateDebugKeyInput(int a1) override 
                                                            { ageHook::Thunk<0x43ADC0>::Call<void>(this, a1); }
        virtual AGE_API void UpdateGame() override          { ageHook::Thunk<0x43A980>::Call<void>(this); }
        virtual AGE_API void NextRace() override            { ageHook::Thunk<0x421B70>::Call<void>(this); };
        virtual AGE_API void HitWaterHandler() override     { ageHook::Thunk<0x439130>::Call<void>(this); };
        virtual AGE_API void DropThruCityHandler() override { ageHook::Thunk<0x439230>::Call<void>(this); };
        virtual AGE_API void SendChatMessage(char *a1) override       
                                                            { ageHook::Thunk<0x43B530>::Call<void*>(this, a1); }
        virtual AGE_API void BeDone(int a1) override        { ageHook::Thunk<0x43A4D0>::Call<void>(this, a1); };

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<mmGameMulti, mmGame>("mmGameMulti")
                .endClass();
        }
    };

    class mmSingleRoam : public mmGameSingle
    {
    private:
        byte _buffer[0x76F0 - sizeof(asNode) - 0x4];
    public:
        ANGEL_ALLOCATOR

        AGE_API mmSingleRoam(void) {
            //scoped_vtable x(this);
            ageHook::Thunk<0x41F990>::Call<void>(this);
        };

        virtual AGE_API ~mmSingleRoam(void) {
            scoped_vtable x(this);
            ageHook::Thunk<0x41F9C0>::Call<void>(this);
        };

        /*
            asNode virtuals
        */

        virtual AGE_API void Update(void) override          { ageHook::Thunk<0x41FCE0>::Call<void>(this); };
        virtual AGE_API void Reset(void) override           { ageHook::Thunk<0x41FC90>::Call<void>(this); };

        /*
            mmGame overrides
        */

        virtual AGE_API int Init() override                 { return ageHook::Thunk<0x41FA30>::Call<int>(this); };
        virtual AGE_API void InitMyPlayer() override        { ageHook::Thunk<0x41FAA0>::Call<void>(this); };
        virtual AGE_API void InitOtherPlayers() override    { ageHook::Thunk<0x41FC40>::Call<void>(this); };
        virtual AGE_API void InitGameObjects() override     { ageHook::Thunk<0x41FB40>::Call<void>(this); };
        virtual AGE_API void InitHUD() override             { ageHook::Thunk<0x41FB10>::Call<void>(this); };
        virtual AGE_API void UpdateGameInput(int a1) override  
                                                            { ageHook::Thunk<0x41FCF0>::Call<void>(this, a1); }
        virtual AGE_API void UpdateDebugKeyInput(int a1) override 
                                                            { ageHook::Thunk<0x41FD00>::Call<void>(this, a1); }
        virtual AGE_API void UpdateGame() override          { ageHook::Thunk<0x433CA0>::Call<void>(this); }
        virtual AGE_API void NextRace() override            { ageHook::Thunk<0x41FDA0>::Call<void>(this); };
        virtual AGE_API void HitWaterHandler() override     { ageHook::Thunk<0x41FCD0>::Call<void>(this); };
        virtual AGE_API void SwitchState(int a1) override   { ageHook::Thunk<0x41FD90>::Call<void>(this, a1); };

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<mmSingleRoam, mmGameSingle>("mmSingleRoam")
                .endClass();
        }
    };

    ASSERT_SIZEOF(mmSingleRoam, 0x76F0);
    
    class mmSingleRace : public mmGameSingle
    {
    private:
        byte _buffer[0x76F0 - sizeof(asNode) - 0x4];
    public:
        ANGEL_ALLOCATOR

        AGE_API mmSingleRace(void) {
            scoped_vtable x(this);
            ageHook::Thunk<0x41DF30>::Call<void>(this);
        };

        virtual AGE_API ~mmSingleRace(void) {
            scoped_vtable x(this);
            ageHook::Thunk<0x41DF60>::Call<void>(this);
        };

        /*
            asNode virtuals
        */

        virtual AGE_API void Update(void) override          { ageHook::Thunk<0x41E5A0>::Call<void>(this); };
        virtual AGE_API void Reset(void) override           { ageHook::Thunk<0x41E4F0>::Call<void>(this); };

        /*
            mmGame overrides
        */

        virtual AGE_API int Init() override                 { return ageHook::Thunk<0x41E010>::Call<int>(this); };
        virtual AGE_API void InitMyPlayer() override        { ageHook::Thunk<0x41E130>::Call<void>(this); };
        virtual AGE_API void InitOtherPlayers() override    { ageHook::Thunk<0x41E4E0>::Call<void>(this); };
        virtual AGE_API void InitGameObjects() override     { ageHook::Thunk<0x41E200>::Call<void>(this); };
        virtual AGE_API void InitHUD() override             { ageHook::Thunk<0x41E1A0>::Call<void>(this); };
        virtual AGE_API void UpdateGameInput(int a1) override  
                                                            { ageHook::Thunk<0x41E5B0>::Call<void>(this, a1); }
        virtual AGE_API void UpdateDebugKeyInput(int a1) override 
                                                            { ageHook::Thunk<0x41F970>::Call<void>(this, a1); }
        virtual AGE_API void UpdateGame() override          { ageHook::Thunk<0x41E5F0>::Call<void>(this); }
        virtual AGE_API void NextRace() override            { ageHook::Thunk<0x41F600>::Call<void>(this); };
        virtual AGE_API void HitWaterHandler() override     { ageHook::Thunk<0x41E530>::Call<void>(this); };
        virtual AGE_API void SwitchState(int a1) override   { ageHook::Thunk<0x41EDB0>::Call<void>(this, a1); };
        virtual AGE_API void* GetWaypoints() override       { return ageHook::Thunk<0x41F980>::Call<void*>(this); };

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<mmSingleRace, mmGameSingle>("mmSingleRace")
                .endClass();
        }
    };

    ASSERT_SIZEOF(mmSingleRace, 0x76F0);

    class mmSingleCircuit : public mmGameSingle
    {
    private:
        byte _buffer[0x7720 - sizeof(asNode) - 0x4];
    public:
        ANGEL_ALLOCATOR

        AGE_API mmSingleCircuit(void) {
            scoped_vtable x(this);
            ageHook::Thunk<0x41C4A0>::Call<void>(this);
        };

        virtual AGE_API ~mmSingleCircuit(void) {
            scoped_vtable x(this);
            ageHook::Thunk<0x41C4E0>::Call<void>(this);
        };

        /*
            asNode virtuals
        */

        virtual AGE_API void Update(void) override          { ageHook::Thunk<0x41CB80>::Call<void>(this); };
        virtual AGE_API void Reset(void) override           { ageHook::Thunk<0x41CA50>::Call<void>(this); };

        /*
            mmGame overrides
        */

        virtual AGE_API int Init() override                 { return ageHook::Thunk<0x41C590>::Call<int>(this); };
        virtual AGE_API void InitMyPlayer() override        { ageHook::Thunk<0x41C6B0>::Call<void>(this); };
        virtual AGE_API void InitOtherPlayers() override    { ageHook::Thunk<0x41CA40>::Call<void>(this); };
        virtual AGE_API void InitGameObjects() override     { ageHook::Thunk<0x41C770>::Call<void>(this); };
        virtual AGE_API void InitHUD() override             { ageHook::Thunk<0x41C720>::Call<void>(this); };
        virtual AGE_API void UpdateGameInput(int a1) override 
                                                            { ageHook::Thunk<0x41CB90>::Call<void>(this, a1); }
        virtual AGE_API void UpdateDebugKeyInput(int a1) override
                                                            { ageHook::Thunk<0x41F970>::Call<void>(this, a1); }
        virtual AGE_API void UpdateGame() override          { ageHook::Thunk<0x41CEE0>::Call<void>(this); }
        virtual AGE_API void NextRace() override            { ageHook::Thunk<0x41DBC0>::Call<void>(this); };
        virtual AGE_API void HitWaterHandler() override     { ageHook::Thunk<0x41CAA0>::Call<void>(this); };
        virtual AGE_API void SwitchState(int a1) override   { ageHook::Thunk<0x41D690>::Call<void>(this, a1); };
        virtual AGE_API void* GetWaypoints() override       { return ageHook::Thunk<0x41DF20>::Call<void*>(this); };

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<mmSingleCircuit, mmGameSingle>("mmSingleCircuit")
                .endClass();
        }
    };

    ASSERT_SIZEOF(mmSingleCircuit, 0x7720);

    class mmSingleBlitz : public mmGameSingle
    {
    private:
        byte _buffer[0x7710 - sizeof(asNode) - 0x4];
    public:
        ANGEL_ALLOCATOR

        AGE_API mmSingleBlitz(void) {
            scoped_vtable x(this);
            ageHook::Thunk<0x41AC00>::Call<void>(this);
        };

        virtual AGE_API ~mmSingleBlitz(void) {
            scoped_vtable x(this);
            ageHook::Thunk<0x41AC70>::Call<void>(this);
        };

        /*
            asNode virtuals
        */

        virtual AGE_API void Update(void) override          { ageHook::Thunk<0x41B460>::Call<void>(this); };
        virtual AGE_API void Reset(void) override           { ageHook::Thunk<0x41B370>::Call<void>(this); };

        /*
            mmGame overrides
        */

        virtual AGE_API int Init() override                 { return ageHook::Thunk<0x41AD30>::Call<int>(this); };
        virtual AGE_API void InitMyPlayer() override        { ageHook::Thunk<0x41AE50>::Call<void>(this); };
        virtual AGE_API void InitOtherPlayers() override    { ageHook::Thunk<0x41B360>::Call<void>(this); };
        virtual AGE_API void InitGameObjects() override     { ageHook::Thunk<0x41B0D0>::Call<void>(this); };
        virtual AGE_API void InitHUD() override             { ageHook::Thunk<0x41AEC0>::Call<void>(this); };
        virtual AGE_API void UpdateGameInput(int a1) override 
                                                            { ageHook::Thunk<0x41B590>::Call<void>(this, a1); }
        virtual AGE_API void UpdateDebugKeyInput(int a1) override
                                                            { ageHook::Thunk<0x41C480>::Call<void>(this, a1); }
        virtual AGE_API void UpdateGame() override          { ageHook::Thunk<0x41B5D0>::Call<void>(this); }
        virtual AGE_API void NextRace() override            { ageHook::Thunk<0x41C080>::Call<void>(this); };
        virtual AGE_API void HitWaterHandler() override     { ageHook::Thunk<0x41B400>::Call<void>(this); };
        virtual AGE_API void SwitchState(int a1) override   { ageHook::Thunk<0x41BDB0>::Call<void>(this, a1); };
        virtual AGE_API void* GetWaypoints() override       { return ageHook::Thunk<0x41C490>::Call<void*>(this); };

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<mmSingleBlitz, mmGameSingle>("mmSingleBlitz")
                .endClass();
        }
    };

    ASSERT_SIZEOF(mmSingleBlitz, 0x7710);

    class mmSingleStunt : public mmGameSingle
    {
    private:
        byte _buffer[0x7970 - sizeof(asNode) - 0x4];
    public:
        ANGEL_ALLOCATOR

        AGE_API mmSingleStunt(void) {
            scoped_vtable x(this);
            ageHook::Thunk<0x415B00>::Call<void>(this);
        };

        virtual AGE_API ~mmSingleStunt(void) {
            scoped_vtable x(this);
            ageHook::Thunk<0x415BC0>::Call<void>(this);
        };

        /*
            asNode virtuals
        */

        virtual AGE_API void Update(void) override          { ageHook::Thunk<0x416970>::Call<void>(this); };
        virtual AGE_API void Reset(void) override           { ageHook::Thunk<0x416760>::Call<void>(this); };

        /*
            mmGame overrides
        */

        virtual AGE_API int Init() override                 { return ageHook::Thunk<0x415C40>::Call<int>(this); };
        virtual AGE_API void InitMyPlayer() override        { ageHook::Thunk<0x415D40>::Call<void>(this); };
        virtual AGE_API void InitOtherPlayers() override    { ageHook::Thunk<0x416720>::Call<void>(this); };
        virtual AGE_API void InitGameObjects() override     { ageHook::Thunk<0x416450>::Call<void>(this); };
        virtual AGE_API void InitHUD() override             { ageHook::Thunk<0x415DB0>::Call<void>(this); };
        virtual AGE_API void UpdateGameInput(int a1) override 
                                                            { ageHook::Thunk<0x416980>::Call<void>(this, a1); }
        virtual AGE_API void UpdateDebugKeyInput(int a1) override
                                                            { ageHook::Thunk<0x41ABF0>::Call<void>(this, a1); }
        virtual AGE_API void UpdateGame() override          { ageHook::Thunk<0x416990>::Call<void>(this); }
        virtual AGE_API void NextRace() override            { ageHook::Thunk<0x41A490>::Call<void>(this); };
        virtual AGE_API void HitWaterHandler() override     { ageHook::Thunk<0x416920>::Call<void>(this); };
        virtual AGE_API void SwitchState(int a1) override   { ageHook::Thunk<0x41A460>::Call<void>(this, a1); };

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<mmSingleStunt, mmGameSingle>("mmSingleStunt")
                .endClass();
        }
    };

    ASSERT_SIZEOF(mmSingleStunt, 0x7970);

    class mmMultiRoam : public mmGameMulti
    {
    private:
        byte _buffer[0xB208 - sizeof(asNode) - 0x4];
    public:
        ANGEL_ALLOCATOR

        AGE_API mmMultiRoam(void) {
            scoped_vtable x(this);
            ageHook::Thunk<0x427530>::Call<void>(this);
        };

        virtual AGE_API ~mmMultiRoam(void) {
            scoped_vtable x(this);
            ageHook::Thunk<0x427560>::Call<void>(this);
        };

        /*
            asNode virtuals
        */

        virtual AGE_API void Reset(void) override           { ageHook::Thunk<0x427C50>::Call<void>(this); };

        /*
            mmGame overrides
        */

        virtual AGE_API int Init() override                 { return ageHook::Thunk<0x427600>::Call<int>(this); };
        virtual AGE_API void InitMyPlayer() override        { ageHook::Thunk<0x427700>::Call<void>(this); };
        virtual AGE_API void InitGameObjects() override     { ageHook::Thunk<0x4277A0>::Call<void>(this); };
        virtual AGE_API void InitHUD() override             { ageHook::Thunk<0x427770>::Call<void>(this); };
        virtual AGE_API void UpdateGameInput(int a1) override 
                                                            { ageHook::Thunk<0x427CC0>::Call<void>(this, a1); }
        virtual AGE_API void UpdateGame() override          { ageHook::Thunk<0x427CD0>::Call<void>(this); }
        virtual AGE_API void SwitchState(int a1) override   { ageHook::Thunk<0x427EB0>::Call<void>(this, a1); };
        virtual AGE_API void* GetWaypoints() override       { return ageHook::Thunk<0x428250>::Call<void*>(this); };

        /*
            mmGameMulti overrides
        */
        virtual AGE_API void InitNetworkPlayers() override  { ageHook::Thunk<0x427880>::Call<void>(this); };
        virtual AGE_API void SystemMessage(void* msg) override 
                                                            { ageHook::Thunk<0x427EC0>::Call<void>(this, msg); };
        virtual AGE_API void GameMessage(void* msg) override 
                                                            { ageHook::Thunk<0x428050>::Call<void>(this, msg); };

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<mmMultiRoam, mmGameMulti>("mmMultiRoam")
                .endClass();
        }
    };

    ASSERT_SIZEOF(mmMultiRoam, 0xB208);

    class mmMultiRace : public mmGameMulti
    {
    private:
        byte _buffer[0xB208 - sizeof(asNode) - 0x4];
    public:
        ANGEL_ALLOCATOR

        AGE_API mmMultiRace(void) {
            scoped_vtable x(this);
            ageHook::Thunk<0x428260>::Call<void>(this);
        };

        virtual AGE_API ~mmMultiRace(void) {
            scoped_vtable x(this);
            ageHook::Thunk<0x4282A0>::Call<void>(this);
        };

        /*
            asNode virtuals
        */

        virtual AGE_API void Reset(void) override           { ageHook::Thunk<0x428AD0>::Call<void>(this); };

        /*
            mmGame overrides
        */

        virtual AGE_API int Init() override                 { return ageHook::Thunk<0x428340>::Call<int>(this); };
        virtual AGE_API void InitMyPlayer() override        { ageHook::Thunk<0x428430>::Call<void>(this); };
        virtual AGE_API void InitGameObjects() override     { ageHook::Thunk<0x4284F0>::Call<void>(this); };
        virtual AGE_API void InitHUD() override             { ageHook::Thunk<0x4284A0>::Call<void>(this); };
        virtual AGE_API void UpdateGameInput(int a1) override 
                                                            { ageHook::Thunk<0x428BA0>::Call<void>(this, a1); }
        virtual AGE_API void UpdateGame() override          { ageHook::Thunk<0x428BE0>::Call<void>(this); }
        virtual AGE_API void SwitchState(int a1) override   { ageHook::Thunk<0x429440>::Call<void>(this, a1); };
        virtual AGE_API void* GetWaypoints() override       { return ageHook::Thunk<0x429D80>::Call<void*>(this); };

        /*
            mmGameMulti overrides
        */
        virtual AGE_API void InitNetworkPlayers() override  { ageHook::Thunk<0x428700>::Call<void>(this); };
        virtual AGE_API void SystemMessage(void* msg) override 
                                                            { ageHook::Thunk<0x429450>::Call<void>(this, msg); };
        virtual AGE_API void GameMessage(void* msg) override 
                                                            { ageHook::Thunk<0x429560>::Call<void>(this, msg); };

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<mmMultiRace, mmGameMulti>("mmMultiRace")
                .endClass();
        }
    };

    ASSERT_SIZEOF(mmMultiRace, 0xB208);

    class mmMultiCR : public mmGameMulti
    {
    private:
        byte _buffer[0xB2A8 - sizeof(asNode) - 0x4];
    public:
        ANGEL_ALLOCATOR

        AGE_API mmMultiCR(void) {
            scoped_vtable x(this);
            ageHook::Thunk<0x4236E0>::Call<void>(this);
        };

        virtual AGE_API ~mmMultiCR(void) {
            scoped_vtable x(this);
            ageHook::Thunk<0x4237C0>::Call<void>(this);
        };

        /*
            asNode virtuals
        */

        virtual AGE_API void Reset(void) override           { ageHook::Thunk<0x424440>::Call<void>(this); };

        /*
            mmGame overrides
        */

        virtual AGE_API int Init() override                 { return ageHook::Thunk<0x423870>::Call<int>(this); };
        virtual AGE_API void InitMyPlayer() override        { ageHook::Thunk<0x4239F0>::Call<void>(this); };
        virtual AGE_API void InitGameObjects() override     { ageHook::Thunk<0x423C80>::Call<void>(this); };
        virtual AGE_API void InitHUD() override             { ageHook::Thunk<0x423B00>::Call<void>(this); };
        virtual AGE_API void UpdateGameInput(int a1) override 
                                                            { ageHook::Thunk<0x424F80>::Call<void>(this, a1); }
        virtual AGE_API void UpdateDebugKeyInput(int a1) override 
                                                            { ageHook::Thunk<0x4272D0>::Call<void>(this, a1); }
        virtual AGE_API void UpdateGame() override          { ageHook::Thunk<0x424F90>::Call<void>(this); }
        virtual AGE_API void HitWaterHandler() override     { ageHook::Thunk<0x4253A0>::Call<void>(this); }
        virtual AGE_API void DropThruCityHandler() override { ageHook::Thunk<0x425460>::Call<void>(this); }
        virtual AGE_API void SwitchState(int a1) override   { ageHook::Thunk<0x425390>::Call<void>(this, a1); };
        virtual AGE_API void* GetWaypoints() override       { return ageHook::Thunk<0x427510>::Call<void*>(this); };

        /*
            mmGameMulti overrides
        */
        virtual AGE_API void InitNetworkPlayers() override  { ageHook::Thunk<0x424090>::Call<void>(this); };
        virtual AGE_API void SystemMessage(void* msg) override 
                                                            { ageHook::Thunk<0x4261B0>::Call<void>(this, msg); };
        virtual AGE_API void GameMessage(void* msg) override 
                                                            { ageHook::Thunk<0x426340>::Call<void>(this, msg); };

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<mmMultiCR, mmGameMulti>("mmMultiCR")
                .endClass();
        }
    };

    ASSERT_SIZEOF(mmMultiCR, 0xB2A8);

    class mmMultiCircuit : public mmGameMulti
    {
    private:
        byte _buffer[0xB208 - sizeof(asNode) - 0x4];
    public:
        ANGEL_ALLOCATOR

        AGE_API mmMultiCircuit(void) {
            scoped_vtable x(this);
            ageHook::Thunk<0x421B90>::Call<void>(this);
        };

        virtual AGE_API ~mmMultiCircuit(void) {
            scoped_vtable x(this);
            ageHook::Thunk<0x421BC0>::Call<void>(this);
        };

        /*
            asNode virtuals
        */

        virtual AGE_API void Reset(void) override           { ageHook::Thunk<0x422470>::Call<void>(this); };

        /*
            mmGame overrides
        */

        virtual AGE_API int Init() override                 { return ageHook::Thunk<0x421C60>::Call<int>(this); };
        virtual AGE_API void InitMyPlayer() override        { ageHook::Thunk<0x421DD0>::Call<void>(this); };
        virtual AGE_API void InitGameObjects() override     { ageHook::Thunk<0x421E90>::Call<void>(this); };
        virtual AGE_API void InitHUD() override             { ageHook::Thunk<0x421E40>::Call<void>(this); };
        virtual AGE_API void UpdateGameInput(int a1) override 
                                                            { ageHook::Thunk<0x422530>::Call<void>(this, a1); }
        virtual AGE_API void UpdateDebugKeyInput(int a1) override 
                                                            { ageHook::Thunk<0x422DA0>::Call<void>(this, a1); }
        virtual AGE_API void UpdateGame() override          { ageHook::Thunk<0x422570>::Call<void>(this); }
        virtual AGE_API void SwitchState(int a1) override   { ageHook::Thunk<0x422D90>::Call<void>(this, a1); };
        virtual AGE_API void* GetWaypoints() override       { return ageHook::Thunk<0x4236A0>::Call<void*>(this); };

        /*
            mmGameMulti overrides
        */
        virtual AGE_API void InitNetworkPlayers() override  { ageHook::Thunk<0x4220A0>::Call<void>(this); };
        virtual AGE_API void SystemMessage(void* msg) override 
                                                            { ageHook::Thunk<0x422DB0>::Call<void>(this, msg); };
        virtual AGE_API void GameMessage(void* msg) override 
                                                            { ageHook::Thunk<0x422EC0>::Call<void>(this, msg); };

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<mmMultiCircuit, mmGameMulti>("mmMultiCircuit")
                .endClass();
        }
    };

    ASSERT_SIZEOF(mmMultiCircuit, 0xB208);

    class mmMultiBlitz : public mmGameMulti
    {
    private:
        byte _buffer[0xB218 - sizeof(asNode) - 0x4];
    public:
        ANGEL_ALLOCATOR

        AGE_API mmMultiBlitz(void) {
            scoped_vtable x(this);
            ageHook::Thunk<0x41FF30>::Call<void>(this);
        };

        virtual AGE_API ~mmMultiBlitz(void) {
            scoped_vtable x(this);
            ageHook::Thunk<0x41FF60>::Call<void>(this);
        };

        /*
            asNode virtuals
        */

        virtual AGE_API void Reset(void) override           { ageHook::Thunk<0x420880>::Call<void>(this); };

        /*
            mmGame overrides
        */

        virtual AGE_API int Init() override                 { return ageHook::Thunk<0x420000>::Call<int>(this); };
        virtual AGE_API void InitMyPlayer() override        { ageHook::Thunk<0x420130>::Call<void>(this); };
        virtual AGE_API void InitGameObjects() override     { ageHook::Thunk<0x420220>::Call<void>(this); };
        virtual AGE_API void InitHUD() override             { ageHook::Thunk<0x4201A0>::Call<void>(this); };
        virtual AGE_API void UpdateGameInput(int a1) override 
                                                            { ageHook::Thunk<0x420940>::Call<void>(this, a1); }
        virtual AGE_API void UpdateGame() override          { ageHook::Thunk<0x420980>::Call<void>(this); }
        virtual AGE_API void SwitchState(int a1) override   { ageHook::Thunk<0x421250>::Call<void>(this, a1); };
        virtual AGE_API void* GetWaypoints() override       { return ageHook::Thunk<0x421B80>::Call<void*>(this); };

        /*
            mmGameMulti overrides
        */
        virtual AGE_API void InitNetworkPlayers() override  { ageHook::Thunk<0x4204B0>::Call<void>(this); };
        virtual AGE_API void SystemMessage(void* msg) override 
                                                            { ageHook::Thunk<0x421260>::Call<void>(this, msg); };
        virtual AGE_API void GameMessage(void* msg) override 
                                                            { ageHook::Thunk<0x421370>::Call<void>(this, msg); };

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<mmMultiBlitz, mmGameMulti>("mmMultiBlitz")
                .endClass();
        }
    };

    ASSERT_SIZEOF(mmMultiBlitz, 0xB218);

    class mmGameManager: public asNode {
    private:
        asCamera m_Camera;
        mmGame *m_CurrentGame;
        mmSingleRoam *m_SingleRoamGame;
        mmSingleRace *m_SingleRaceGame;
        mmSingleStunt *m_SingleStuntGame;
        mmSingleCircuit *m_SingleCircuitGame;
        mmSingleBlitz *m_SingleBlitzGame;
        mmMultiRace *m_MultiRaceGame;
        mmMultiRoam* m_MultiRoamGame;
        mmMultiCR* m_MultiCRGame;
        mmMultiCircuit* m_MultiCircuitGame;
        mmMultiBlitz* m_MultiBlitzGame;
        int NumUpdateCallsSinceReset;
    public:
        inline mmGame* getGame(void) const {
            return this->m_CurrentGame;
        };

        inline mmPlayer* getPlayerSafe(void) const {
            auto game = this->getGame();
            if (game == nullptr)
                return nullptr;
            auto player = game->getPlayer();
            return player;
        }
    public:
        ANGEL_ALLOCATOR
        void* operator new (std::size_t, void* p) throw() { return p; }

        static ageHook::Type<mmGameManager *> Instance;

        AGE_API mmGameManager(void)  {
            scoped_vtable x(this);
            ageHook::Thunk<0x4029E0>::Call<void>(this);
        }

        virtual AGE_API ~mmGameManager(void) {
            scoped_vtable x(this);
            ageHook::Thunk<0x402E40>::Call<void>(this);
        }

        /*
            asNode virtuals
        */
        AGE_API void Cull()  override                       { ageHook::Thunk<0x4031D0>::Call<void>(this); }
        AGE_API void Update()  override                     { ageHook::Thunk<0x403000>::Call<void>(this); }
        AGE_API void Reset()  override                      { ageHook::Thunk<0x402E30>::Call<void>(this); }

        /* TODO?
        mmGameManager::mmGameManager(void)
        mmGameManager::~mmGameManager(void)

        void mmGameManager::BeDone(void)
        void mmGameManager::ForcePopupUI(void)
        void mmGameManager::ForceReplayUI(void)
        */

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<mmGameManager, asNode>("mmGameManager")
                //properties
                .addPropertyReadOnly("Game", &getGame)
                
                //statics
                .addStaticFunction("Instance", [] {return (mmGameManager *)Instance; })
            .endClass();
        }
    };

    ASSERT_SIZEOF(mmGameManager, 0x1B8);

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
        byte _buffer[0x50];
    public:
        //lua helpers
        inline void luaClearInterest(void) {
            this->SetInterest(nullptr);
        }

        /*
            asNode virtuals
        */
        AGE_API void Reset() override                       { ageHook::Thunk<0x42E780>::Call<void>(this); }
        AGE_API void Update() override                      { ageHook::Thunk<0x42E7F0>::Call<void>(this); }

        /*
            mmArrow
        */
        AGE_API void SetInterest(Vector3 *interestPoint)    { ageHook::Thunk<0x42E790>::Call<void>(this, interestPoint); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<mmArrow, asNode>("mmArrow")
                .addFunction("SetInterest", &SetInterest)
                .addFunction("ClearInterest", &luaClearInterest)
                .endClass();
        }
                                                            
    };
    ASSERT_SIZEOF(mmArrow, 0x68);

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

        /*
            asNode virtuals
        */
        AGE_API void Update() override                      { ageHook::Thunk<0x42DBC0>::Call<void>(this); }
        AGE_API void UpdatePaused() override                { ageHook::Thunk<0x42DF10>::Call<void>(this); }
        AGE_API void Cull() override                        { ageHook::Thunk<0x42DF40>::Call<void>(this); }
        AGE_API void Reset() override                       { ageHook::Thunk<0x42DB20>::Call<void>(this); }
        AGE_API void ResChange(int width, int height) override
                                                            { ageHook::Thunk<0x42D2E0>::Call<void>(this, width, height); }

        /*
            mmHUD
        */
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