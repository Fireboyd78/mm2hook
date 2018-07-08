#pragma once
#include "common.h"

#include <type_traits>

/*
    Patching utilities/structs
*/

namespace mem
{
    inline bool copy(void *lpAddress, const void *lpReadAddress, size_t dwReadSize)
    {
        DWORD dwOldProtect;

        if (VirtualProtect(lpAddress, dwReadSize, PAGE_EXECUTE_READWRITE, &dwOldProtect))
        {
            memcpy(lpAddress, lpReadAddress, dwReadSize);
            VirtualProtect(lpAddress, dwReadSize, dwOldProtect, &dwOldProtect);

            return true;
        }

        return false;
    }

    template <typename T>
    inline T read(auto_ptr address) {
        DWORD dwOldProtect;

        if (VirtualProtect(address, sizeof(T), PAGE_EXECUTE_READ, &dwOldProtect))
        {
            T value = *(T *)address;
            VirtualProtect(address, sizeof(T), dwOldProtect, &dwOldProtect);

            return value;
        }

        return NULL;
    }

    template <typename Func, typename... Args>
    void unpack(Func func, Args&&... args)
    {
        using unpacker_t = int[];

        (void)unpacker_t
        {
            (func(std::forward<Args>(args)), 0)..., 0
        };
    }

    template <typename ...TArgs>
    inline bool write(auto_ptr address, TArgs ...args)
    {
        static_assert(sizeof...(args) > 0,
                      "No arguments provided");

        static_assert(variadic::true_for_all<std::is_trivially_copyable<TArgs>::value...>,
                      "Not all arguments are trivially copyable");

        constexpr size_t totalSize = variadic::sum<sizeof(TArgs)...>;

        DWORD dwOldProtect;

        if (VirtualProtect(address, totalSize, PAGE_EXECUTE_READWRITE, &dwOldProtect))
        {
            void *lpDst = address;

            unpack([&](const auto& value)
            {
                memcpy(lpDst, &value, sizeof(value));
                lpDst = static_cast<char*>(lpDst) + sizeof(value);
            }, std::forward<TArgs>(args)...);

            VirtualProtect(address, totalSize, dwOldProtect, &dwOldProtect);

            return true;
        }

        return false;
    }

    inline bool nop(auto_ptr address, int count = 1)
    {
        constexpr int OP_NOP = 0x90;

        DWORD dwOldProtect;

        if (VirtualProtect(address, count, PAGE_EXECUTE_READWRITE, &dwOldProtect))
        {
            uint8_t *lpDst = address;

            for (int i = 0; i < count; i++)
                lpDst[i] = OP_NOP;

            VirtualProtect(address, count, dwOldProtect, &dwOldProtect);

            return true;
        }

        return false;
    }
}
