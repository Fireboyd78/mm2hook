#pragma once

#include <cstdint>
#include "mm2_vector.h"

using namespace MM2;

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
    std::uint8_t OA, std::uint8_t OR, std::uint8_t OG, std::uint8_t OB,
    std::uint8_t NA, std::uint8_t NR, std::uint8_t NG, std::uint8_t NB
>
constexpr inline std::uint32_t ConvertColor(const std::uint32_t color)
{
    using OF = ColorFlags<OA, OR, OG, OB>;
    using NF = ColorFlags<NA, NR, NG, NB>;

    return
        (((color & OF::SMA) >> OF::SA) * NF::MA / (OF::MA ? OF::MA : 1) << NF::SA) |
        (((color & OF::SMR) >> OF::SR) * NF::MG / (OF::MR ? OF::MR : 1) << NF::SR) |
        (((color & OF::SMG) >> OF::SG) * NF::MG / (OF::MG ? OF::MG : 1) << NF::SG) |
        (((color & OF::SMB) >> OF::SB) * NF::MB / (OF::MB ? OF::MB : 1) << NF::SB);
}

template <std::uint8_t A, std::uint8_t R, std::uint8_t G, std::uint8_t B>
Vector4 ColorToVector(std::uint32_t color)
{
    using CF = ColorFlags<A, R, G, B>;

    return
    {
        float((color & CF::SMR) >> CF::SR) / (CF::MR ? CF::MR : 1),
        float((color & CF::SMG) >> CF::SG) / (CF::MG ? CF::MG : 1),
        float((color & CF::SMB) >> CF::SB) / (CF::MB ? CF::MB : 1),
        float((color & CF::SMA) >> CF::SA) / (CF::MA ? CF::MA : 1)
    };
}

template <std::uint8_t A, std::uint8_t R, std::uint8_t G, std::uint8_t B>
std::uint32_t VectorToColor(Vector4 color)
{
    using CF = ColorFlags<A, R, G, B>;

    return
        (std::uint32_t(color.X * CF::MR) << CF::SR) |
        (std::uint32_t(color.Y * CF::MG) << CF::SG) |
        (std::uint32_t(color.Z * CF::MB) << CF::SB) |
        (std::uint32_t(color.W * CF::MA) << CF::SA);
}