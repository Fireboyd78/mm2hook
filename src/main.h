#pragma once

// MM2 uses DirectInput 7
#define DIRECTINPUT_VERSION 0x0700

#include "common.h"
#include "hook.h"
#include "patch.h"

#include "mm2.h"
#include "mm2_lua.h"

// dinput hooking
#include <dinput.h>

using namespace LuaIntf;

typedef HRESULT(WINAPI *LPFNDIRECTINPUTCREATE)(HINSTANCE, DWORD, LPVOID *, LPUNKNOWN);

EXTERN_C LPFNDIRECTINPUTCREATE lpDICreate;
EXTERN_C HRESULT DirectInputCreateA_Impl(HINSTANCE hinst, DWORD dwVersion, LPVOID *ppDI, LPUNKNOWN punkOuter);

LuaState L;