#pragma once
#include "common.h"
#include "hook.h"

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

typedef struct MM2AddressData {
    DWORD addresses[MM2_NUM_VERSIONS];
} *LPMM2AddressData;

class IMM2HookPtr : public IHookPtr {
private:
    int hook_idx = -1;
protected:
    MM2AddressData addressData;
public:
    NOTHROW inline IMM2HookPtr(const MM2AddressData &addressData);

    NOTHROW inline ~IMM2HookPtr();

    inline void set_version(MM2Version gameVersion) {
        IHookPtr::operator=(addressData.addresses[gameVersion]);
    };
};

template<typename TRet>
class MM2FnHook : public IMM2HookPtr {
public:
    NOTHROW inline MM2FnHook(const MM2AddressData &addressData)
        : IMM2HookPtr(addressData) {};
    NOTHROW inline MM2FnHook(DWORD addrBeta1, DWORD addrBeta2, DWORD addrRetail)
        : IMM2HookPtr(MM2AddressData{ addrBeta1, addrBeta2, addrRetail }) {};
    
    template<typename ...TArgs>
    inline TRet operator()(TArgs ...args) const {
        return static_cast<TRet(__cdecl *)(TArgs...)>(lpAddr)(args...);
    };

    template<typename ...TArgs, class TThis>
    inline TRet operator()(const TThis &&This, TArgs ...args) const {
        return static_cast<TRet(__thiscall *)(const TThis, TArgs...)>(lpAddr)(This, args...);
    };
};

template<typename TType>
class MM2PtrHook : public IMM2HookPtr {
public:
    NOTHROW inline MM2PtrHook(const MM2AddressData &addressData)
        : IMM2HookPtr(addressData) {};
    NOTHROW inline MM2PtrHook(DWORD addrBeta1, DWORD addrBeta2, DWORD addrRetail)
        : IMM2HookPtr(MM2AddressData{ addrBeta1, addrBeta2, addrRetail }) {};

    inline bool is_null(void) const {
        return (lpAddr == nullptr);
    }

    inline void set(TType value) {
        *static_cast<TType*>(lpAddr) = value;
    }

    inline operator TType*() const {
        return (TType*)lpAddr;
    };

    inline operator TType() const {
        // used to check for null, but there should be a check instead.
        return *static_cast<TType*>(lpAddr);
    };

    inline TType* operator[](int index) const {
        return &*(TType*)((char *)lpAddr + (index * sizeof(TType)));
    };
};

/*
Game patching functions
*/

enum CB_HOOK_TYPE {
    HOOK_JMP,
    HOOK_CALL
};

struct FN_PTR {
    const LPVOID lpHandler;

    template<typename T>
    constexpr FN_PTR(T *test) : lpHandler((LPVOID)(*(DWORD*)&test)) {};

    template<class TT, typename T>
    constexpr FN_PTR(T(TT::*test)) : lpHandler((LPVOID)(*(DWORD*)&test)) {};

    constexpr operator LPVOID() const {
        return lpHandler;
    };

    constexpr operator DWORD() const {
        return reinterpret_cast<DWORD>(lpHandler);
    };
};

#define INIT_CB_DATA(type, addr1, addr2, addr3) {{ addr1, addr2, addr3 }, type }

#define INIT_CB_CALL(addr1, addr2, addr3) INIT_CB_DATA(HOOK_CALL, addr1, addr2, addr3)
#define INIT_CB_JUMP(addr1, addr2, addr3) INIT_CB_DATA(HOOK_JMP, addr1, addr2, addr3)

template<int size = 1>
struct CB_INSTALL_INFO {
    static const int length = size;

    FN_PTR cb_proc;
    struct {
        MM2AddressData addr_data;
        CB_HOOK_TYPE type;
    } cb_data[size];
};

template<int size = 1, int count = 1>
struct PATCH_INSTALL_INFO {
    static const int length = count;
    static const int buffer_size = size;

    char buffer[size];
    MM2AddressData addrData[count];
};

template<int count = 1>
struct VT_INSTALL_INFO {
    static const int length = count;

    FN_PTR dwHookAddr;
    MM2AddressData addrData[count];
};

bool InstallVTableHook(LPCSTR name, MM2Version gameVersion, LPVOID lpData, int count);
bool InstallGamePatch(LPCSTR name, MM2Version gameVersion, LPVOID lpData, int buffer_size, int count);
bool InstallGameCallback(LPCSTR cb_name, MM2Version gameVersion, LPVOID lpCallback, int count);

template<int count>
inline bool InstallVTableHook(LPCSTR name, MM2Version gameVersion, const VT_INSTALL_INFO<count> &data) {
    return InstallVTableHook(name, gameVersion, (LPVOID)&data, data.length);
}

template<int size, int count>
inline bool InstallGamePatch(LPCSTR name, MM2Version gameVersion, const PATCH_INSTALL_INFO<size, count> &data) {
    return InstallGamePatch(name, gameVersion, (LPVOID)&data, data.buffer_size, data.length);
}

template<int size>
inline bool InstallGameCallback(LPCSTR cb_name, MM2Version gameVersion, const CB_INSTALL_INFO<size> &cb) {
    return InstallGameCallback(cb_name, gameVersion, (LPVOID)&cb, cb.length);
}