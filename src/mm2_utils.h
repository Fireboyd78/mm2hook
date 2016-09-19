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
    NOTHROW inline IMM2HookPtr(MM2Version gameVersion, DWORD dwAddress) {
        IHookPtr::operator=(addressData.addresses[gameVersion] = dwAddress);
    };
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
    NOTHROW inline MM2FnHook(MM2Version gameVersion, DWORD dwAddress)
        : IMM2HookPtr(gameVersion, dwAddress) {};

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
    NOTHROW inline MM2PtrHook(MM2Version gameVersion, DWORD dwAddress)
        : IMM2HookPtr(gameVersion, dwAddress) {};

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