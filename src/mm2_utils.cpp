#include "mm2_utils.h"
#include "patch.h"

void InstallGamePatch(LPCSTR name,
                      std::initializer_list<unsigned char> bytes,
                      std::initializer_list<unsigned int> addresses)
{
    LogFile::Format(" - Installing patch: '%s'...\n", name);

    for (auto addr : addresses)
    {
        LogFile::Format("   - %08X", addr);

        mem::write_buffer(LPVOID(addr), bytes.begin(), bytes.size());
    }

    LogFile::Format("   - Installed %u patches\n", addresses.size());
}

void InstallGameCallback(auto_ptr lpCallback,
                         CB_INSTALL_INFO info)
{
    auto addr = info.hookAddr;
    auto type = info.hookType;

    unsigned int dwRVA = lpCallback - (addr + 5);

    switch (type)
    {
        case HOOK_TYPE::CALL:
        {
            mem::write_args<unsigned char, unsigned int>(LPVOID(addr), 0xE8, dwRVA);
        } break;

        case HOOK_TYPE::JMP:
        {
            mem::write_args<unsigned char, unsigned int>(LPVOID(addr), 0xE9, dwRVA);
        } break;

        case HOOK_TYPE::PUSH:
        {
            mem::write_args<unsigned char, unsigned int>(LPVOID(addr), 0x68, lpCallback);
        } break;
    }
}

void InstallGameCallback(LPCSTR name,
                         auto_ptr lpCallback,
                         std::initializer_list<CB_INSTALL_INFO> callbacks)
{
    LogFile::Format(" - Installing callback: '%s'...\n", name);

    const char* hook_types[HOOK_TYPE::COUNT] =
    {
        "jmp",
        "call",
        "push"
    };

    for (auto cb : callbacks)
    {
        InstallGameCallback(lpCallback, cb);

        LogFile::Format("   - [%s] %08X => %08X\n", hook_types[cb.hookType], cb.hookAddr, lpCallback);
    }

    LogFile::Format("   - Installed %u callbacks\n", callbacks.size());
}

void InstallVTableHook(LPCSTR name,
                       auto_ptr lpHookAddr,
                       std::initializer_list<unsigned int> addresses) {
    LogFile::Format(" - Installing V-Table hook: '%s'...\n", name);

    for (auto addr : addresses)
    {
        mem::write_args<unsigned int>(LPVOID(addr), lpHookAddr);

        LogFile::Format("   - %08X => %08X", addr, lpHookAddr);
    }

    LogFile::Format("   - Installed %u / %u hooks\n", addresses.size());
}
