#include "mm2_utils.h"
#include "patch.h"

LPCSTR hook_types[hookType::COUNT] = {
    "jmp",
    "call",
    "push"
};

static init_base *g_initializers;

void init_base::Register() {
    this->next = g_initializers;
    g_initializers = this;
};

void init_base::RunAll() {
    for (auto func = g_initializers; func != nullptr; func = func->next) {
        func->Run();
    }
};

void __stdcall InstallHandler(LPCSTR name, InitFn installHandler) {
    LogFile::Format("Installing '%s' handler...\n", name);
    installHandler();
};

void InstallPatch(LPCSTR description,
                  std::initializer_list<unsigned char> bytes,
                  std::initializer_list<unsigned int> addresses)
{
    const auto begin = bytes.begin();
    const auto size = bytes.size();

    LogFile::Format(" - Installing patch [%08X : %08X]:\n", begin, size);

    if (description != NULL)
        LogFile::Format(" - Description: %s\n", description);

    for (auto addr : addresses)
    {
        LogFile::Format("   => %08X\n", addr);

        mem::copy(LPVOID(addr), begin, size);
    }
}

void InstallCallback(auto_ptr lpAddr, cbInfo callback)
{
    auto addr = callback.addr;
    auto type = callback.type;

    unsigned int dwVA = lpAddr.dwHandler;
    unsigned int dwRVA = (dwVA - (addr.dwHandler + 5));

    switch (type)
    {
        case hookType::CALL:
        {
            if (mem::read<byte>(addr) == 0xFF) {
                mem::write<byte, uint, byte>(addr, 0xE8, dwRVA, 0x90);
            } else {
                mem::write<byte, uint>(addr, 0xE8, dwRVA);
            }
        } break;

        case hookType::JMP:
        {
            if (mem::read<byte>(addr) == 0xFF) {
                mem::write<byte, uint, byte>(addr, 0xE9, dwRVA, 0x90);
            } else {
                mem::write<byte, uint>(addr, 0xE9, dwRVA);
            }
        } break;

        case hookType::PUSH:
        {
            mem::write<byte, uint>(addr, 0x68, lpAddr);
        } break;
    }
}

void InstallCallback(LPCSTR name,
                     LPCSTR description,
                     auto_ptr lpAddr,
                     std::initializer_list<cbInfo> callbacks)
{
    if (name != NULL)
    {
        LogFile::Format(" - Installing callback [%08X] for '%s':\n", lpAddr, name);
    }
    else
    {
        LogFile::Format(" - Installing callback [%08X]:\n", lpAddr);
    }

    if (description != NULL)
        LogFile::Format(" - Description: %s\n", description);

    for (auto cb : callbacks)
    {
        LogFile::Format("   => [%s] %08X\n", hook_types[cb.type], cb.addr);

        InstallCallback(lpAddr, cb);
    }
}

void InstallVTableHook(LPCSTR name, auto_ptr lpAddr, std::initializer_list<unsigned int> addresses) {
    LogFile::Format(" - Installing V-Table hook [%08X]: '%s'...\n", lpAddr, name);

    for (auto addr : addresses)
    {
        LogFile::Format("   => %08X\n", addr, lpAddr);

        mem::write<unsigned int>(LPVOID(addr), lpAddr);
    }
}
