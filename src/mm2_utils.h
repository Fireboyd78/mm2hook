#pragma once
#include "mm2_common.h"

// resolves externals for static AGEHook members in classes
#define DECLARE_HOOK(x) decltype(x) x

/*
    Game patching functions
*/

enum hookType : unsigned int
{
    JMP,
    CALL,
    PUSH,

    COUNT,
};

struct cbInfo
{
    auto_ptr addr;
    hookType type;
};

template<hookType type>
struct cbHook
{
    cbInfo info;

    constexpr cbHook(const DWORD addrs) : info { addrs, type } {};

    constexpr operator cbInfo() const {
        return info;
    };
};

void InstallPatch(LPCSTR description,
                  std::initializer_list<unsigned char> bytes,
                  std::initializer_list<unsigned int> addresses);

inline void InstallPatch(std::initializer_list<unsigned char> bytes,
                         std::initializer_list<unsigned int> addresses) {
    InstallPatch(NULL, bytes, addresses);
}

void InstallCallback(auto_ptr lpAddr, cbInfo callback);

void InstallCallback(LPCSTR name,
                     LPCSTR description,
                     auto_ptr lpAddr,
                     std::initializer_list<cbInfo> callbacks);

inline void InstallCallback(LPCSTR name, auto_ptr lpAddr, std::initializer_list<cbInfo> callbacks) {
    InstallCallback(name, NULL, lpAddr, callbacks);
}

void InstallVTableHook(LPCSTR name, auto_ptr lpAddr, std::initializer_list<unsigned int> addresses);

union ColorARGB
{
    struct
    {
        byte b, g, r, a;
    };

    unsigned int color;
};

template <
    unsigned int A, unsigned int R, unsigned int G, unsigned int B
>
struct ColorFlags
{
    enum : unsigned int
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
    unsigned int OA, unsigned int OR, unsigned int OG, unsigned int OB,
    unsigned int NA, unsigned int NR, unsigned int NG, unsigned int NB
>
inline unsigned int ConvertColor(const unsigned int color)
{
    using OF = ColorFlags<OA, OR, OG, OB>;
    using NF = ColorFlags<NA, NR, NG, NB>;

    return
        (((color & OF::SMA) >> OF::SA) * NF::MA / (OF::MA ? OF::MA : 1) << NF::SA) |
        (((color & OF::SMR) >> OF::SR) * NF::MG / (OF::MR ? OF::MR : 1) << NF::SR) |
        (((color & OF::SMG) >> OF::SG) * NF::MG / (OF::MG ? OF::MG : 1) << NF::SG) |
        (((color & OF::SMB) >> OF::SB) * NF::MB / (OF::MB ? OF::MB : 1) << NF::SB);
}

inline unsigned int GetPixelFormatColor(LPDDPIXELFORMAT lpDDPixelFormat, unsigned int color) {
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
