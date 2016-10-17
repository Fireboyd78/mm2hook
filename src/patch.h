#pragma once
#include "common.h"

#include <type_traits>

/*
    Patching utilities/structs
*/

namespace mem
{
    inline bool write_buffer(void* lpAddress, const void* lpReadAddress, std::size_t dwReadSize)
    {
        DWORD dwOldProtect;
        if (VirtualProtect(lpAddress, dwReadSize, PAGE_EXECUTE_READWRITE, &dwOldProtect))
        {
            memcpy(lpAddress, lpReadAddress, dwReadSize),

            VirtualProtect(lpAddress, dwReadSize, dwOldProtect, NULL);

            return true;
        }

        return false;
    }

    template <typename... TArgs>
    inline bool write_args(void* lpAddress, TArgs... args)
    {
        static_assert(sizeof...(args) > 0,
                      "No arguments provided");

        static_assert(variadic::true_for_all<std::is_trivially_copyable<TArgs>::value...>,
                      "Not all arguments are trivially copyable");

        constexpr std::size_t totalSize = variadic::sum<sizeof(TArgs)...>;

        DWORD dwOldProtect;
        if (VirtualProtect(lpAddress, totalSize, PAGE_EXECUTE_READWRITE, &dwOldProtect))
        {
            using variadic_unpacker_t = int[ ];

            (void) variadic_unpacker_t
            {
                (
                    //*static_cast<TArgs*>(lpAddress) = args,
                    memcpy(lpAddress, &args, sizeof(args)),
                    lpAddress = static_cast<char*>(lpAddress) + sizeof(args),
                0)...
            };

            VirtualProtect(lpAddress, totalSize, dwOldProtect, NULL);

            return true;
        }

        return false;
    }
}
