#pragma once
#include "mm2_common.h"

// resolves externals for static AGEHook members in classes
#define DECLARE_HOOK(x) decltype(x) x

/*
    Game patching functions
*/

enum HOOK_TYPE : unsigned int
{
    JMP,
    CALL,
    PUSH,

    COUNT,
};

struct CB_INSTALL_INFO
{
    DWORD hookAddr;
    HOOK_TYPE hookType;
};

template<HOOK_TYPE hookType>
struct CB_HOOK
{
    CB_INSTALL_INFO info;

    constexpr CB_HOOK(const DWORD &addrs) : info { addrs, hookType } {};

    constexpr operator CB_INSTALL_INFO() const {
        return info;
    };
};

void InstallVTableHook(LPCSTR name, 
                       auto_ptr lpHookAddr,
                       std::initializer_list<DWORD> addresses);

void InstallGamePatch(LPCSTR name,
                      std::initializer_list<unsigned char> bytes,
                      std::initializer_list<DWORD> addresses);

void InstallGameCallback(auto_ptr lpCallback,
                         CB_INSTALL_INFO info);

void InstallGameCallback(LPCSTR name,
                         auto_ptr lpCallback,
                         std::initializer_list<CB_INSTALL_INFO> addresses);

union COLOR_ARGB
{
    struct
    {
        byte b, g, r, a;
    };

    UINT32 color;
};

template <std::uint8_t A, std::uint8_t R, std::uint8_t G, std::uint8_t B>
struct ColorFlags
{
    enum : std::uint32_t
    {
        // Bit Shifts (created in reverse order)
        SB = 0,
        SG = SB + B,
        SR = SG + G,
        SA = SR + R,

        // Bit Masks
        MA = ((1u << A) - 1u),
        MR = ((1u << R) - 1u),
        MG = ((1u << G) - 1u),
        MB = ((1u << B) - 1u),

        // Shifted Bit Masks
        SMA = MA << SA,
        SMR = MR << SR,
        SMG = MG << SG,
        SMB = MB << SB,
    };
};

template <
    int OA, int OR, int OG, int OB,
    int NA, int NR, int NG, int NB
>
constexpr inline UINT32 ConvertColor(const UINT32 color)
{
    using OF = ColorFlags<OA, OR, OG, OB>;
    using NF = ColorFlags<NA, NR, NG, NB>;

    return
        (((color & OF::SMA) >> OF::SA) * NF::MA / (OF::MA ? OF::MA : 1) << NF::SA) |
        (((color & OF::SMR) >> OF::SR) * NF::MG / (OF::MR ? OF::MR : 1) << NF::SR) |
        (((color & OF::SMG) >> OF::SG) * NF::MG / (OF::MG ? OF::MG : 1) << NF::SG) |
        (((color & OF::SMB) >> OF::SB) * NF::MB / (OF::MB ? OF::MB : 1) << NF::SB);
}

inline UINT32 GetPixelFormatColor(LPDDPIXELFORMAT lpDDPixelFormat, UINT32 color) {
    switch (lpDDPixelFormat->dwGBitMask)
    {
        // 555
        case 0x3E0:
            return ConvertColor<0, 8, 8, 8, 0, 5, 5, 5>(color);
        // 565
        case 0x7E0:
            return ConvertColor<0, 8, 8, 8, 0, 5, 6, 5>(color);
        // 888
        case 0xFF00:
            // already in the right format
            return color;
    }
    // unknown format
    return -1;
};