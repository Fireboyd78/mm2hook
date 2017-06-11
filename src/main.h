#pragma once

#include "common.h"
#include "patch.h"

#include <DbgHelp.h>

#include "mm2.h"


LPFNDIRECTINPUTCREATE lpDICreate;

// Export as 'DirectInputCreateA/W' so we can hook into MM2
// (Apparently DirectInputCreateW gets called sometimes...)
#pragma comment(linker, "/EXPORT:DirectInputCreateA=_DirectInputCreate_Impl@16")
#pragma comment(linker, "/EXPORT:DirectInputCreateW=_DirectInputCreate_Impl@16")
STDAPI DirectInputCreate_Impl(HINSTANCE hinst, DWORD dwVersion, LPVOID *ppDI, LPUNKNOWN punkOuter) {
    return lpDICreate(hinst, dwVersion, ppDI, punkOuter);
}

CMidtownMadness2 *pMM2;
LuaState L;

bool isConsoleOpen = false;
int pedAttachCount = 0;

MM2::gfxTexture * redGlow = NULL;
MM2::gfxTexture * blueGlow = NULL;
float glowTimer = 0;
int glowindex = 0;
long lastMS = 0;
bool cycleInterest = false;