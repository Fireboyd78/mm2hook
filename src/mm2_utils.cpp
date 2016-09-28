#include "mm2_utils.h"
#include "patch.h"

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