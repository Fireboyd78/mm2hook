#include "mm2.h"

#define MAX_HOOK_PTRS 4096

using namespace MM2;

#define DEFINE_PRINT_HOOK(x,y) \
NAKED void x(LPCSTR str, ...) { \
    __asm jmp dword ptr ds:y \
}

MM2Version EngineVersionToGameVersion(int engineVersion) {
    switch (engineVersion) {
        case 3323:
            return MM2_BETA_1;
        case 3366:
            return MM2_BETA_2;
        case 3393:
            return MM2_RETAIL;
    }
    return MM2_INVALID;
}

static int g_hook_idx = 0; // next index to use
// global hook pool -- used to directly initialize pointers
static LPVOID g_hook_ptrs[MAX_HOOK_PTRS];

NOTHROW inline IMM2HookPtr::IMM2HookPtr(const MM2AddressData &addressData) {
    this->addressData = addressData;

    if (g_hook_idx < MAX_HOOK_PTRS) {
        g_hook_ptrs[g_hook_idx++] = this;
    }
    else {
        MessageBox(NULL, "FATAL ERROR: No more hook pointers available!", "MM2Hook", MB_OK | MB_ICONERROR);
    }
}

NOTHROW inline IMM2HookPtr::~IMM2HookPtr() {
    if (hook_idx > 0) {
        g_hook_ptrs[hook_idx] = NULL;
    }
}

inline void IMM2HookPtr::set_version(MM2Version gameVersion) {
    IHookPtr::operator=(addressData.addresses[gameVersion]);
}

#define INIT_DATA(b1,b2,r) ( MM2AddressData { b1, b2, r } )

MM2FnHook<void>     lpPrintf                                INIT_DATA( NULL, NULL, 0x4C9720 );
MM2FnHook<void>     lpMessagef                              INIT_DATA( NULL, NULL, 0x4C9750 );
MM2FnHook<void>     lpDisplayf                              INIT_DATA( NULL, NULL, 0x4C9780 );
MM2FnHook<void>     lpWarningf                              INIT_DATA( NULL, NULL, 0x4C97B0 );
MM2FnHook<void>     lpErrorf                                INIT_DATA( NULL, NULL, 0x4C97E0 );
MM2FnHook<void>     lpQuitf                                 INIT_DATA( NULL, NULL, 0x4C9810 );
MM2FnHook<void>     lpAbortf                                INIT_DATA( NULL, NULL, 0x4C9850 );

MM2FnHook<void>     lpaiMap_Dump                            INIT_DATA( NULL, NULL, 0x538840 );
MM2PtrHook<aiMap>   lpAIMAP_ptr                             INIT_DATA( NULL, NULL, 0x6B2E10 );

MM2FnHook<void>     lpdatOutput_CloseLog                    INIT_DATA( NULL, NULL, 0x4C9530 );
MM2FnHook<bool>     lpdatOutput_OpenLog                     INIT_DATA( NULL, NULL, 0x4C9590 );
MM2FnHook<void>     lpdatOutput_SetOutputMask               INIT_DATA( NULL, NULL, 0x4C95A0 );

MM2PtrHook<mmGameManager *>
                    lpmmGameManager_Instance                INIT_DATA( NULL, NULL, 0x5E0D08 );

MM2FnHook<int>      lpmmPopup_IsEnabled                     INIT_DATA( NULL, NULL, 0x42A280 );
MM2FnHook<void>     lpmmPopup_Lock                          INIT_DATA( NULL, NULL, 0x42B4F0 );
MM2FnHook<void>     lpmmPopup_Unlock                        INIT_DATA( NULL, NULL, 0x42B500 );
MM2FnHook<void>     lpmmPopup_ProcessChat                   INIT_DATA( NULL, NULL, 0x42A400 );

#ifdef IO_EVENT_HOOK
MM2FnHook<bool>     lpioEventQueue_Pop                      INIT_DATA( NULL, NULL, 0x4BA930 );
MM2FnHook<bool>     lpioEventQueue_Peek                     INIT_DATA( NULL, NULL, 0x4BA980 );
MM2FnHook<void>     lpioEventQueue_Queue                    INIT_DATA( NULL, NULL, 0x4BA9D0 );
MM2FnHook<void>     lpioEventQueue_Command                  INIT_DATA( NULL, NULL, 0x4BAA50 );
#endif

MM2FnHook<void>     lpStream_DumpOpenFiles                  INIT_DATA( NULL, NULL, 0x4C9970 );
MM2FnHook<Stream*>  lpStream_Open                           INIT_DATA( NULL, NULL, 0x4C99C0 );
MM2FnHook<Stream*>  lpStream_Create                         INIT_DATA( NULL, NULL, 0x4C9A00 );
MM2FnHook<int>      lpStream_Read                           INIT_DATA( NULL, NULL, 0x4C9AA0 );
MM2FnHook<int>      lpStream_Write                          INIT_DATA( NULL, NULL, 0x4C9BF0 );
MM2FnHook<int>      lpStream_GetCh                          INIT_DATA( NULL, NULL, 0x4C9D00 );
MM2FnHook<int>      lpStream_PutCh                          INIT_DATA( NULL, NULL, 0x4C9D30 );
MM2FnHook<int>      lpStream_Seek                           INIT_DATA( NULL, NULL, 0x4C9D60 );
MM2FnHook<int>      lpStream_Tell                           INIT_DATA( NULL, NULL, 0x4C9DB0 );
MM2FnHook<int>      lpStream_Close                          INIT_DATA( NULL, NULL, 0x4C9DC0 );
MM2FnHook<int>      lpStream_Size                           INIT_DATA( NULL, NULL, 0x4C9E00 );
MM2FnHook<int>      lpStream_Flush                          INIT_DATA( NULL, NULL, 0x4C9E60 );

int InitializeMM2(MM2Version gameVersion) {
    IMM2HookPtr *hook;
    int i = 0;

    LogFile::WriteLine("Initializing MM2 hooks...");

    while ((hook = (IMM2HookPtr*)g_hook_ptrs[i++]) != NULL)
        hook->set_version(gameVersion);

    LogFile::Format(" - nHooks: %d\n", i + 1);

    return (i > 0) ? HOOK_INIT_OK : HOOK_INIT_UNSUPPORTED;
}

CMidtownMadness2::CMidtownMadness2(int engineVersion)
    : CAGEGame(engineVersion) {
    this->m_gameVersion = EngineVersionToGameVersion(engineVersion);
    
    // try to initialize manager and hooks
    if (this->Initialize() != HOOK_INIT_OK ||
        InitializeMM2(this->m_gameVersion) != HOOK_INIT_OK) {
        LogFile::WriteLine("Failed to initialize the hook system! Terminating...");

        MessageBox(NULL, "A fatal error occurred while trying to initialize MM2Hook. Please remove MM2Hook to launch the game.", "MM2Hook", MB_OK | MB_ICONERROR);

        ExitProcess(EXIT_FAILURE);
    }
}

CMidtownMadness2::CMidtownMadness2(LPAGEGameInfo gameEntry)
    : CMidtownMadness2(gameEntry->age_version) {
}

namespace MM2 {
    DEFINE_PRINT_HOOK(Printf, lpPrintf);
    DEFINE_PRINT_HOOK(Messagef, lpMessagef);
    DEFINE_PRINT_HOOK(Displayf, lpDisplayf);
    DEFINE_PRINT_HOOK(Warningf, lpWarningf);
    DEFINE_PRINT_HOOK(Errorf, lpErrorf);
    DEFINE_PRINT_HOOK(Quitf, lpQuitf);
    DEFINE_PRINT_HOOK(Abortf, lpAbortf);

    void aiMap::Dump(THIS_ void) {
        lpaiMap_Dump.ThisCall(lpAIMAP_ptr);
    }

    bool datOutput::OpenLog(LPCSTR filename) {
        return lpdatOutput_OpenLog(filename);
    }

    void datOutput::CloseLog(void) {
        lpdatOutput_CloseLog();
    }

    void datOutput::SetOutputMask(UINT mask) {
        lpdatOutput_SetOutputMask(mask);
    }

#if IO_EVENT_HOOK
    bool ioEventQueue::Pop(ioEvent &outEvent) {
        return lpioEventQueue_Pop(&outEvent);
    }

    bool ioEventQueue::Peek(ioEvent &outEvent, int &idx) {
        return lpioEventQueue_Peek(&outEvent, &idx);
    }

    void ioEventQueue::Queue(ioEvent::ioEventType type, int x, int y, int value) {
        lpioEventQueue_Queue(type, x, y, value);
    }

    void ioEventQueue::Command(void *command) {
        lpioEventQueue_Command(command);
    }
#endif

    MM2PtrHook<mmGameManager*> mmGameManager::Instance(void) {
        return lpmmGameManager_Instance;
    }

    int mmPopup::IsEnabled(THIS_ void) {
        return lpmmPopup_IsEnabled.ThisCall(this);
    }

    void mmPopup::Lock(THIS_ void) {
        lpmmPopup_Lock.ThisCall(this);
    }

    void mmPopup::Unlock(THIS_ void) {
        lpmmPopup_Unlock.ThisCall(this);
    }

    void mmPopup::ProcessChat(THIS_ void) {
        lpmmPopup_ProcessChat.ThisCall(this);
    }

#pragma region "Stream implementation"
    void Stream::DumpOpenFiles(void) {
        lpStream_DumpOpenFiles();
    }

    Stream * Stream::Open(LPCSTR filename, bool isZipFile) {
        return lpStream_Open(filename, isZipFile);
    }

    Stream * Stream::Create(LPCSTR filename) {
        return lpStream_Create(filename);
    }

    int Stream::Read(THIS_ LPVOID dstBuf, int size) {
        return lpStream_Read.ThisCall(this, dstBuf, size);
    }

    int Stream::Write(THIS_ const LPVOID srcBuf, int size) {
        return lpStream_Write.ThisCall(this, srcBuf, size);
    }

    int Stream::GetCh(THIS_ void) {
        return lpStream_GetCh.ThisCall(this);
    }

    int Stream::PutCh(THIS_ unsigned char ch) {
        return lpStream_PutCh.ThisCall(this, ch);
    }

    int Stream::Seek(THIS_ int offset) {
        return lpStream_Seek.ThisCall(this, offset);
    }

    int Stream::Tell(THIS_ void) {
        return lpStream_Tell.ThisCall(this);
    }

    int Stream::Close(THIS_ void) {
        return lpStream_Close();
    }

    int Stream::Size(THIS_ void) {
        return lpStream_Size();
    }

    int Stream::Flush(THIS_ void) {
        return lpStream_Flush();
    }
#pragma endregion
}
