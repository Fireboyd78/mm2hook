#pragma once
#include "common.h"

#define HOOK_INIT_OK            0
#define HOOK_INIT_FAILED        1
#define HOOK_INIT_UNSUPPORTED   2

#define VTCALLTYPE              (__thiscall **)
#define VTCALL                  void VTCALLTYPE
#define VTCALL_(t)              t VTCALLTYPE                     

#define HOOK_PTR pBase

//
// Description:
//  A template used for hooking into virtual table pointers without overriding the original data.
//  Classes that inherit from IAutoHook will also inherit from the base class.
//  The default constructor will create a pointer to the original data, allowing access to base methods.
//
// Params:
//  TBase - The base type that is being hooked into.
//
template<typename TBase>
struct IAutoHook : public TBase {
protected:
    TBase *HOOK_PTR;
public:
    IAutoHook(TBase *lpBase) {
        HOOK_PTR = lpBase;
    };
};

#define BEGIN_HOOK_TABLE(name,base) \
struct name : public IAutoHook<base> { \
public: \
	name(base *lpBase) : IAutoHook(lpBase) {};

#define END_HOOK_TABLE() };

template<typename TRet = void, typename ...TArgs>
struct FnHook {
private:
    LPVOID lpFunc;
public:
    FnHook() {
        lpFunc = NULL;
    };

    FnHook(LPVOID lpFunc) {
        lpFunc = lpFunc;
    };

    FnHook(const DWORD dwAddress)
        : FnHook((LPVOID)dwAddress) {
    };

    FORCEINLINE TRet operator()(TArgs ...args) const {
        return (*(TRet(__cdecl *)(TArgs...))lpFunc)(args...);
    };

    template<typename ...TArgs2>
    FORCEINLINE TRet operator()(TArgs ...args, TArgs2 ...args2) const {
        return (*(TRet(__cdecl *)(TArgs..., TArgs2...))lpFunc)(args..., args2...);
    };

    template<class TThis>
    FORCEINLINE TRet operator()(TThis &This, TArgs ...args) const {
        return (*(TRet(__thiscall *)(_THIS_ TArgs...))lpFunc)((LPVOID)This, args...);
    };

    FnHook& operator=(LPVOID lpFunc) {
        this->lpFunc = lpFunc;
        return *this;
    };

    FnHook& operator=(const DWORD dwAddress) {
        this->lpFunc = (LPVOID)dwAddress;
        return *this;
    };
};

template<typename TType>
struct PtrHook {
private:
    LPVOID lpData;
public:
    FORCEINLINE PtrHook() {
        this->lpData = NULL;
    };

    FORCEINLINE PtrHook(LPVOID lpData) {
        this->lpData = lpData;
    };

    FORCEINLINE PtrHook(const DWORD dwAddress)
        : PtrHook((LPVOID)dwAddress) {
    };

    FORCEINLINE operator TType() const {
        if (lpData == NULL)
        {
            //LogFile::WriteLine("PtrHook::get() -- lpData is NULL!\n");
            return NULL;
        }
        else
        {
            TType ret = *static_cast<TType*>(lpData);
            //LogFile::Format("PtrHook::get() -- lpData is [%08X] => %08X\n", lpData, ret);
            return ret;
        }
    };

    PtrHook& operator=(LPVOID lpData) {
        this->lpData = lpData;
        return *this;
    };

    PtrHook& operator=(const DWORD dwAddress) {
        this->lpData = (LPVOID)dwAddress;
        return *this;
    };
};

//
// Namespace that includes various hooking methods.
//
namespace hook {
    struct VTHook {
        struct Stub {
        private:
            LPVOID lpData;
            LPVOID lpFunc;
        public:
            NOTHROW Stub(const VTHook &vtHook, int index) {
                this->lpData = vtHook.GetDataPtr();
                this->lpFunc = (LPVOID)(*(DWORD*)lpData + (index * 0x4));
            };

            NOTHROW Stub(const VTHook &vtHook) : Stub(vtHook, 0) {
            };

            LPVOID GetDataPtr(void) const {
                return lpData;
            };

            LPVOID GetVTablePtr(void) const {
                return (LPVOID)(*(DWORD*)lpData);
            };

            LPVOID GetFunctionPtr(void) const {
                return lpFunc;
            };

            DWORD GetFunctionOffset(void) const {
                return ((DWORD)GetFunctionPtr() - (DWORD)GetVTablePtr());
            };

            int GetFunctionIndex(void) const {
                return GetFunctionOffset() / 0x4;
            };

            explicit operator LPVOID(void) const {
                return GetFunctionPtr();
            };

            /**********************************************************************
            * Resources:
            * - http://stackoverflow.com/a/16722559 > Virtual table functions
            * - http://stackoverflow.com/a/34251857 > Variadic templates
            **********************************************************************/

            template<typename TRet>
            TRet Call(void) const {
                return (*(VTCALL_(TRet)(_THIS))lpFunc)(lpData);
            };

            template<typename TRet, typename ...TArgs>
            TRet Call(TArgs... args) const {
                return (*(VTCALL_(TRet)(_THIS_ TArgs...))lpFunc)(lpData, args...);
            };

            template<typename ...TArgs>
            void operator()(TArgs... args) const {
                Call<void, TArgs...>(args...);
            };

            void operator()(void) const {
                Call<void>();
            };
        };
    private:
        LPVOID lpData;
    public:
        NOTHROW VTHook(LPVOID lpData) {
            this->lpData = lpData;
        };

        NOTHROW VTHook(const DWORD dwAddress)
            : VTHook((LPVOID)dwAddress) {
        };

        LPVOID GetDataPtr(void) const {
            return lpData;
        };

        LPVOID GetVTablePtr(void) const {
            return (LPVOID)(*(DWORD*)lpData);
        };

        Stub GetFunctionStub(int index) const {
            return Stub(*this, index);
        };

        Stub operator[](int index) const {
            return GetFunctionStub(index);
        };

        VTHook operator >> (int index) {
            return VTHook((LPVOID)((DWORD*)lpData + (index * 0x4)));
        };

        VTHook operator<<(int index) {
            return VTHook((LPVOID)((DWORD*)lpData - (index * 0x4)));
        };

        VTHook& operator>>=(int index) {
            this->lpData = (LPVOID)((DWORD*)lpData + (index * 0x4));
            return *this;
        };

        VTHook& operator<<=(int index) {
            this->lpData = (LPVOID)((DWORD*)lpData - (index * 0x4));
            return *this;
        };

        VTHook operator+(DWORD offset) {
            return VTHook((LPVOID)((DWORD*)lpData + offset));
        };

        VTHook operator-(DWORD offset) {
            return VTHook((LPVOID)((DWORD*)lpData - offset));
        };

        VTHook& operator+=(DWORD offset) {
            this->lpData = (LPVOID)((DWORD*)lpData + offset);
            return *this;
        };

        VTHook& operator-=(DWORD offset) {
            this->lpData = (LPVOID)((DWORD*)lpData - offset);
            return *this;
        };

        VTHook& operator=(LPVOID lpData) {
            this->lpData = lpData;
            return *this;
        };

        VTHook& operator=(const DWORD dwAddress) {
            this->lpData = (LPVOID)lpData;
            return *this;
        };

        bool operator==(const VTHook& vt) const {
            return (this->lpData == vt.lpData);
        };

        bool operator==(LPVOID lpData) const {
            return (this->lpData == lpData);
        };
    };

    template<typename TRet>
    inline TRet Call(LPVOID lpFunc) {
        return (*(TRet(__cdecl *)(void))lpFunc)(void);
    }

    template<typename TRet, typename ...TArgs>
    inline TRet Call(LPVOID lpFunc, TArgs ...args) {
        return (*(TRet(__cdecl *)(TArgs...))lpFunc)(args...);
    }

    template<typename TRet>
    inline TRet Call(const DWORD dwAddress) {
        return Call<TRet>((LPVOID)dwAddress);
    }

    template<typename TRet, typename ...TArgs>
    inline TRet Call(const DWORD dwAddress, TArgs ...args) {
        return Call<TRet>((LPVOID)dwAddress, args...);
    }

    template<typename TRet>
    inline TRet ThisCall(LPVOID lpFunc, LPVOID lpData) {
        return (*(TRet(__thiscall *)(_THIS))lpFunc)(lpData);
    }

    template<typename TRet, typename ...TArgs>
    inline TRet ThisCall(LPVOID lpFunc, LPVOID lpData, TArgs ...args) {
        return (*(TRet(__thiscall *)(_THIS_ TArgs...))lpFunc)(lpData, args...);
    }

    template<typename TRet>
    inline TRet ThisCall(const DWORD dwAddress, LPVOID lpData) {
        return ThisCall<TRet>((LPVOID)dwAddress, lpData);
    }

    template<typename TRet, typename ...TArgs>
    inline TRet ThisCall(const DWORD dwAddress, LPVOID lpData, TArgs ...args) {
        return ThisCall<TRet>((LPVOID)dwAddress, lpData, args...);
    }
}