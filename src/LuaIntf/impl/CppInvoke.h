//
// https://github.com/SteveKChiu/lua-intf
//
// Copyright 2014, Steve K. Chiu <steve.k.chiu@gmail.com>
//
// The MIT License (http://www.opensource.org/licenses/mit-license.php)
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//

template <typename FN>
struct CppCouldBeLambda
{
    static constexpr bool value = std::is_class<FN>::value;
};

template <typename R, typename... P>
struct CppCouldBeLambda <std::function<R(P...)>>
{
    static constexpr bool value = false;
};

template <typename FN>
struct CppLambdaTraits
    : public CppLambdaTraits <decltype(&FN::operator())> {};

template <typename FN, typename R, typename... P>
struct CppLambdaTraits <R(FN::*)(P...) const>
{
    using FunctionType = std::function<R(P...)>;
};

template <typename FN, typename R, typename... P>
struct CppLambdaTraits <R(FN::*)(P...)>
{
    using FunctionType = std::function<R(P...)>;
};

//----------------------------------------------------------------------------

template <typename FN, typename R, typename TUPLE, size_t N, size_t... INDEX>
struct CppDispatchMethod
    : CppDispatchMethod <FN, R, TUPLE, N - 1, N - 1, INDEX...> {};

template <typename FN, typename R, typename TUPLE, size_t... INDEX>
struct CppDispatchMethod <FN, R, TUPLE, 0, INDEX...>
{
    static R call(const FN& func, TUPLE& args)
    {
        return func(std::get<INDEX>(args).value()...);
    }
};

template <typename FN, typename R, typename... P>
struct CppInvokeMethod
{
    using TUPLE = std::tuple<P...>;
    static const size_t N = sizeof...(P);

    static R call(const FN& func, TUPLE& args)
    {
        return CppDispatchMethod<FN, R, TUPLE, N>::call(func, args);
    }

    static int push(lua_State* L, const FN& func, TUPLE& args)
    {
        LuaType<R>::push(L, call(func, args));
        return 1;
    }
};

template <typename FN, typename... P>
struct CppInvokeMethod <FN, void, P...>
{
    using TUPLE = std::tuple<P...>;
    static const size_t N = sizeof...(P);

    static void call(const FN& func, TUPLE& args)
    {
        CppDispatchMethod<FN, void, TUPLE, N>::call(func, args);
    }

    static int push(lua_State*, const FN& func, TUPLE& args)
    {
        call(func, args);
        return 0;
    }
};

template <typename FN, typename... P>
struct CppInvokeMethod <FN, int, CppArgHolder<lua_State*>, P...>
{
    static int call(const FN& func, std::tuple<CppArgHolder<lua_State*>, P...>& args)
    {
        return CppDispatchMethod<FN, int, std::tuple<CppArgHolder<lua_State*>, P...>, sizeof...(P)+1>::call(func, args);
    }

    static int push(lua_State*, const FN& func, std::tuple<CppArgHolder<lua_State*>, P...>& args)
    {
        return call(func, args);
    }
};

template <typename FN, typename... P>
struct CppInvokeMethod <FN, int, CppArgHolder<LuaState>, P...>
{
    static int call(const FN& func, std::tuple<CppArgHolder<LuaState>, P...>& args)
    {
        return CppDispatchMethod<FN, int, std::tuple<CppArgHolder<LuaState>, P...>, sizeof...(P)+1>::call(func, args);
    }

    static int push(lua_State*, const FN& func, std::tuple<CppArgHolder<LuaState>, P...>& args)
    {
        return call(func, args);
    }
};

template <typename FN, typename... RP, typename... P>
struct CppInvokeMethod <FN, std::tuple<RP...>, P...>
{
    using TUPLE = std::tuple<P...>;
    using R = std::tuple<RP...>;

    static const size_t N = sizeof...(P);

    static R call(const FN& func, TUPLE& args)
    {
        return CppDispatchMethod<FN, R, TUPLE, N>::call(func, args);
    }

    static int push(lua_State* L, const FN& func, TUPLE& args)
    {
        R ret = call(func, args);
        return pushTuple<0, RP...>(L, ret);
    }

private:
    template <size_t INDEX, typename RV0, typename... RV>
    static int pushTuple(lua_State* L, const R& ret)
    {
        LuaType<RV0>::push(L, std::get<INDEX>(ret));
        return 1 + pushTuple<INDEX + 1, RV...>(L, ret);
    }

    template <size_t INDEX>
    static int pushTuple(lua_State*, const R&)
    {
        return 0;
    }
};

//----------------------------------------------------------------------------

template <typename T, typename TUPLE, size_t N, size_t... INDEX>
struct CppDispatchClassConstructor
    : CppDispatchClassConstructor <T, TUPLE, N - 1, N - 1, INDEX...> {};

template <typename T, typename TUPLE, size_t... INDEX>
struct CppDispatchClassConstructor <T, TUPLE, 0, INDEX...>
{
    static T* call(TUPLE& args)
    {
        return new T(std::get<INDEX>(args).value()...);
    }

    static T* call(void* mem, TUPLE& args)
    {
        return ::new (mem) T(std::get<INDEX>(args).value()...);
    }
};

template <typename T>
struct CppInvokeClassConstructor
{
    template <typename... P>
    using TUPLE = std::tuple<P...>;

    template <typename... P>
    using DispatchConstructor = CppDispatchClassConstructor<T, TUPLE<P...>, sizeof...(P)>;

    template <typename... P>
    static T* call(TUPLE<P...>& args)
    {
        return DispatchConstructor<P...>::call(args);
    }

    template <typename... P>
    static T* call(void* mem, TUPLE<P...>& args)
    {
        return DispatchConstructor<P...>::call(mem, args);
    }
};

//----------------------------------------------------------------------------

template <typename T, bool IS_PROXY, typename FN, typename R, typename TUPLE, size_t N, size_t... INDEX>
struct CppDispatchClassMethod
    : CppDispatchClassMethod <T, IS_PROXY, FN, R, TUPLE, N - 1, N - 1, INDEX...> {};

template <typename T, typename FN, typename R, typename TUPLE, size_t... INDEX>
struct CppDispatchClassMethod <T, false, FN, R, TUPLE, 0, INDEX...>
{
    static R call(T* t, const FN& fn, TUPLE& args)
    {
        return (t->*fn)(std::get<INDEX>(args).value()...);
    }
};

template <typename T, typename FN, typename R, typename TUPLE, size_t... INDEX>
struct CppDispatchClassMethod <T, true, FN, R, TUPLE, 0, INDEX...>
{
    static R call(T* t, const FN& fn, TUPLE& args)
    {
        return fn(t, std::get<INDEX>(args).value()...);
    }
};

template <typename T, bool IS_PROXY, typename FN, typename R, typename... P>
struct CppInvokeClassMethod
{
    using TUPLE = std::tuple<P...>;
    static const size_t N = sizeof...(P);

    static R call(T* t, const FN& func, TUPLE& args)
    {
        return CppDispatchClassMethod<T, IS_PROXY, FN, R, TUPLE, N>::call(t, func, args);
    }

    static int push(lua_State* L, T* t, const FN& func, TUPLE& args)
    {
        LuaType<R>::push(L, call(t, func, args));
        return 1;
    }
};

template <typename T, bool IS_PROXY, typename FN, typename... P>
struct CppInvokeClassMethod <T, IS_PROXY, FN, void, P...>
{
    using TUPLE = std::tuple<P...>;
    static const size_t N = sizeof...(P);

    static void call(T* t, const FN& func, TUPLE& args)
    {
        CppDispatchClassMethod<T, IS_PROXY, FN, void, TUPLE, N>::call(t, func, args);
    }

    static int push(lua_State*, T* t, const FN& func, TUPLE& args)
    {
        call(t, func, args);
        return 0;
    }
};

template <typename T, bool IS_PROXY, typename FN, typename... P>
struct CppInvokeClassMethod <T, IS_PROXY, FN, int, CppArgHolder<lua_State*>, P...>
{
    static int call(T* t, const FN& func, std::tuple<CppArgHolder<lua_State*>, P...>& args)
    {
        return CppDispatchClassMethod<T, IS_PROXY, FN, int, std::tuple<CppArgHolder<lua_State*>, P...>, sizeof...(P)+1>::call(t, func, args);
    }

    static int push(lua_State*, T* t, const FN& func, std::tuple<CppArgHolder<lua_State*>, P...>& args)
    {
        return call(t, func, args);
    }
};

template <typename T, bool IS_PROXY, typename FN, typename... P>
struct CppInvokeClassMethod <T, IS_PROXY, FN, int, CppArgHolder<LuaState>, P...>
{
    static int call(T* t, const FN& func, std::tuple<CppArgHolder<LuaState>, P...>& args)
    {
        return CppDispatchClassMethod<T, IS_PROXY, FN, int, std::tuple<CppArgHolder<LuaState>, P...>, sizeof...(P)+1>::call(t, func, args);
    }

    static int push(lua_State*, T* t, const FN& func, std::tuple<CppArgHolder<LuaState>, P...>& args)
    {
        return call(t, func, args);
    }
};

template <typename T, bool IS_PROXY, typename FN, typename... RP, typename... P>
struct CppInvokeClassMethod <T, IS_PROXY, FN, std::tuple<RP...>, P...>
{
    using TUPLE = std::tuple<P...>;
    using R = std::tuple<RP...>;

    static const size_t N = sizeof...(P);

    static R call(T* t, const FN& func, TUPLE& args)
    {
        return CppDispatchClassMethod<T, IS_PROXY, FN, R, TUPLE, N>::call(t, func, args);
    }

    static int push(lua_State* L, T* t, const FN& func, TUPLE& args)
    {
        R ret = call(t, func, args);
        return pushTuple<0, RP...>(L, ret);
    }

private:
    template <size_t INDEX, typename RV0, typename... RV>
    static int pushTuple(lua_State* L, const R& ret)
    {
        LuaType<RV0>::push(L, std::get<INDEX>(ret));
        return 1 + pushTuple<INDEX + 1, RV...>(L, ret);
    }

    template <size_t INDEX>
    static int pushTuple(lua_State*, const R&)
    {
        return 0;
    }
};
