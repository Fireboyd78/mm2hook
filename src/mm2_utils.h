#pragma once
#include "mm2_common.h"

#include <array>

constexpr std::size_t MAX_HOOK_PTRS = 4096;

extern class MM2HookMgr;
extern class IMM2HookPtr;

enum MM2Version
{
    MM2_INVALID = -1,

    MM2_BETA_1 = 0,
    MM2_BETA_2 = 1,
    MM2_RETAIL = 2,

    MM2_NUM_VERSIONS,

    // versions that cannot load
    MM2_BETA_2_PETITE
};

using MM2AddressData = std::array<DWORD, MM2_NUM_VERSIONS>;

class MM2HookMgr
{
public:
    static void alloc(IMM2HookPtr *hook);
    static void free(int hook_idx);
    static bool reset(IMM2HookPtr *hook);

    static int Initialize(MM2Version version);
};

class IMM2HookPtr : public IHookPtr
{
private:
    int hook_idx = -1;
protected:
    MM2AddressData addressData;
public:
    inline IMM2HookPtr::IMM2HookPtr(const MM2AddressData &addressData)
        : addressData(addressData)
    {
        MM2HookMgr::alloc(this);
    };

    inline IMM2HookPtr(DWORD addrBeta1,
                       DWORD addrBeta2,
                       DWORD addrRetail)
        : IMM2HookPtr(MM2AddressData { addrBeta1, addrBeta2, addrRetail })
    { };

    inline IMM2HookPtr::~IMM2HookPtr()
    {
        MM2HookMgr::free(hook_idx);
    };

    inline void set_version(MM2Version gameVersion)
    {
        lpAddr = (LPVOID) addressData[gameVersion];
    };
};

template<typename TRet>
class MM2FnHook : public IMM2HookPtr
{
public:
    constexpr MM2FnHook(const MM2AddressData &addressData)
        : IMM2HookPtr(addressData) { };
    constexpr MM2FnHook(DWORD addrBeta1, DWORD addrBeta2, DWORD addrRetail)
        : IMM2HookPtr(addrBeta1, addrBeta2, addrRetail) { };

    template<typename ...TArgs>
    constexpr TRet operator()(TArgs ...args) const
    {
        typedef TRet(__cdecl *MethodCall)(TArgs...);

        return static_cast<MethodCall>(lpAddr)(args...);
    };

    template<typename ...TArgs, class TThis>
    constexpr TRet operator()(const TThis &&This, TArgs ...args) const
    {
        typedef TRet(__thiscall *MemberCall)(const TThis, TArgs...);

        return static_cast<MemberCall>(lpAddr)(This, args...);
    };
};

template<typename TType>
class MM2PtrHook : public IMM2HookPtr
{
public:
    constexpr MM2PtrHook(const MM2AddressData &addressData)
        : IMM2HookPtr(addressData)
    { };

    constexpr MM2PtrHook(DWORD addrBeta1, DWORD addrBeta2, DWORD addrRetail)
        : IMM2HookPtr(addrBeta1, addrBeta2, addrRetail)
    { };

    inline TType* get() const
    {
        return static_cast<TType*>(this->lpAddr);
    }

    inline void set(TType value)
    {
        *this->get() = value;
    }

    /*
        TType*->TMember - Pointer to struct of TType
    */
    inline TType& operator->() const
    {
        return *this->get();
    };

    /*
        &TType - Instance of TType
    */
    inline TType* operator&() const
    {
        return this->get();
    };

    /*
        *TType - Pointer to TType
    */
    inline TType& operator*() const
    {
        return *this->get();
    };


    /*
        TType*(this) - Convert this to TType pointer
    */
    explicit inline operator TType*() const
    {
        return this->get();
    }

    /*
        TType&(this) - Convert this to TType reference
    */
    inline operator TType&() const
    {
        return *this->get();
    }

    /*
        TType[0] - Pointer is TType array
    */
    inline TType& operator[ ](std::size_t index) const
    {
        return this->get()[index];
    }

    /*
        this() - TType is function pointer
    */
    template <typename... TArgs>
    inline auto operator()(TArgs... args)
    {
        return (*this->get())(args...);
    }
};

template <typename TFunc>
using MM2RawFnHook = MM2PtrHook<std::remove_pointer_t<TFunc>>;

/*
    Game patching functions
*/

enum CB_HOOK_TYPE
{
    HOOK_JMP = 0,
    HOOK_CALL,

    /* short-hand */

    JMP = 0,
    CALL,
};

struct CB_INSTALL_INFO
{
    MM2AddressData hookAddrs;
    CB_HOOK_TYPE hookType;
};

template<CB_HOOK_TYPE hookType>
struct CB_HOOK
{
    CB_INSTALL_INFO info;

    constexpr CB_HOOK(const MM2AddressData &addrs) : info { addrs, hookType } {};

    constexpr operator CB_INSTALL_INFO() const {
        return info;
    };
};

void InstallVTableHook(LPCSTR name, 
                       auto_ptr lpHookAddr,
                       std::initializer_list<MM2AddressData> addresses);

void InstallGamePatch(LPCSTR name,
                      std::initializer_list<unsigned char> bytes,
                      std::initializer_list<MM2AddressData> addresses);

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