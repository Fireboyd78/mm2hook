#include <modules\level.h>
using namespace MM2;

/*
    lvlLevel
*/
declfield(lvlLevel::Singleton)(0x655D9C);

/*
    dgPath
*/
LuaRef dgPath::sm_LuaCurrentCallback;
bool dgPath::sm_LuaCallbackSet = false;