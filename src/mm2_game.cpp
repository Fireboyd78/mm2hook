#include "mm2_game.h"
using namespace MM2;

MM2FnHook<void> mmHUD::$SetMessage                  ( NULL, NULL, 0x42E1F0 );
MM2FnHook<void> mmHUD::$SetMessage2                 ( NULL, NULL, 0x42E240 );
MM2FnHook<void> mmHUD::$PostChatMessage             ( NULL, NULL, 0x42D280 );

MM2PtrHook<mmGameManager *>
                mmGameManager::$Instance            ( NULL, NULL, 0x5E0D08 );

MM2FnHook<bool> mmGameMusicData::$LoadAmbientSFX    ( NULL, NULL, 0x434060 );

MM2FnHook<int>  mmPopup::$IsEnabled                 ( NULL, NULL, 0x42A280 );
MM2FnHook<void> mmPopup::$Lock                      ( NULL, NULL, 0x42B4F0 );
MM2FnHook<void> mmPopup::$Unlock                    ( NULL, NULL, 0x42B500 );
MM2FnHook<void> mmPopup::$ProcessChat               ( NULL, NULL, 0x42A400 );