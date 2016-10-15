#include "mm2_game.h"
using namespace MM2;

/*
    mmGame
*/
DECLARE_HOOK(mmGame::$$ctor);
DECLARE_HOOK(mmGame::$$dtor);

DECLARE_HOOK(mmGame::$Update);
DECLARE_HOOK(mmGame::$UpdatePaused);
DECLARE_HOOK(mmGame::$Reset);

DECLARE_HOOK(mmGame::$Init);

DECLARE_HOOK(mmGame::$InitGameStrings);
DECLARE_HOOK(mmGame::$InitOtherPlayers);

DECLARE_HOOK(mmGame::$HitWaterHandler);

DECLARE_HOOK(mmGame::$DropThruCityHandler);
DECLARE_HOOK(mmGame::$SendChatMessage);

DECLARE_HOOK(mmGame::$BeDone);

/*
    mmGameManager
*/
DECLARE_HOOK(mmGameManager::$Instance);

/*
    mmGameMusicData
*/
DECLARE_HOOK(mmGameMusicData::$LoadAmbientSFX);

/*
    mmHUD
*/
DECLARE_HOOK(mmHUD::$SetMessage$1);
DECLARE_HOOK(mmHUD::$SetMessage$2);
DECLARE_HOOK(mmHUD::$PostChatMessage);

/*
    mmPopup
*/
DECLARE_HOOK(mmPopup::$IsEnabled);
DECLARE_HOOK(mmPopup::$Lock);
DECLARE_HOOK(mmPopup::$Unlock);
DECLARE_HOOK(mmPopup::$ProcessChat);