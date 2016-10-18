#include "mm2_utils.h"
#include "patch.h"

void InstallPatch(LPCSTR name,
                  std::initializer_list<unsigned char> bytes,
                  std::initializer_list<unsigned int> addresses)
{
    LogFile::Format(" - Installing patch: '%s'...\n", name);

    for (auto addr : addresses)
    {
        LogFile::Format("   - %08X", addr);

        mem::write_buffer(LPVOID(addr), bytes.begin(), bytes.size());
    }

    LogFile::Format("   - Installed %d patches\n", addresses.size());
}

void InstallCallback(auto_ptr lpAddr,
                     cbInfo callback)
{
    auto addr = callback.addr;
    auto type = callback.type;

    unsigned int dwRVA = lpAddr - (addr + 5);

    switch (type)
    {
        case hookType::CALL:
        {
            mem::write_args<unsigned char, unsigned int>(addr, 0xE8, dwRVA);
        } break;

        case hookType::JMP:
        {
            mem::write_args<unsigned char, unsigned int>(addr, 0xE9, dwRVA);
        } break;

        case hookType::PUSH:
        {
            mem::write_args<unsigned char, unsigned int>(addr, 0x68, lpAddr);
        } break;
    }
}

void InstallCallback(LPCSTR name,
                     auto_ptr lpAddr,
                     std::initializer_list<cbInfo> callbacks)
{
    LogFile::Format(" - Installing callback: '%s'...\n", name);

    const char* hook_types[hookType::COUNT] =
    {
        "jmp",
        "call",
        "push"
    };

    for (auto cb : callbacks)
    {
        InstallCallback(lpAddr, cb);

        LogFile::Format("   - [%s] %08X => %08X\n", hook_types[cb.type], cb.addr, lpAddr);
    }

    LogFile::Format("   - Installed %d callbacks\n", callbacks.size());
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

    LogFile::Format("   - Installed %d / %d hooks\n", addresses.size());
}
