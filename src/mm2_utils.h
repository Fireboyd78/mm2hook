#pragma once
#include "mm2_common.h"

/*
    Game patching functions
*/

#define CALL cb::type::call
#define JMP cb::type::jmp
#define PUSH cb::type::push

template <cb::type type>
using cbHook = cb::hook<type>;

void Installf(LPCSTR format, ...);

typedef void (*InitFn)();

// make this clean up the stack since we'll be calling it a lot
void __stdcall InstallHandler(LPCSTR name, InitFn installHandler);

/*
    Assumes THandler is a class that implements a public,
    static method called 'Install' with no return type.
*/
template <class THandler>
inline void InstallHandler(LPCSTR name) {
    InstallHandler(name, &THandler::Install);
};

class init_base {
protected:
    init_base *next;

    void Register();
public:
    virtual ~init_base() = default;

    init_base() {
        Register();
    }

    virtual void Run() {
        /* do nothing */
    }

    static void RunAll();
};

class init_func : public init_base {
protected:
    InitFn fnInit;
public:
    explicit init_func(InitFn fnInit)
        : fnInit(fnInit), init_base() {}

    void Run() override {
        fnInit();
    }
};

class init_handler : public init_func {
protected:
    const char *name;
public:
    explicit init_handler(const char *name, InitFn fnInit)
        : name(name), init_func(fnInit) {}

    void Run() override {
        InstallHandler(name, fnInit);
    }
};

template <typename THandler>
static init_handler CreateHandler(const char *name) {
    return init_handler(name, &THandler::Install);
}

void InstallPatch(LPCSTR description,
                  std::initializer_list<unsigned char> bytes,
                  std::initializer_list<unsigned int> addresses);

inline void InstallPatch(std::initializer_list<unsigned char> bytes,
                         std::initializer_list<unsigned int> addresses) {
    InstallPatch(NULL, bytes, addresses);
}

void InstallCallback(auto_ptr lpAddr, const cb::info &callback);

void InstallCallback(LPCSTR name,
                     LPCSTR description,
                     auto_ptr lpAddr,
                     cb::info::list callbacks);

inline void InstallCallback(LPCSTR name, auto_ptr lpAddr, cb::info::list callbacks) {
    InstallCallback(name, NULL, lpAddr, callbacks);
}

inline void InstallCallback(auto_ptr lpAddr, LPCSTR description, cb::info::list callbacks) {
    InstallCallback(NULL, description, lpAddr, callbacks);
}

inline void InstallCallback(auto_ptr lpAddr, cb::info::list callbacks, LPCSTR description) {
    InstallCallback(NULL, description, lpAddr, callbacks);
}

inline void InstallCallback(auto_ptr lpAddr, cb::info::list callbacks) {
    InstallCallback(NULL, NULL, lpAddr, callbacks);
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
