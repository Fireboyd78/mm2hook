#include "mm2_data.h"
using namespace MM2;

/*
    datArgParser
*/
DECLARE_HOOK(datArgParser::$Get_Any);
DECLARE_HOOK(datArgParser::$Get_Int);
DECLARE_HOOK(datArgParser::$Get_Float);
DECLARE_HOOK(datArgParser::$Get_String);
DECLARE_HOOK(datArgParser::$Get_WordCount);

/*
    datAssetManager
*/
DECLARE_HOOK(datAssetManager::$FullPath_$1);
DECLARE_HOOK(datAssetManager::$FullPath_$2);
DECLARE_HOOK(datAssetManager::$Exists_$1);
DECLARE_HOOK(datAssetManager::$Exists_$2);

/*
    datCallback
*/
DECLARE_HOOK(datCallback::$$ctor$void);

DECLARE_HOOK(datCallback::$$ctor$ThisCB_$1);
DECLARE_HOOK(datCallback::$$ctor$ThisCB_$2);
DECLARE_HOOK(datCallback::$$ctor$ThisCB_$3);

DECLARE_HOOK(datCallback::$$ctor$CB_$1);
DECLARE_HOOK(datCallback::$$ctor$CB_$2);
DECLARE_HOOK(datCallback::$$ctor$CB_$3);

DECLARE_HOOK(datCallback::$Call);

/*
    datOutput
*/
DECLARE_HOOK(datOutput::$CloseLog);
DECLARE_HOOK(datOutput::$OpenLog);
DECLARE_HOOK(datOutput::$SetOutputMask);

/*
    datParser
*/
DECLARE_HOOK(datParser::$$ctor);
DECLARE_HOOK(datParser::$$dtor);

DECLARE_HOOK(datParser::$AddParser);

DECLARE_HOOK(datParser::$AddRecord);

DECLARE_HOOK(datParser::$Load_$1);
DECLARE_HOOK(datParser::$Load_$2);
DECLARE_HOOK(datParser::$Load_$3);

DECLARE_HOOK(datParser::$Save_$1);
DECLARE_HOOK(datParser::$Save_$2);
DECLARE_HOOK(datParser::$Save_$3);

/*
    datTimeManager
*/
DECLARE_HOOK(datTimeManager::$Reset);
DECLARE_HOOK(datTimeManager::$Update);