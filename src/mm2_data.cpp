#include "mm2_data.h"
using namespace MM2;

/*
    datArgParser
*/
MM2FnHook<bool> datArgParser::$Get_$1           ( NULL, NULL, 0x4C6190 );
MM2FnHook<bool> datArgParser::$Get_$2           ( NULL, NULL, 0x4C61C0 );
MM2FnHook<bool> datArgParser::$Get_$3           ( NULL, NULL, 0x4C6210 );
MM2FnHook<bool> datArgParser::$Get_$4           ( NULL, NULL, 0x4C6260 );

/*
    datAssetManager
*/
MM2FnHook<void> datAssetManager::$FullPath_$1   ( NULL, NULL, 0x4C55E0 );
MM2FnHook<void> datAssetManager::$FullPath_$2   ( NULL, NULL, 0x4C56F0 );
MM2FnHook<bool> datAssetManager::$Exists_$1     ( NULL, NULL, 0x4C59B0 );
MM2FnHook<bool> datAssetManager::$Exists_$2     ( NULL, NULL, 0x4C59E0 );

/*
    datOutput
*/
MM2FnHook<void> datOutput::$CloseLog            ( NULL, NULL, 0x4C9530 );
MM2FnHook<bool> datOutput::$OpenLog             ( NULL, NULL, 0x4C9590 );
MM2FnHook<void> datOutput::$SetOutputMask       ( NULL, NULL, 0x4C95A0 );

/*
    datTimeManager
*/
MM2FnHook<void> datTimeManager::$Reset          ( NULL, NULL, 0x4C6300 );
MM2FnHook<void> datTimeManager::$Update         ( NULL, NULL, 0x4C6340 );