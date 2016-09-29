#include "mm2_game.h"
using namespace MM2;

/*
    mmHUD
*/
MM2FnHook<void> mmHUD::$SetMessage                  ( NULL, NULL, 0x42E1F0 );
MM2FnHook<void> mmHUD::$SetMessage2                 ( NULL, NULL, 0x42E240 );
MM2FnHook<void> mmHUD::$PostChatMessage             ( NULL, NULL, 0x42D280 );

/*
    mmGame
*/
MM2FnHook<void> mmGame::$$ctor                      ( NULL, NULL, 0x412550 );
MM2FnHook<void> mmGame::$$dtor                      ( NULL, NULL, 0x413940 );

MM2FnHook<void> mmGame::$Update                     ( NULL, NULL, 0x413E90 );
MM2FnHook<void> mmGame::$UpdatePaused               ( NULL, NULL, 0x4144A0 );
MM2FnHook<void> mmGame::$Reset                      ( NULL, NULL, 0x413D40 );

MM2FnHook<int>  mmGame::$Init                       ( NULL, NULL, 0x412710 );

MM2FnHook<void> mmGame::$InitGameStrings            ( NULL, NULL, 0x413650 );
MM2FnHook<void> mmGame::$InitOtherPlayers           ( NULL, NULL, 0x4133F0 );

MM2FnHook<void> mmGame::$HitWaterHandler            ( NULL, NULL, 0x414290 );

MM2FnHook<void> mmGame::$DropThruCityHandler        ( NULL, NULL, 0x414280 );
MM2FnHook<void> mmGame::$SendChatMessage            ( NULL, NULL, 0x414E50 );

MM2FnHook<void> mmGame::$BeDone                     ( NULL, NULL, 0x414D30 );

/*
    mmGameManager
*/
MM2PtrHook<mmGameManager *>
                mmGameManager::$Instance            ( NULL, NULL, 0x5E0D08 );

/*
    mmGameMusicData
*/
MM2FnHook<bool> mmGameMusicData::$LoadAmbientSFX    ( NULL, NULL, 0x434060 );

/*
    mmPopup
*/
MM2FnHook<int>  mmPopup::$IsEnabled                 ( NULL, NULL, 0x42A280 );
MM2FnHook<void> mmPopup::$Lock                      ( NULL, NULL, 0x42B4F0 );
MM2FnHook<void> mmPopup::$Unlock                    ( NULL, NULL, 0x42B500 );
MM2FnHook<void> mmPopup::$ProcessChat               ( NULL, NULL, 0x42A400 );