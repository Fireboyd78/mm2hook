#pragma once

#include <array>

#include "common.h"
#include "hook.h"

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
    HOOK_JMP,
    HOOK_CALL
};

template<int count = 1>
struct VT_INSTALL_INFO
{
    static const int length = count;

    ANY_PTR dwHookAddr;
    MM2AddressData addrData[count];
};

bool InstallVTableHook(LPCSTR name, MM2Version gameVersion, LPVOID lpData, int count);

void InstallGamePatch(LPCSTR name,
                      MM2Version gameVersion,
                      std::initializer_list<unsigned char> bytes,
                      std::initializer_list<MM2AddressData> addresses);

void InstallGameCallback(LPCSTR name,
                         MM2Version gameVersion,
                         ANY_PTR lpCallback,
                         CB_HOOK_TYPE type,
                         std::initializer_list<MM2AddressData> addresses);

template<int count>
inline bool InstallVTableHook(LPCSTR name, MM2Version gameVersion, const VT_INSTALL_INFO<count> &data)
{
    return InstallVTableHook(name, gameVersion, (LPVOID) &data, data.length);
}