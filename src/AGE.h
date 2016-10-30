#pragma once
#include "common.h"

//
// A.G.E. (Angel Game Engine) common includes
//

#define AGE_API NOINLINE __declspec(dllexport)

//
// DirectX includes
//

// use DIRECTX_VERSION to set some/all of them
// or define them individually, etc.
// otherwise they'll default via the includes
#ifdef DIRECTX_VERSION
# ifndef DIRECT3D_VERSION
#  define DIRECT3D_VERSION DIRECTX_VERSION
# endif
# ifndef DIRECTDRAW_VERSION
#  define DIRECTDRAW_VERSION DIRECTX_VERSION
# endif
# ifndef DIRECTINPUT_VERSION
#  define DIRECTINPUT_VERSION DIRECTX_VERSION
# endif
# ifndef DIRECTSOUND_VERSION
#  define DIRECTSOUND_VERSION DIRECTX_VERSION
# endif
#endif

#include <d3d.h>
#include <ddraw.h>
#include <dinput.h>

#include <mmsystem.h>
#include <dsound.h>

typedef HRESULT(WINAPI *LPFNDIRECTINPUTCREATE)(HINSTANCE, DWORD, LPVOID *, LPUNKNOWN);
typedef HRESULT(WINAPI *LPDIRECTDRAWCREATEEX)(GUID *, LPVOID *, REFIID, IUnknown *);

struct ageInfo
{
    short gameVersion;
    short engineVersion;
    LPCSTR versionString;
};

struct ageInfoLookup {
    DWORD offset;
    bool isSupported;

    ageInfo info;
};

class ageGame {
protected:
    ageInfo m_info;
public:
    ageGame(const ageInfo &gameInfo) : m_info(gameInfo) {};

    ageGame(short gameVersion, short engineVersion)
        : m_info { gameVersion, engineVersion } {};
    ageGame(short gameVersion, short engineVersion, LPCSTR versionString)
        : m_info { gameVersion, engineVersion, versionString } {};

    static bool GetAGEInfo(const ageInfoLookup *infoLookup, ageInfoLookup &outInfo) {
        const ageInfoLookup *l = NULL;
        for (l = infoLookup; l->offset != 0; l++) {
            if (strcmp((LPCSTR)l->offset, l->info.versionString) == 0)
            {
                outInfo = *l;
                return true;
            }
        }
        return false;
    };

    virtual int GetGameVersion() const {
        return m_info.gameVersion;
    };

    virtual int GetEngineVersion() const {
        return m_info.engineVersion;
    };

    virtual void Initialize() {};
};

template <int address>
class AGEHook {
private:
    template <typename TRet, typename... TArgs>
    using MethodCall = TRet(*)(TArgs...);

    template <typename TRet, class TThis, typename... TArgs>
    using MemberCall = TRet(__thiscall *)(const TThis, TArgs...);

    template <typename TRet, typename... TArgs>
    using StdMethodCall = TRet(__stdcall *)(TArgs...);
public:
    static_assert((address != 0), "Address cannot be zero.");

    template <typename TType>
    class Type {
    protected:
        TType *lpValue;
    public:
        constexpr Type() : lpValue((TType*)address) {};

        inline TType* get() const {
            return this->lpValue;
        }

        inline void set(TType value) {
            *this->get() = value;
        }

        /*
            TType*->TMember - Pointer to struct of TType
        */
        inline TType& operator->() const {
            return *this->get();
        };

        /*
            &TType - Instance of TType
        */
        inline TType* operator&() const {
            return this->get();
        };

        /*
            *TType - Pointer to TType
        */
        inline TType& operator*() const {
            return *this->get();
        };


        /*
            TType*(this) - Convert this to TType pointer
        */
        explicit inline operator TType*() const {
            return this->get();
        }

        /*
            TType&(this) - Convert this to TType reference
        */
        inline operator TType&() const {
            return *this->get();
        }

        /*
            TType[0] - Pointer is TType array
        */
        inline TType& operator[](std::size_t index) const {
            return this->get()[index];
        }

        /*
            this() - TType is function pointer
        */
        template <typename... TArgs>
        inline auto operator()(TArgs... args) {
            return (*this->get())(args...);
        }
    };

    template <typename TRet>
    class Func {
    protected:
        LPVOID lpFunc;
    public:
        constexpr Func() : lpFunc((LPVOID)address) {};

        template<typename ...TArgs>
        constexpr TRet operator()(TArgs ...args) const {
            return static_cast<MethodCall<TRet, TArgs...>>(lpFunc)(args...);
        };

        template<typename ...TArgs, class TThis>
        constexpr TRet operator()(const TThis &&This, TArgs ...args) const {
            return static_cast<MemberCall<TRet, TThis, TArgs...>>(lpFunc)(This, args...);
        };

        class StdCall : protected Func<TRet> {
        public:
            template<typename ...TArgs>
            constexpr TRet operator()(TArgs ...args) const {
                return static_cast<StdMethodCall<TRet, TArgs...>>(lpFunc)(args...);
            };
        };
    };

    template <typename TRet>
    class MemberFunc : protected Func<TRet> {
    public:
        template<typename ...TArgs, class TThis>
        constexpr TRet operator()(const TThis &&This, TArgs ...args) const {
            return static_cast<MemberCall<TRet, TThis, TArgs...>>(lpFunc)(This, args...);
        };
    };

    /*
         __cdecl function declaration
    */
    template <typename TRet, typename... TArgs>
    class Func<MethodCall<TRet, TArgs...>> : protected Func<TRet> {
        using MethodCall = MethodCall<TRet, TArgs...>;
    public:
        constexpr TRet operator()(TArgs ...args) const {
            return static_cast<MethodCall>(lpFunc)(args...);
        };

        constexpr operator MethodCall() const {
            return static_cast<MethodCall>(lpFunc);
        };
    };

    /*
        __stdcall function declaration
    */
    template <typename TRet, typename... TArgs>
    class Func<StdMethodCall<TRet, TArgs...>> : protected Func<TRet> {
        using StdMethodCall = StdMethodCall<TRet, TArgs...>;
    public:
        constexpr TRet operator()(TArgs ...args) const {
            return static_cast<StdMethodCall>(lpFunc)(args...);
        };

        constexpr operator StdMethodCall() const {
            return static_cast<StdMethodCall>(lpFunc);
        };
    };

    /*
        __thiscall function declaration
    */
    template <typename TRet, class TThis, typename... TArgs>
    class Func<MemberCall<TRet, TThis, TArgs...>> : protected Func<TRet> {
        using MemberCall = MemberCall<TRet, TThis, TArgs...>;
    public:
        constexpr TRet operator()(const TThis &&This, TArgs ...args) const {
            return static_cast<MemberCall>(lpFunc)(args...);
        };

        constexpr operator MemberCall() const {
            return static_cast<MemberCall>(lpFunc);
        };
    };
};