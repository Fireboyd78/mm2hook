#include "mm2_utils.h"
#include "patch.h"

void InstallGamePatch(LPCSTR name,
                      std::initializer_list<unsigned char> bytes,
                      std::initializer_list<MM2AddressData> addresses)
{
    LogFile::Format(" - Installing patch: '%s'...\n", name);

    std::size_t count = 0;
    for (auto addr : addresses)
    {
        LogFile::Format("   - %08X : ", addr);

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

void InstallGameCallback(LPCSTR name,
                         auto_ptr lpCallback,
                         std::initializer_list<CB_INSTALL_INFO> callbacks)
{
    static const char *hook_types[] = {
        "jmp",
        "call"
    };

    LogFile::Format(" - Installing callback: '%s'...\n", name);

    std::size_t count = 0;
    for (auto cb : callbacks)
    {
        auto addr = cb.hookAddr;
        auto type = cb.hookType;

        LogFile::Format("   - [%s] %08X => %08X : ", hook_types[type], addr, lpCallback);

        if (addr != NULL && InstallCallbackHook(addr, lpCallback, type == HOOK_CALL))
        {
            LogFile::WriteLine("OK");

            ++count;
        } else
        {
            LogFile::WriteLine("Not Supported");
        }
    }

    LogFile::Format("   - Installed %u / %u callbacks\n", count, callbacks.size());
}

void InstallVTableHook(LPCSTR name, 
                       auto_ptr lpHookAddr,
                       std::initializer_list<MM2AddressData> addresses) {
    LogFile::Format(" - Installing V-Table hook: '%s'...\n", name);

    std::size_t count = 0;
    for (auto addr : addresses)
    {
        LogFile::Format("   - %08X => %08X :", addr, lpHookAddr);

        if (addr != NULL)
        {
            InstallVTHook(addr, lpHookAddr);
            LogFile::WriteLine("OK");

            ++count;
        }
        else
        {
            LogFile::WriteLine("Not supported");
        }
    }

    LogFile::Format("   - Installed %u / %u hooks\n", count, addresses.size());
}