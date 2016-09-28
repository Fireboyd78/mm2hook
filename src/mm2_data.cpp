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
    datCallback
*/
MM2FnHook<void> datCallback::$$ctor$void        ( NULL, NULL, 0x4C7A40 );

MM2FnHook<void> datCallback::$$ctor$ThisCB_$1   ( NULL, NULL, 0x4C7A50 );
MM2FnHook<void> datCallback::$$ctor$ThisCB_$2   ( NULL, NULL, 0x4C7A80 );
MM2FnHook<void> datCallback::$$ctor$ThisCB_$3   ( NULL, NULL, 0x4C7AA0 );

MM2FnHook<void> datCallback::$$ctor$CB_$1       ( NULL, NULL, 0x4C7AC0 );
MM2FnHook<void> datCallback::$$ctor$CB_$2       ( NULL, NULL, 0x4C7AF0 );
MM2FnHook<void> datCallback::$$ctor$CB_$3       ( NULL, NULL, 0x4C7B20 );

MM2FnHook<void> datCallback::$Call              ( NULL, NULL, 0x4C7B50 );

/*
    datOutput
*/
MM2FnHook<void> datOutput::$CloseLog            ( NULL, NULL, 0x4C9530 );
MM2FnHook<bool> datOutput::$OpenLog             ( NULL, NULL, 0x4C9590 );
MM2FnHook<void> datOutput::$SetOutputMask       ( NULL, NULL, 0x4C95A0 );

/*
    datParser
*/
MM2FnHook<void> datParser::$$ctor               ( NULL, NULL, 0x4A78E0 );
MM2FnHook<void> datParser::$$dtor               ( NULL, NULL, 0x4A7920 );

MM2FnHook<datParser *>
                datParser::$AddParser           ( NULL, NULL, 0x4A7980 );

MM2FnHook<datParserRecord &> 
                datParser::$AddRecord           ( NULL, NULL, 0x4A79D0 );

MM2FnHook<bool> datParser::$Load_$1             ( NULL, NULL, 0x4A7A90 );
MM2FnHook<bool> datParser::$Load_$2             ( NULL, NULL, 0x4A7AF0 );
MM2FnHook<bool> datParser::$Load_$3             ( NULL, NULL, 0x4A7B40 );

MM2FnHook<bool> datParser::$Save_$1             ( NULL, NULL, 0x4A7B90 );
MM2FnHook<bool> datParser::$Save_$2             ( NULL, NULL, 0x4A7C00 );
MM2FnHook<bool> datParser::$Save_$3             ( NULL, NULL, 0x4A7C50 );

/*
    datTimeManager
*/
MM2FnHook<void> datTimeManager::$Reset          ( NULL, NULL, 0x4C6300 );
MM2FnHook<void> datTimeManager::$Update         ( NULL, NULL, 0x4C6340 );