#pragma once
#include "common.h"
#include "hook.h"

#define MAX_HOOK_PTRS 4096

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

struct MM2AddressData {
    DWORD addresses[MM2_NUM_VERSIONS];
};

class MM2HookMgr {
public:
    static void alloc(IMM2HookPtr *hook);
    static void free(int hook_idx);
    static bool reset(IMM2HookPtr *hook);

    static int Initialize(MM2Version version);
};

class IMM2HookPtr : public IHookPtr {
private:
    int hook_idx = -1;
protected:
    MM2AddressData addressData;
public:
    inline IMM2HookPtr::IMM2HookPtr(const MM2AddressData &addressData) : addressData(addressData) {
        MM2HookMgr::alloc(this);
    };

    inline IMM2HookPtr::~IMM2HookPtr() {
        MM2HookMgr::free(hook_idx);
    };

    inline IMM2HookPtr( DWORD addrBeta1,
                        DWORD addrBeta2,
                        DWORD addrRetail
    ) : IMM2HookPtr( 
            MM2AddressData {
                addrBeta1,
                addrBeta2,
                addrRetail
            }
    ) {};

    inline void set_version(MM2Version gameVersion) {
        lpAddr = (LPVOID)addressData.addresses[gameVersion];
    };
};

template<typename TRet>
class MM2FnHook : public IMM2HookPtr {
public:
    constexpr MM2FnHook(const MM2AddressData &addressData)
        : IMM2HookPtr(addressData) {};
    constexpr MM2FnHook(DWORD addrBeta1, DWORD addrBeta2, DWORD addrRetail)
        : IMM2HookPtr(addrBeta1, addrBeta2, addrRetail) {};

    template<typename ...TArgs>
    constexpr TRet operator()(TArgs ...args) const {
        typedef TRet (__cdecl *MethodCall)(TArgs...);

        return static_cast<MethodCall>(lpAddr)(args...);
    };

    template<typename ...TArgs, class TThis>
    constexpr TRet operator()(const TThis &&This, TArgs ...args) const {
        typedef TRet (__thiscall *MemberCall)(const TThis, TArgs...);

        return static_cast<MemberCall>(lpAddr)(This, args...);
    };
};

template<typename TType>
class MM2PtrHook : public IMM2HookPtr {
public:
    constexpr MM2PtrHook(const MM2AddressData &addressData)
        : IMM2HookPtr(addressData) {};
    constexpr MM2PtrHook(DWORD addrBeta1, DWORD addrBeta2, DWORD addrRetail)
        : IMM2HookPtr(addrBeta1, addrBeta2, addrRetail) {};

    inline void set(TType value) {
        *static_cast<TType*>(lpAddr) = value;
    };

    /*
        Member-of-pointer operator (e.g. ptr->SomeMethod(); // MM2PtrHook<TClass> ptr_hook)
    */
    constexpr TType* operator->() const {
        return reinterpret_cast<TType*>(lpAddr);
    };

    /*
        Address-of operator (e.g. TType *ptr = &ptr_hook; // MM2PtrHook<TType> ptr_hook)
    */
    constexpr TType* operator &() const {
        return reinterpret_cast<TType*>(lpAddr);
    };

    /*
        Indirection operator (e.g. TType ptr = *ptr_hook; // MM2PtrHook<TType> ptr_hook)
    */
    constexpr TType operator *() const {
        return *static_cast<TType*>(lpAddr);
    };

    /*
        Implicit conversion (e.g. TType *value = ptr_hook; // MM2PtrHook<TType *> ptr_hook)
    */
    constexpr operator TType*() const {
        return reinterpret_cast<TType*>(lpAddr);
    };

    /*
        Implicit conversion (e.g. TType value = ptr_hook; // MM2PtrHook<TType> ptr_hook)
    */
    constexpr operator TType() const {
        return *static_cast<TType*>(lpAddr);
    };

    constexpr TType* operator[](int index) const {
        return reinterpret_cast<TType*>((char *)lpAddr + (index * sizeof(TType)));
    };
};

/*
Game patching functions
*/

enum CB_HOOK_TYPE {
    HOOK_JMP,
    HOOK_CALL
};

#define INIT_CB_DATA(type, addr1, addr2, addr3) {{ addr1, addr2, addr3 }, type }

#define INIT_CB_CALL(addr1, addr2, addr3) INIT_CB_DATA(HOOK_CALL, addr1, addr2, addr3)
#define INIT_CB_JUMP(addr1, addr2, addr3) INIT_CB_DATA(HOOK_JMP, addr1, addr2, addr3)

template<int size = 1>
struct CB_INSTALL_INFO {
    static const int length = size;

    fn_ptr cb_proc;
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

    fn_ptr dwHookAddr;
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