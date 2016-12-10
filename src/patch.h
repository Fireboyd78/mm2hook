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

    template <typename ...TArgs>
    inline bool write(void *lpAddress, TArgs ...args)
    {
        static_assert(sizeof...(args) > 0,
                      "No arguments provided");

        static_assert(variadic::true_for_all<std::is_trivially_copyable<TArgs>::value...>,
                      "Not all arguments are trivially copyable");

        constexpr size_t totalSize = variadic::sum<sizeof(TArgs)...>;

        DWORD dwOldProtect;

        if (VirtualProtect(lpAddress, totalSize, PAGE_EXECUTE_READWRITE, &dwOldProtect))
        {
            void *lpDst = lpAddress;

            using variadic_unpacker_t = int[];

            (void)variadic_unpacker_t
            {
                (
                    memcpy(lpDst, &args, sizeof(args)),
                    lpDst = static_cast<char*>(lpDst) + sizeof(args),
                0)...
            };

            VirtualProtect(lpAddress, totalSize, dwOldProtect, &dwOldProtect);

            return true;
        }

        return false;
    }
}
