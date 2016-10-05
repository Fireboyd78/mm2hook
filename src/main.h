#pragma once

#include "common.h"
#include "hook.h"
#include "patch.h"

#include "mm2.h"
#include "mm2_lua.h"

// MM2 uses DirectX 7
#define DIRECT3D_VERSION    0x0700
#define DIRECTINPUT_VERSION 0x0700
#include <d3d.h>
#include <dinput.h>

typedef HRESULT (WINAPI *LPFNDIRECTINPUTCREATE)(HINSTANCE, DWORD, LPVOID *, LPUNKNOWN);
typedef HRESULT (WINAPI *LPDIRECTDRAWCREATEEX)(GUID *, LPVOID *, REFIID, IUnknown *);

EXTERN_C LPFNDIRECTINPUTCREATE lpDICreate;
EXTERN_C HRESULT DirectInputCreateA_Impl(HINSTANCE hinst, DWORD dwVersion, LPVOID *ppDI, LPUNKNOWN punkOuter);

LuaState L;