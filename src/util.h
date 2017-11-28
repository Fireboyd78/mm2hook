#pragma once

//
// Useful preprocessor directives
//

#define EXPAND( x ) x
#define STRFY(x) #x

#define ASSERT_SIZEOF(t,s)      static_assert(sizeof(t) <= s, "Size mismatch for type '" STRFY(t) "'; too big (expected " STRFY(s) ")"); \
                                static_assert(sizeof(t) == s, "Size mismatch for type '" STRFY(t) "'; too small (expected " STRFY(s) ")")

#define FASTCALL(method)        void __fastcall method
#define FASTCALL_(type,method)  type __fastcall method

#define STDCALL(method)         HRESULT STDMETHODCALLTYPE method
#define STDCALL_(type,method)   type STDMETHODCALLTYPE method

#define NAKED                   __declspec(naked)

#define NOINLINE                __declspec(noinline)
#define NOTHROW                 __declspec(nothrow)
#define NOVTABLE                __declspec(novtable)

#define INLINE_CONSTEXPR        constexpr __forceinline

// short-hand for NOINLINE
#define NO_INL                  NOINLINE

// hooked __thiscall functions require a 'this' pointer parameter
#define _THIS                   void *This
#define _THIS_                  _THIS,

#define FNPTR(t,m)              t (*m)
#define FNPTR_(t,d,m)           t (d *m)
#define FNPTR_CDECL(t,m)        FNPTR_(t,__cdecl,m)
#define FNPTR_FASTCALL(t,m)     FNPTR_(t,__fastcall,m)
#define FNPTR_STDCALL(t,m)      FNPTR_(t,__stdcall,m)
#define FNPTR_THISCALL(t,m)     FNPTR_(t,__thiscall,m)

//
// Macros for classes with virtual tables
//

#define PUSH_VTABLE_(x)         DWORD x = *(DWORD*)this
#define POP_VTABLE_(x)          *(DWORD*)this = x
#define PUSH_VTABLE()           PUSH_VTABLE_(vtbl)
#define POP_VTABLE()            POP_VTABLE_(vtbl)

//
// Pointer macros for data types
//

#define _PTR(p)                 (DWORD*)p
#define _PTR_(t,p)              (t*)p

#define _OFFPTR(p,o)            (*_PTR(p) + o)

#define PTR(p)                  (DWORD*)p
#define PTR_(p,o)               PTR(((BYTE*)p + o))

//
// Console color macros
//

#define TEXTCOLOR_BLACK             0
#define TEXTCOLOR_BLUE              1
#define TEXTCOLOR_GREEN             2
#define TEXTCOLOR_CYAN              3
#define TEXTCOLOR_RED               4
#define TEXTCOLOR_MAGENTA           5
#define TEXTCOLOR_BROWN             6
#define TEXTCOLOR_LIGHTGRAY         7
#define TEXTCOLOR_DARKGRAY          8
#define TEXTCOLOR_LIGHTBLUE         9
#define TEXTCOLOR_LIGHTGREEN        10
#define TEXTCOLOR_LIGHTCYAN         11
#define TEXTCOLOR_LIGHTRED          12
#define TEXTCOLOR_LIGHTMAGENTA      13
#define TEXTCOLOR_YELLOW            14
#define TEXTCOLOR_WHITE             15

#define BACKCOLOR_BLACK             ( 0  << 4 )
#define BACKCOLOR_BLUE              ( 1  << 4 )
#define BACKCOLOR_GREEN             ( 2  << 4 )
#define BACKCOLOR_CYAN              ( 3  << 4 )
#define BACKCOLOR_RED               ( 4  << 4 )
#define BACKCOLOR_MAGENTA           ( 5  << 4 )
#define BACKCOLOR_BROWN             ( 6  << 4 )
#define BACKCOLOR_LIGHTGRAY         ( 7  << 4 )
#define BACKCOLOR_DARKGRAY          ( 8  << 4 )
#define BACKCOLOR_LIGHTBLUE         ( 9  << 4 )
#define BACKCOLOR_LIGHTGREEN        ( 10 << 4 )
#define BACKCOLOR_LIGHTCYAN         ( 11 << 4 )
#define BACKCOLOR_LIGHTRED          ( 12 << 4 )
#define BACKCOLOR_LIGHTMAGENTA      ( 13 << 4 )
#define BACKCOLOR_YELLOW            ( 14 << 4 )
#define BACKCOLOR_WHITE             ( 15 << 4 )

//
// Pointer templates
//

struct auto_ptr
{
    union {
        const DWORD dwHandler;
        const LPVOID lpHandler;
    };

    template<typename T>
    constexpr auto_ptr(T ptr)
        : dwHandler(*(DWORD*)&ptr) {};

    constexpr auto_ptr(DWORD ptr)
        : dwHandler(ptr) {};

    constexpr operator LPVOID() const {
        return lpHandler;
    };

    constexpr operator DWORD() const {
        return dwHandler;
    };
};

template <typename TR = void>
constexpr inline TR* getPtr(LPVOID p, int offset) {
    return (TR*)((BYTE*)p + offset);
};

template <typename TR = void, class TC>
constexpr inline TR* getPtr(const TC *p, int offset) {
    return (TR*)getPtr((LPVOID)p, offset);
};

template <typename TV>
constexpr inline void setPtr(LPVOID p, int offset, TV value) {
    *(TV*)((BYTE*)p + offset) = value;
};

template <class TC, typename TV>
constexpr inline void setPtr(const TC *p, int offset, TV value) {
    *(TV*)((BYTE*)p + offset) = value;
};

//
// Pointer macros for assembler directives
//

#define __PTR(src)              ptr[src]
#define __PTR_DS(src)           ptr ds:[src]

#define __PTR_(t,src)           t __PTR(src) // <t> ptr[src]
#define __PTR_DS_(t,src)        t __PTR_DS(src) // <t> ptr ds:[src]

#define __BPTR(src)             __PTR_(byte,src) // byte ptr[src]
#define __BPTR_DS(src)          __PTR_DS_(byte,src) // byte ptr ds:[src]

#define __WPTR(src)             __PTR_(word,src) // word ptr[src]
#define __WPTR_DS(src)          __PTR_DS_(word,src) // word ptr ds:[src]

#define __DWPTR(src)            __PTR_(dword,src) // dword ptr[src]
#define __DWPTR_DS(src)         __PTR_DS_(dword,src) // dword ptr ds:[src]

// jump to function at pointer
#define JMP_PTR(x) \
    __asm jmp dword ptr ds:x

//
// Debugging utilities
//

inline void debug(LPCSTR lpOutputStr) {
    OutputDebugStringA(lpOutputStr);
};

void debugf(const char *format, ...);

//
// Empty function templates
//

void    NullSub(void);

int     ReturnNullOrZero(void);
bool    ReturnFalse(void);

//
// Useful helper functions
//

bool LoadSystemLibrary(LPCSTR lpDllName, HMODULE *out);
bool GetHookProcAddress(HMODULE hModule, LPCSTR lpProcName, FARPROC *out);

inline bool file_exists(LPCSTR filename) {
    if (auto file = fopen(filename, "r")) {
        fclose(file);
        return true;
    }
    return false;
}

namespace variadic
{
    template <unsigned long long...>
    constexpr unsigned long long sum = 0;

    template <unsigned long long S, unsigned long long... SS>
    constexpr unsigned long long sum<S, SS...> = S + sum<SS...>;

    // true_for_all
    template <bool...>
    constexpr bool true_for_all = true;

    template <bool B, bool... BB>
    constexpr bool true_for_all<B, BB...> = B && true_for_all<BB...>;

    // true_for_any
    template <bool...>
    constexpr bool true_for_any = false;

    template <bool B, bool... BB>
    constexpr bool true_for_any<B, BB...> = B || true_for_any<BB...>;
}

template <class TBase>
struct base_t {
    typename typedef base_t type;

    template<typename TRet, typename ...TArgs>
    using cast_t = TRet(TBase::*)(TArgs...);

    template<typename TRet, class TClass, typename ...TArgs>
    static constexpr cast_t<TRet, TArgs...> cast(TRet (TClass::*func)(TArgs...)) {
        return static_cast<cast_t<TRet, TArgs...>>(func);
    }
};

struct handler_t {
    template<class T>
    inline T * get(void) {
        return reinterpret_cast<T *>(this);
    }
};