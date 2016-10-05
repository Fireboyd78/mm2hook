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
        auto addr = info->addrData[i][gameVersion];

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

void InstallGamePatch(LPCSTR name,
                      MM2Version gameVersion,
                      std::initializer_list<unsigned char> bytes,
                      std::initializer_list<MM2AddressData> addresses)
{
    LogFile::Format(" - Installing patch: '%s'...\n", name);

    std::size_t count = 0;
    for (auto patch : addresses)
    {
        auto addr = patch[gameVersion];

        LogFile::Format("   - %08X", addr);

        if (addr != NULL)
        {
            InstallPatch(addr, bytes.begin(), bytes.size());
            LogFile::WriteLine("OK");

            count++;
        }
        else
        {
            LogFile::WriteLine("Not Supported");
        }
    }

    LogFile::Format("   - Installed %u / %u patches\n", count, addresses.size());
}

void InstallGameCallback(LPCSTR name, MM2Version gameVersion, ANY_PTR lpCallback, CB_HOOK_TYPE type, std::initializer_list<MM2AddressData> addresses)
{
    LogFile::Format(" - Installing callback: '%s'...\n", name);

    std::size_t count = 0;
    for (auto patch : addresses)
    {
        auto addr = patch[gameVersion];

        LogFile::Format("   - %08X => %08X :", addr, lpCallback);
        if (addr != NULL && InstallCallbackHook(addr, lpCallback, type == HOOK_CALL))
        {
            LogFile::WriteLine("OK");

            ++count;
        }
        else
        {
            LogFile::WriteLine("Not Supported");
        }
    }

    LogFile::Format("   - Installed %u / %u callbacks\n", count, addresses.size());
}