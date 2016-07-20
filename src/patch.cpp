#include "patch.h"

int patchIdx = 0;
PATCH_INFO patches[MAX_PATCHES_OPEN];

BOOL PushPatch(LPVOID lpHookAddr, DWORD dwPatchSize, PATCH_INFO *pPatch)
{
    if ((patchIdx + 1) == MAX_PATCHES_OPEN)
    {
        MessageBox(NULL, "FATAL ERROR: Too many open patches! (patchIdx > MAX_PATCHES_OPEN)", "Error!", MB_OK | MB_ICONERROR);
        _exit(EXIT_FAILURE);
    }

    auto *patch = &patches[patchIdx++];
    patch->lpPatchAddr = lpHookAddr;
    patch->dwPatchSize = dwPatchSize;

    BOOL result = VirtualProtect(lpHookAddr, dwPatchSize, PAGE_EXECUTE_READWRITE, &(patch->flOldProtect));

    if (pPatch != NULL)
        memcpy(pPatch, patch, sizeof(PATCH_INFO));

    return result;
}

BOOL PopPatch(PATCH_INFO *pPatch)
{
    if (patchIdx == 0)
        return false;

    auto *patch = &patches[patchIdx--];

    if (pPatch != NULL)
        memcpy(pPatch, patch, sizeof(PATCH_INFO));

    BOOL result = VirtualProtect(patch->lpPatchAddr, patch->dwPatchSize, patch->flOldProtect, &(patch->flOldProtect));

    // reset patch
    patch->lpPatchAddr = nullptr;
    patch->dwPatchSize = 0;
    patch->flOldProtect = 0;

    return result;
}

void InstallVTHook(DWORD dwHookAddr, DWORD dwNewAddr) {
    PushPatch((LPVOID)dwHookAddr, 4);
    *(DWORD*)dwHookAddr = dwNewAddr;
    PopPatch();
}

void InstallVTHook(DWORD dwHookAddr, DWORD dwNewAddr, LPDWORD lpdwOldAddr) {
    *lpdwOldAddr = *(DWORD*)dwHookAddr;
    InstallVTHook(dwHookAddr, dwNewAddr);
}

bool InstallCallbackHook(DWORD dwAddress, DWORD fnDest, bool isCall) {
    const LPVOID lpAddr = (LPVOID)dwAddress;
    const DWORD opSize = 5;

    int relOffset = dwAddress + opSize;
    int relFnDest = (int)(fnDest - relOffset);

    // hopefully this is good enough error checking
    // otherwise we're going to have some serious problems!
    if ((relOffset + relFnDest) != fnDest)
        return false;

    PushPatch(lpAddr, opSize);
    *(BYTE*)lpAddr = (isCall) ? 0xE8 : 0xE9;
    *(int*)((BYTE*)lpAddr + 1) = relFnDest;
    PopPatch();

    return true;
}

bool InstallFunctionHook(DWORD dwAddress, DWORD fnDest) {
    const LPVOID lpAddr = (LPVOID)dwAddress;

    bool absolute = false;
    BYTE op = *(BYTE*)lpAddr;

    if (op == 0xFF)
    {
        BYTE type = *((BYTE*)lpAddr + 1) & 0xF;

        // unknown/unsupported type
        if (type != 5)
            return false;

        absolute = true;
    }
    else if (op != 0xE8 || op != 0xE9)
    {
        // unknown/invalid
        return false;
    }

    if (absolute)
    {
        // we need some new memory to hold our pointer
        // TODO: possibly encapsulate all new pointers into a global pool?
        DWORD *lpNewPointer = new DWORD;

        // set new pointer to our function
        *lpNewPointer = fnDest;

        // NOTE: the old pointer will be left dangling...do we ever need that data again?
        PushPatch(lpAddr, 6);
        *(DWORD**)((BYTE*)lpAddr + 2) = lpNewPointer;
        PopPatch();
    }
    else
    {
        const DWORD opSize = 5;

        int relOffset = dwAddress + opSize;
        int relFnDest = (int)(fnDest - relOffset);

        // hopefully this is good enough error checking
        // otherwise we're going to have some serious problems!
        if ((relOffset + relFnDest) != fnDest)
            return false;

        PushPatch(lpAddr, opSize);
        *(int*)((BYTE*)lpAddr + 1) = relFnDest;
        PopPatch();
    }

    return true;
}

void InstallPatch(DWORD dwAddress, BYTE *patchData, DWORD dwSize) {
    PushPatch((LPVOID)dwAddress, dwSize);
    memcpy((LPVOID)dwAddress, patchData, dwSize);
    PopPatch();
}