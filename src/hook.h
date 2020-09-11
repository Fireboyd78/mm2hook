#pragma once
#include "common.h"

class cb {
public:
    enum class type : int { jmp, call, push, _count };

    struct info {
        using list = std::initializer_list<info>;

        auto_ptr addr;
        type type;
    };

    template <type _Type>
    struct hook : info {
        constexpr hook(const DWORD addr) : info{ addr, _Type } {};
    };

    using jmp = hook<type::jmp>;
    using call = hook<type::call>;
    using push = hook<type::push>;
};

template <typename TRet, typename... TArgs>
using MethodCall = TRet(*)(TArgs...);

template <typename TRet, class TThis, typename... TArgs>
using MemberCall = TRet(__thiscall *)(const TThis, TArgs...);

template <typename TRet, typename... TArgs>
using StdMethodCall = TRet(__stdcall *)(TArgs...);

template <typename TRet, class TThis, typename... TArgs>
using VirtualCall = TRet(TThis::*)(TArgs...);

namespace hook {
	template <typename TType, bool is_pointer = std::is_pointer<TType>::value, bool is_array = std::is_array<TType>::value>
    class Type {};

    /*
        Hook template for value types
    */
    template <typename TType>
    class Type<TType, false, false> {
    protected:
        TType *lpValue;
    public:
        constexpr Type(int address) : lpValue(reinterpret_cast<TType *>(address)) {};
        
        inline TType & get() const                          { return *lpValue; }
        inline void set(TType value)                        { *lpValue = value; }

        inline TType * ptr() const                          { return lpValue; }

        /*
            Operators
        */

        inline TType * operator->() const                   { return lpValue; };
        inline TType * operator&() const                    { return lpValue; };
        inline TType & operator*() const                    { return *lpValue; };
        inline TType * operator[](int index) const          { return &lpValue[index]; }
        inline TType & operator=(TType value)               { return (*lpValue = value); }

        inline operator TType &() const                     { return *lpValue; }

        /*
            Comparison operators
        */

        inline bool operator==(const TType &rhs) const      { return *lpValue == rhs; }
        inline bool operator!=(const TType &rhs) const      { return *lpValue != rhs; }

        /*
            Value-type operators
        */

        inline bool operator<(const TType &rhs) const       { return *lpValue < rhs; }
        inline bool operator>(const TType &rhs) const       { return *lpValue > rhs; }
        inline bool operator<=(const TType &rhs) const      { return *lpValue <= rhs; }
        inline bool operator>=(const TType &rhs) const      { return *lpValue >= rhs; }
        
        inline TType operator+() const                      { return +(*lpValue); }
        inline TType operator-() const                      { return -(*lpValue); }
        inline TType operator~() const                      { return ~(*lpValue); }
        
        inline TType operator+(const TType &rhs) const      { return *lpValue + rhs; }
        inline TType operator-(const TType &rhs) const      { return *lpValue - rhs; }
        inline TType operator*(const TType &rhs) const      { return *lpValue * rhs; }
        inline TType operator/(const TType &rhs) const      { return *lpValue / rhs; }
        inline TType operator%(const TType &rhs) const      { return *lpValue % rhs; }
        inline TType operator&(const TType &rhs) const      { return *lpValue & rhs; }
        inline TType operator|(const TType &rhs) const      { return *lpValue | rhs; }
        inline TType operator^(const TType &rhs) const      { return *lpValue ^ rhs; }
        inline TType operator<<(const TType &rhs) const     { return *lpValue << rhs; }
        inline TType operator>>(const TType &rhs) const     { return *lpValue >> rhs; }
        
        inline TType operator+=(const TType &rhs)           { return (*lpValue += rhs); }
        inline TType operator-=(const TType &rhs)           { return (*lpValue -= rhs); }
        inline TType operator*=(const TType &rhs)           { return (*lpValue *= rhs); }
        inline TType operator/=(const TType &rhs)           { return (*lpValue /= rhs); }
        inline TType operator%=(const TType &rhs)           { return (*lpValue %= rhs); }
        inline TType operator&=(const TType &rhs)           { return (*lpValue &= rhs); }
        inline TType operator|=(const TType &rhs)           { return (*lpValue |= rhs); }
        inline TType operator^=(const TType &rhs)           { return (*lpValue ^= rhs); }
        inline TType operator<<=(const TType &rhs)          { return (*lpValue <<= rhs); }
        inline TType operator>>=(const TType &rhs)          { return (*lpValue >>= rhs); }
    };

    /*
        Hook template for pointer types
    */
    template <typename TType>
    class Type<TType, true, false> {
    protected:
        TType *lpValue;
    public:
        constexpr Type(int address) : lpValue(reinterpret_cast<TType *>(address)) {};

        inline TType & get() const                          { return *lpValue; }
        inline void set(TType value)                        { *lpValue = value; }

        inline TType * ptr() const                          { return lpValue; }

        /*
            Operators
        */

        inline TType & operator->() const                   { return *lpValue; };
        inline TType * operator&() const                    { return lpValue; };
        inline TType & operator*() const                    { return *lpValue; };
        inline TType operator[](int index) const            { return lpValue[index]; }
        inline TType & operator=(TType value)               { return (*lpValue = value); }

        inline operator TType &() const                     { return *lpValue; }

        /*
            Comparison operators
        */

        inline bool operator==(const TType &rhs) const      { return *lpValue == rhs; }
        inline bool operator!=(const TType &rhs) const      { return *lpValue != rhs; }
        
        inline bool operator==(const std::nullptr_t &rhs) const
                                                            { return *lpValue == nullptr; }
        inline bool operator!=(const std::nullptr_t &rhs) const
                                                            { return *lpValue != nullptr; }

        template <typename... TArgs>
        inline auto operator()(TArgs... args) {
            return (*lpValue)(args...);
        }
    };

    /*
        Hook template for array types
    */
    template <typename TArray>
    class Type<TArray, false, true> {
        /*
            we need all this spaghett to resolve the actual array type
            because the fucking template isn't smart enough to do so
        */

        template <typename _T, int N>
        static constexpr _T _type(_T(*ary)[N]);

        template <typename _T, int N>
        static constexpr int _count(_T(*ary)[N]) {
            return N;
        };

        using type = decltype(_type((TArray *)nullptr));

        template <typename TRet, typename ...TArgs>
        using rtype = TRet;
    protected:
        using TValue = rtype<type>;

        TValue *lpValue;
    public:
        constexpr Type(int address) : lpValue(reinterpret_cast<TValue *>(address)) {};

        inline int count() const {
            return _count((TArray *)nullptr);
        }

        inline TValue * ptr() const                         { return lpValue; }
        inline TValue * ptr(int index) const                { return lpValue + index; }

        /*
            Operators
        */

        inline TValue * operator&() const                   { return lpValue; };
        inline TValue & operator[](int index) const         { return lpValue[index]; }

        template <typename TType>
        inline operator TType *() const                     { return reinterpret_cast<TType *>(lpValue); }
    };

    template <typename TType>
    class TypeProxy {
    protected:
        TType *lpValue;
    public:
        static_assert(!std::is_pointer<TType>::value, "Type proxy cannot be a pointer to a class.");

        constexpr TypeProxy(int address) : lpValue(reinterpret_cast<TType *>(address)) {};

        inline void read(TType &value)              { memcpy(&value, lpValue, sizeof(TType)); }
        inline void write(TType &value)             { memcpy(lpValue, &value, sizeof(TType)); }

        inline TType* operator->() const            { return lpValue; }
        inline TType* operator&() const             { return lpValue; }
        inline TType& operator*() const             { return *lpValue; }
        inline TType& operator[](int index) const   { return &lpValue[index]; }

        inline operator TType*() const              { return lpValue; }
        inline operator TType&() const              { return *lpValue; }
    };

    template<int offset, typename TValue>
    struct Field {
    public:
        template <class TThis>
        static INLINE_CONSTEXPR TValue get(const TThis *p) {
            return *(TValue*)((BYTE*)p + offset);
        };

        template <class TThis>
        static INLINE_CONSTEXPR void set(const TThis *p, TValue value) {
            *(TValue*)((BYTE*)p + offset) = value;
        };

        template <class TThis>
        static INLINE_CONSTEXPR TValue* ptr(const TThis *p) {
            return (TValue*)((BYTE*)p + offset);
        };
    };

    template <int address>
    struct Thunk {
    private:
        template<typename TRet, class TThis, typename ...TArgs>
        static INLINE_CONSTEXPR const TRet _ConstCall(int callback, const TThis *This, TArgs ...args) {
            return (This->*reinterpret_cast<VirtualCall<const TRet, const TThis, TArgs...> &>(callback))(args...);
        };

        template<typename TRet, class TThis, typename ...TArgs>
        static INLINE_CONSTEXPR TRet _ThisCall(int callback, TThis *This, TArgs ...args) {
            return (This->*reinterpret_cast<VirtualCall<TRet, TThis, TArgs...> &>(callback))(args...);
        };
    public:
        template<typename TRet, class TThis, typename ...TArgs>
        static INLINE_CONSTEXPR TRet Call(const TThis &&This, TArgs ...args) {
            return static_cast<MemberCall<TRet, TThis, TArgs...>>(reinterpret_cast<LPVOID>(address))(This, args...);
        };

        template<class TThis, typename ...TArgs>
        static INLINE_CONSTEXPR const TThis Call(const TThis *This, TArgs ...args) {
            return _ConstCall<TThis>(address, This, args...);
        };

        // HACK: call from a pointer to a class...
        template<typename TRet, class TThis, typename ...TArgs>
        static INLINE_CONSTEXPR TRet ThisCall(TThis *This, TArgs ...args) {
            return _ThisCall<TRet>(address, This, args...);
        };
    };

    template <int address>
    struct StaticThunk {
    public:
        template<typename TRet, typename ...TArgs>
        static INLINE_CONSTEXPR TRet Call(TArgs ...args) {
            return static_cast<MethodCall<TRet, TArgs...>>(reinterpret_cast<LPVOID>(address))(args...);
        };
    };

    template <typename TRet>
    class Func {
    protected:
        LPVOID lpFunc;
    public:
        constexpr Func(int address) : lpFunc(reinterpret_cast<LPVOID>(address)) {};

        template<typename ...TArgs>
        INLINE_CONSTEXPR TRet operator()(TArgs ...args) const {
            return static_cast<MethodCall<TRet, TArgs...>>(lpFunc)(args...);
        };

        class StdCall : protected Func<TRet> {
        public:
            constexpr StdCall(int address) : Func<TRet>(address) {};

            template<typename ...TArgs>
            INLINE_CONSTEXPR TRet operator()(TArgs ...args) const {
                return static_cast<StdMethodCall<TRet, TArgs...>>(lpFunc)(args...);
            };

            template <typename TRet, typename... TArgs>
            INLINE_CONSTEXPR operator StdMethodCall<TRet, TArgs...>() const {
                return static_cast<StdMethodCall<TRet, TArgs...>>(lpFunc);
            };
        };

        class ThisCall : protected Func<TRet> {
        public:
            constexpr ThisCall(int address) : Func<TRet>(address) {};

            template<typename ...TArgs, class TThis>
            INLINE_CONSTEXPR TRet operator()(const TThis &&This, TArgs ...args) const {
                return static_cast<MemberCall<TRet, TThis, TArgs...>>(lpFunc)(This, args...);
            };

            template <typename TRet, class TThis, typename... TArgs>
            INLINE_CONSTEXPR operator MemberCall<TRet, TThis, TArgs...>() const {
                return static_cast<MemberCall<TRet, TThis, TArgs...>>(lpFunc);
            };
        };
    };

    template <typename TRet>
    class MemberFunc : protected Func<TRet> {
    public:
        constexpr MemberFunc(int address) : Func<TRet>(address) {};

        template<typename ...TArgs, class TThis>
        INLINE_CONSTEXPR TRet operator()(const TThis &&This, TArgs ...args) const {
            return static_cast<MemberCall<TRet, TThis, TArgs...>>(lpFunc)(This, args...);
        };

        template <typename TRet, class TThis, typename... TArgs>
        INLINE_CONSTEXPR operator MemberCall<TRet, TThis, TArgs...>() const {
            return static_cast<MemberCall<TRet, TThis, TArgs...>>(lpFunc);
        };
    };

    /*
        __cdecl function declaration
    */
    template <typename TRet, typename... TArgs>
    class Func<MethodCall<TRet, TArgs...>> : protected Func<TRet> {
        using MethodCall = MethodCall<TRet, TArgs...>;
    public:
        constexpr Func(int address) : Func<TRet>(address) {};

        INLINE_CONSTEXPR TRet operator()(TArgs ...args) const {
            return static_cast<MethodCall>(lpFunc)(args...);
        };

        INLINE_CONSTEXPR operator MethodCall() const {
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
        constexpr Func(int address) : Func<TRet>(address) {};

        INLINE_CONSTEXPR TRet operator()(TArgs ...args) const {
            return static_cast<StdMethodCall>(lpFunc)(args...);
        };

        INLINE_CONSTEXPR operator StdMethodCall() const {
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
        constexpr Func(int address) : Func<TRet>(address) {};

        INLINE_CONSTEXPR TRet operator()(const TThis &&This, TArgs ...args) const {
            return static_cast<MemberCall>(lpFunc)(args...);
        };

        INLINE_CONSTEXPR operator MemberCall() const {
            return static_cast<MemberCall>(lpFunc);
        };
    };
};

template <typename TType>
using _Type = hook::Type<TType>;

template <typename TType>
using _TypeProxy = hook::TypeProxy<TType>;

template <typename ...TArgs>
using _Func = hook::Func<TArgs...>;

template <typename ...TArgs>
using _MemberFunc = hook::MemberFunc<TArgs...>;

template <int address>
using _Thunk = hook::Thunk<address>;

template <int address>
using _StaticThunk = hook::StaticThunk<address>;