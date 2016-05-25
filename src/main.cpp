#include "main.h"

LPFNDIRECTINPUTCREATE lpDICreate;

// Export as 'DirectInputCreateA' so we can hook into MM2
#pragma comment(linker, "/EXPORT:DirectInputCreateA=_DirectInputCreateA_Impl")
HRESULT NAKED DirectInputCreateA_Impl(HINSTANCE hinst, DWORD dwVersion, LPVOID *ppDI, LPUNKNOWN punkOuter)
{
    _asm jmp DWORD PTR ds:lpDICreate
}

CMidtownMadness2 *pMM2;

WNDPROC hProcOld;
LRESULT APIENTRY WndProcNew(HWND, UINT, WPARAM, LPARAM);

void InstallVTHook(DWORD dwHookAddr, DWORD dwNewAddr) {
    DWORD flOldProtect;

    VirtualProtect((LPVOID)dwHookAddr, 4, PAGE_EXECUTE_READWRITE, &flOldProtect);
    *(DWORD*)dwHookAddr = dwNewAddr;
    VirtualProtect((LPVOID)dwHookAddr, 4, flOldProtect, &flOldProtect);
}

void InstallVTHook(DWORD dwHookAddr, DWORD dwNewAddr, LPDWORD lpdwOldAddr) {
    *lpdwOldAddr = *(DWORD*)dwHookAddr;
    InstallVTHook(dwHookAddr, dwNewAddr);
}

void InstallPatch(DWORD dwAddress, BYTE *patchData, DWORD dwSize) {
    DWORD flOldProtect;

    VirtualProtect((LPVOID)dwAddress, dwSize, PAGE_EXECUTE_READWRITE, &flOldProtect);
    memcpy((LPVOID)dwAddress, patchData, dwSize);
    VirtualProtect((LPVOID)dwAddress, dwSize, flOldProtect, &flOldProtect);
};

void SetJmpDest(LPVOID patch, DWORD jmpOffset, LPVOID jmpDest, LPDWORD jmpStorage) {
    *jmpStorage = (DWORD)jmpDest;
    *(DWORD*)((BYTE*)patch + jmpOffset) = (DWORD)jmpStorage;
};

void InstallJmpHook(DWORD dwAddress, DWORD jmpDest) {
    const LPVOID lpAddr = (LPVOID)dwAddress;
    const DWORD dwSize = 5; // size of jmp instruction
    DWORD relOffset = dwAddress + dwSize;

    DWORD flOldProtect;

    VirtualProtect(lpAddr, dwSize, PAGE_EXECUTE_READWRITE, &flOldProtect);

    // This can be extremely fatal if the target is too far away!
    // Should probably implement some error checking...
    *(BYTE*)lpAddr = 0xE9; // 'JMP REL32'
    *(int*)((BYTE*)lpAddr + 1) = (int)(jmpDest - relOffset);

    VirtualProtect(lpAddr, dwSize, flOldProtect, &flOldProtect);
};

bool SubclassGameWindow(HWND gameWnd, WNDPROC pWndProcNew, WNDPROC *ppWndProcOld)
{
    if (gameWnd != NULL)
    {
        WNDPROC hProcOld = (WNDPROC)GetWindowLong(gameWnd, GWL_WNDPROC);

        *ppWndProcOld = hProcOld;

        if (hProcOld != NULL && SetWindowLong(gameWnd, GWL_WNDPROC, (LONG)pWndProcNew))
            return true;
    }
    return false;
}

bool HandleKeyPress(DWORD vKey)
{
    switch (vKey) {
        // '~'
        case VK_OEM_3:
        {
            // close the game!
            //ExitProcess(EXIT_SUCCESS);
            //MM2::Quitf("Bye bye from %s!", "MM2Hook");

            MM2FnHook<void> lpmmPopup_ProcessChat{ MM2_RETAIL, 0x42A400 };

            auto mgr = *MM2::mmGameManager::Instance();
            auto srPtr = *(DWORD*)((BYTE*)mgr + 0x188);

            LogFile::Format("mgr: %08X, srPtr: %08X", mgr, srPtr);

            if (srPtr == NULL) {
                LogFile::Format("\n");
            } else {
                auto popup = *(DWORD*)((BYTE*)srPtr + 0x94);

                LogFile::Format(", popup: %08X\n", popup);

                if (popup != NULL) {
                    (*(void(__thiscall *)(THIS_ LPVOID))lpmmPopup_ProcessChat.ptr())((LPVOID)popup);
                }
            }

        } return true;
        case 'f':
        case 'F':
            MM2::Stream::DumpOpenFiles();
            return true;
    }
    return false;
}

LRESULT APIENTRY WndProcNew(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
        case WM_KEYUP:
        case WM_SYSKEYUP:
        {
            if (HandleKeyPress((DWORD)wParam))
                return 0;
        } break;
    }
    return CallWindowProc(hProcOld, hWnd, uMsg, wParam, lParam);
}

bool isHookSetup = false;

void SetupHook() {
    LogFile::Write("Setting MM2 output log file...");

    if (MM2::datOutput::OpenLog("mm2.log"))
        LogFile::Write("Done!\n");
    else
        LogFile::Write("FAIL!\n");

    bool subclassWindow = true;

    if (subclassWindow)
    {
        LogFile::Write("Subclassing window...");

        if (SubclassGameWindow(pMM2->GetMainWindowHwnd(), WndProcNew, &hProcOld))
            LogFile::Write("Done!\n");
        else
            LogFile::Write("FAIL!\n");
    }

    isHookSetup = true;
}

void ResetHook(bool restarting) {
    LogFile::Write("Hook reset request received: ");
    LogFile::WriteLine((restarting) ? "leaving GameLoop" : "entering GameLoop");
    // TODO: reset stuff here?
}

PtrHook<BOOL> gameClosing;

// replaces VtResumeSampling
void HookStartup() {
    if (!gameClosing) {
        if (!isHookSetup)
        {
            LogFile::WriteLine("Hook startup request received.");
            SetupHook();
        }
        else
        {
            // GameLoop was restarted
            ResetHook(false);
        }
    } else {
        LogFile::WriteLine("WTF: Hook startup request received, but the game is closing!");
    }
};

// replaces VtPauseSampling
void HookShutdown() {
    if (gameClosing) {
        LogFile::WriteLine("Hook shutdown request received.");
        // TODO: shutdown stuff here?
    } else {
        // GameLoop is restarting
        ResetHook(true);
    }
};

bool HookFramework(MM2Version gameVersion) {
    switch (gameVersion)
    {
        case MM2_RETAIL:
        {
            *(DWORD*)0x5E0CC4 = (DWORD)&HookStartup;
            *(DWORD*)0x5E0CD8 = (DWORD)&HookShutdown;

            gameClosing = 0x6B1708;
        } return true;
    }
    return false;
};

void InstallPatches(MM2Version gameVersion) {
    switch (gameVersion)
    {
        case MM2_RETAIL:
        {
            
        } break;
    }
};

//
// Initialize all the important stuff prior to MM2 starting up
//
void Initialize(MM2Version gameVersion) {
    // first hook into the framework
    LogFile::Write("Hooking into the framework...");

    if (HookFramework(gameVersion)) {
        LogFile::Write("Done!\n");
    } else {
        LogFile::Write("FAIL!\n");
    }

    InstallPatches(gameVersion);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
        case DLL_PROCESS_ATTACH:
        {
            // Initialize the log file
            debug("initializing...");
            LogFile::Initialize("mm2hook.log", "--<< MM2Hook log file >>--\n");
            LogFile::WriteLine("MM2Hook initialized.");

            HMODULE hDIModule = NULL;
            AGEGameInfo gameInfo;

            LogFile::WriteLine("Checking for known MM2 versions...");

            if (CMidtownMadness2::GetGameInfo(gameInfo))
            {
                LogFile::Format(" - Detected game version %d\n", gameInfo.age_version);

                if (gameInfo.isSupported)
                {
                    if (LoadSystemLibrary("dinput.dll", &hDIModule) &&
                        GetHookProcAddress(hDIModule, "DirectInputCreateA", (FARPROC*)&lpDICreate))
                    {
                        // initialize game manager
                        pMM2 = new CMidtownMadness2(&gameInfo);

                        LogFile::WriteLine("Successfully injected into MM2!");
                        Initialize(pMM2->GetGameVersion());
                    }
                    else
                    {
                        LogFile::WriteLine("Failed to inject into MM2!");
                    }
                }
                else
                {
                    LogFile::WriteLine("Unsupported game version! Terminating...");
                    MessageBox(NULL, "Sorry, this version of MM2 is unsupported. Please remove MM2Hook to launch the game.", "MM2Hook", MB_OK | MB_ICONERROR);

                    ExitProcess(EXIT_FAILURE);
                }
            }
            else
            {
                LogFile::WriteLine("Unknown module detected! Terminating...");
                MessageBox(NULL, "Unknown module! MM2Hook will now terminate the process.", "MM2Hook", MB_OK | MB_ICONERROR);

                ExitProcess(EXIT_FAILURE);
            }
        } break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
        break;
	}
	return TRUE;
}

