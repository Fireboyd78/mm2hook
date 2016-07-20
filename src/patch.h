#pragma once
#include "common.h"

/*
    Patching utilities/structs
*/

#define MAX_PATCHES_OPEN 32

struct PATCH_INFO {
    LPVOID lpPatchAddr;
    DWORD dwPatchSize;
    DWORD flOldProtect;
};

BOOL PushPatch(LPVOID lpHookAddr, DWORD dwPatchSize, PATCH_INFO *pPatch = NULL);
BOOL PopPatch(PATCH_INFO *pPatch = NULL);

void InstallVTHook(DWORD dwHookAddr, DWORD dwNewAddr);
void InstallVTHook(DWORD dwHookAddr, DWORD dwNewAddr, LPDWORD lpdwOldAddr);

bool InstallCallbackHook(DWORD dwAddress, DWORD fnDest, bool isCall);
bool InstallFunctionHook(DWORD dwAddress, DWORD jmpDest);

void InstallPatch(DWORD dwAddress, BYTE *patchData, DWORD dwSize);