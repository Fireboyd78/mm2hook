#include "mm2_utils.h"
#include "patch.h"

// current game version
static MM2Version g_version = MM2_INVALID;

// next index to use
static int g_hook_idx = 0;

// global hook pool -- used to directly initialize pointers
static IMM2HookPtr *g_hooks[MAX_HOOK_PTRS] = { NULL };

void MM2HookMgr::alloc(IMM2HookPtr *hook) {
    if (g_hook_idx < MAX_HOOK_PTRS) {
        reset(hook); // try to update pointer
        g_hooks[g_hook_idx++] = hook;
    } else {
        MessageBox(NULL, "FATAL ERROR: No more hook pointers available!", "MM2Hook", MB_OK | MB_ICONERROR);
    }
};

void MM2HookMgr::free(int hook_idx) {
    if (hook_idx > 0)
        g_hooks[hook_idx] = NULL;
};

bool MM2HookMgr::reset(IMM2HookPtr *hook) {
    if (g_version != MM2_INVALID) {
        hook->set_version(g_version);
        return true;
    }
    return false;
};

int MM2HookMgr::Initialize(MM2Version version) {
    g_version = version;
    g_hook_idx = 0;

    if (g_version == MM2_INVALID)
        return HOOK_INIT_FAILED;

    // try to initialize hooks
    IMM2HookPtr *hook;

    int i = 0;
    int numHooks = 0;

    LogFile::WriteLine("Initializing MM2 hooks...");

    while ((hook = g_hooks[i++]) != NULL) {
        if (reset(hook))
            ++numHooks;
    }

    LogFile::Format(" - numHooks: %d\n", numHooks);

    if (numHooks == 0) {
        LogFile::WriteLine("WARNING: No supported hooks were found for this game version. Crashes may occur!");
        return HOOK_INIT_UNSUPPORTED;
    }

    return HOOK_INIT_OK;
};

bool InstallVTableHook(LPCSTR name, MM2Version gameVersion, LPVOID lpData, int count) {
    LogFile::Format(" - Installing V-Table hook: '%s'...\n", name);

    auto info = (VT_INSTALL_INFO<>*)lpData;

    if (count == 0) {
        LogFile::WriteLine(" - ERROR! Data is empty!");
        return false;
    }

    int progress = 0;

    for (int i = 0; i < count; i++) {
        auto addr = info->addrData[i].addresses[gameVersion];

        LogFile::Format("  - [%d] %08X => %08X : ", (i + 1), addr, info->dwHookAddr);

        if (addr != NULL)
        {
            InstallVTHook(addr, info->dwHookAddr);
            LogFile::WriteLine("OK");

            progress++;
        } else LogFile::WriteLine("Not supported");
    }

    bool success = (progress > 0);
    return success;
}

bool InstallGamePatch(LPCSTR name, MM2Version gameVersion, LPVOID lpData, int buffer_size, int count) {
    LogFile::Format(" - Installing patch: '%s'...\n", name);

    auto info = (PATCH_INSTALL_INFO<>*)lpData;

    if (count == 0 || buffer_size == 0) {
        LogFile::WriteLine(" - ERROR! Data is empty!");
        return false;
    }

    int progress = 0;

    for (int i = 0; i < count; i++) {
        auto addr = info->addrData[i].addresses[gameVersion];

        LogFile::Format("  - [%d] %08X => %08X : ", (i + 1), addr, (BYTE*)&info->buffer);

        if (addr != NULL)
        {
            InstallPatch(addr, (BYTE*)&info->buffer, buffer_size);
            LogFile::WriteLine("OK");

            progress++;
        } else LogFile::WriteLine("Not supported");
    }

    bool success = (progress > 0);
    return success;
}

bool InstallGameCallback(LPCSTR cb_name, MM2Version gameVersion, LPVOID lpCallback, int count) {
    LogFile::Format(" - Installing callback: '%s'...\n", cb_name);

    if (count == 0) {
        LogFile::WriteLine(" - ERROR! Address data is empty!");
        return false;
    }

    auto cb_info = (CB_INSTALL_INFO<>*)lpCallback;

    const DWORD cb = cb_info->cb_proc;
    int progress = 0;

    for (int i = 0; i < count; i++) {
        auto data = cb_info->cb_data[i];
        auto addr = data.addr_data.addresses[gameVersion];

        LogFile::Format("  - [%d]: %08X => %08X : ", (i + 1), addr, cb);

        if (addr != NULL && InstallCallbackHook(addr, cb, (data.type == HOOK_CALL)))
        {
            LogFile::WriteLine("OK");
            progress++;
        } else LogFile::WriteLine("Not supported");
    }

    bool success = (progress > 0);
    return success;
}