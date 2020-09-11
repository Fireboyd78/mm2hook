#pragma once
#include <modules\data.h>

namespace MM2
{
    // Forward declarations
    class datCallback;

    // External declarations
    extern class Base;

    // Class definitions

    namespace $
    {
        namespace datCallback
        {
            declhook(0x4C7A40, _MemberFunc<void>, $$ctor$void);

            declhook(0x4C7A50, _MemberFunc<void>, $$ctor$ThisCB$1);
            declhook(0x4C7A80, _MemberFunc<void>, $$ctor$ThisCB$2);
            declhook(0x4C7AA0, _MemberFunc<void>, $$ctor$ThisCB$3);

            declhook(0x4C7AC0, _MemberFunc<void>, $$ctor$CB$1);
            declhook(0x4C7AF0, _MemberFunc<void>, $$ctor$CB$2);
            declhook(0x4C7B20, _MemberFunc<void>, $$ctor$CB$3);

            declhook(0x4C7B50, _MemberFunc<void>, Call);
        }
    }

    class datCallback
    {
    protected:
        enum Flags
        {
            ParamCount0 = 0x40000000,
            ParamCount1 = 0x80000000,
            ParamCount2 = 0xC0000000,
            ParamCountFlags = ParamCount0 | ParamCount1 | ParamCount2
        };

        Base* _class;
        unsigned int _callback;
        void* _parameter;

        unsigned int _get_flags()
        {
            return _callback & ParamCountFlags;
        }

        unsigned int _get_callback()
        {
            return _callback & ~ParamCountFlags;
        }

        unsigned int _combine_callback(void* callback, unsigned int flags)
        {
            return reinterpret_cast<unsigned int&>(callback) | flags;
        }

        template <typename ...TArgs>
        void virtual_callback(uint callback, TArgs ...args) const {
            (_class->*reinterpret_cast<VirtualCall<void, Base, TArgs...> &>(callback))(args...);
        }

        template <typename ...TArgs>
        void method_callback(uint callback, TArgs ...args) const {
            reinterpret_cast<MethodCall<void, TArgs...> &>(callback)(args...);
        }

    public:
        static hook::TypeProxy<datCallback> NullCallback;

        inline unsigned int ptr() const {
            return _callback & ~ParamCountFlags;
        }

        AGE_API datCallback()
            : _class(NULL)
            , _callback(NULL)
            , _parameter(NULL)
        { }

        AGE_API datCallback(void(*callback)())
            : _class((Base*)callback)
            , _callback(0x4C7BE3 | ParamCount0)
            , _parameter(NULL)
        { }
        
        AGE_API datCallback(void(__stdcall *callback)(void*), void* parameter)
            : _class((Base*)callback)
            , _callback(0x4C7BE3 | ParamCount1)
            , _parameter(parameter)
        { }

        AGE_API datCallback(void(__stdcall *callback)())
            : _class((Base*)callback)
            , _callback(0x4C7BE3 | ParamCount0)
            , _parameter(NULL)
        { }

        AGE_API datCallback(void(__stdcall *callback)(void*, void*), void* parameter)
            : _class((Base*)callback)
            , _callback(0x4C7BE3 | ParamCount2)
            , _parameter(parameter)
        { }

        AGE_API void Call(void* parameter)
        {
            auto callback = _get_callback();
            auto flags = _get_flags();

            if (flags)
            {
                if (_class)
                {
                    switch (flags)
                    {
                        case ParamCount0: return virtual_callback(callback);
                        case ParamCount1: return virtual_callback(callback, _parameter);
                        case ParamCount2: return virtual_callback(callback, _parameter, parameter);
                    }
                } else
                {
                    switch (flags)
                    {
                        case ParamCount0: return method_callback(callback);
                        case ParamCount1: return method_callback(callback, _parameter);
                        case ParamCount2: return method_callback(callback, _parameter, parameter);
                    }
                }
            }
        }

        //lua
        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<datCallback>("datCallback")
                .endClass();
        }
    };

    ASSERT_SIZEOF(datCallback, 0xC);

    // Lua initialization

}