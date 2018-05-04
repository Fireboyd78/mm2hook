#pragma once
#include "mm2_common.h"
#include "mm2_stream.h"

namespace MM2
{
    // Forward declarations
    extern class datArgParser;
    extern class datAssetManager;
    extern class datCallback;
    extern class datOutput;
    extern class datParser;
    extern class datParserRecord;
    extern class datTimeManager;

    // External declarations
    extern class Base;
    extern class Stream;

    namespace $
    {
        namespace datArgParser
        {
            declhook(0x4C6190, _Func<bool>, Get_Any);
            declhook(0x4C61C0, _Func<bool>, Get_Int);
            declhook(0x4C6210, _Func<bool>, Get_Float);
            declhook(0x4C6260, _Func<bool>, Get_String);

            declhook(0x4C62A0, _Func<int>, GetArgCount);
        }
        namespace datAssetManager
        {
            declhook(0x4C55E0, _Func<void>, FullPath$1);
            declhook(0x4C56F0, _Func<void>, FullPath$2);
            declhook(0x4C59B0, _Func<bool>, Exists$1);
            declhook(0x4C59E0, _Func<bool>, Exists$2);
        }
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
        namespace datParser
        {
            declhook(0x4A78E0, _MemberFunc<void>, $$ctor);
            declhook(0x4A7920, _MemberFunc<void>, $$dtor);

            declhook(0x4A7980, _MemberFunc<MM2::datParser *>, AddParser);
            declhook(0x4A79D0, _MemberFunc<MM2::datParserRecord &>, AddRecord);

            declhook(0x4A7A90, _MemberFunc<bool>, Load$1);
            declhook(0x4A7AF0, _MemberFunc<bool>, Load$2);
            declhook(0x4A7B40, _MemberFunc<bool>, Load$3);

            declhook(0x4A7B90, _MemberFunc<bool>, Save$1);
            declhook(0x4A7C00, _MemberFunc<bool>, Save$2);
            declhook(0x4A7C50, _MemberFunc<bool>, Save$3);
        }
        namespace datTimeManager
        {
            declhook(0x4C6300, _Func<void>, Reset);
            declhook(0x4C6340, _Func<void>, Update);
        }
    }

    class datArgParser {
    public:
        static ageHook::Type<int> Argc;
        static ageHook::Type<char **> Argv;

        AGE_API static bool Get(LPCSTR arg) {
            return $::datArgParser::Get_Any(arg);
        };

        AGE_API static bool Get(LPCSTR arg, UINT index, int *out) {
            return $::datArgParser::Get_Int(arg, index, out);
        };

        AGE_API static bool Get(LPCSTR arg, UINT index, float *out) {
            return $::datArgParser::Get_Float(arg, index, out);
        };

        AGE_API static bool Get(LPCSTR arg, UINT index, LPCSTR *out) {
            return $::datArgParser::Get_String(arg, index, out);
        };

        AGE_API static int GetArgCount(LPCSTR arg) {
            return $::datArgParser::GetArgCount(arg);
        };
    };

    class datAssetManager {
    public:
        /* TODO?
        static Stream * Open(char const *,char const *,bool,bool);
        static Stream * Open(char const *,char const *,char const *,bool,bool);
        static Stream * Create(char const *,char const *,bool);
        static Stream * Create(char const *,char const *,char const *,bool);
        */

        AGE_API static void FullPath(char *buffer, int length, LPCSTR directory, LPCSTR filename) {
            $::datAssetManager::FullPath$1(buffer, length, directory, filename);
        };

        AGE_API static void FullPath(char *buffer, int length, LPCSTR directory, LPCSTR filename, LPCSTR extension) {
            $::datAssetManager::FullPath$2(buffer, length, directory, filename, extension);
        };

        // these don't work for files outside of archives
        AGE_API static bool Exists(LPCSTR directory, LPCSTR filename) {
            return $::datAssetManager::Exists$1(directory, filename);
        };

        AGE_API static bool Exists(LPCSTR directory, LPCSTR filename, LPCSTR extension) {
            return $::datAssetManager::Exists$2(directory, filename, extension);
        };
    };

    /*
        Callback type definitions
    */

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
        static ageHook::TypeProxy<datCallback> NullCallback;

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
    };

    class datOutput {
    public:
        static ageHook::Type<Stream *> sm_Stream;

        static ageHook::Type<bool> ShowPopupErrors;
        static ageHook::Type<bool> ShowPopupQuits;

        static ageHook::Type<bool> OutputSent;

        AGE_API static bool OpenLog(LPCSTR filename) {
            return ageHook::StaticThunk<0x4C95A0>::Call<bool>(filename);
        };

        AGE_API static bool OpenLog(LPCSTR filename, const coreFileMethods *fileMethods) {
            return ((sm_Stream = Stream::Create(filename, fileMethods)) != NULL);
        };

        AGE_API static void CloseLog(void) {
            ageHook::StaticThunk<0x4C9530>::Call<void>();
        };

        AGE_API static void CallBeforeMsgBoxFunction(void) {
            ageHook::StaticThunk<0x4C9570>::Call<void>();
        }

        AGE_API static void CallAfterMsgBoxFunction(void) {
            ageHook::StaticThunk<0x4C9580>::Call<void>();
        }

        AGE_API static void SetBeforeMsgBoxFunction(void (*lpFunc)(void)) {
            ageHook::StaticThunk<0x4C9550>::Call<void>(lpFunc);
        }

        AGE_API static void SetAfterMsgBoxFunction(void (*lpFunc)(void)) {
            ageHook::StaticThunk<0x4C9560>::Call<void>(lpFunc);
        }
        
        AGE_API static void SetOutputMask(UINT mask) {
            ageHook::StaticThunk<0x4C9590>::Call<void>(mask);
        };

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<datOutput>("datOutput")
                .addStaticFunction("SetOutputMask", &SetOutputMask)
            .endClass();
        }
    };

    class datParserRecord {
    private:
        /*
            This class is NOT meant to be manipulated outside of datParser
            Hopefully by making this private we'll minimize any possible bugs
        */
        datParserRecord() {};
    public:
        int type;

        short dataCount;
        short dataLength;

        char name[64];

        void *dataPtr;
        void *callback; // unused

        // simple yet effective linked list:
        // parser
        // > record
        //  > record
        //   > record
        //    > (etc.)
        // parser will loop through each record and its children
        // once the destructor is called, any children below it are destroyed as well
        datParserRecord *child;
    };

    class datParser {
    private:
        char name[64];

        int numRecords;
        int _unk_44; // doesn't get initialized nor used?

        datParserRecord *record;

        int _unk_4c; // set to zero but never used?
        int indentLevel;

        /*
            Record types:

            0 - RawToken (no tabs/spaces allowed -- e.g. "MyProperty token_value")
            1 - Bool
            2 - Byte
            3 - Short
            4 - Int
            5 - Float
            6 - Vector2
            7 - Vector3
            8 - Vector4
            9 - Parser (datParser)
        */
        AGE_API datParserRecord & AddRecord(int type, LPCSTR name, void *dataPtr, int count) {
            // last parameter is the 'callback' which is never actually used
            return $::datParser::AddRecord(this, type, name, dataPtr, count, NULL);
        };

        inline datParserRecord & AddRecord(int type, LPCSTR name, void *dataPtr) {
            return AddRecord(type, name, dataPtr, 1);
        };
    public:
        AGE_API datParser(const char *filename) {
            $::datParser::$$ctor(this, filename);
        };

        AGE_API ~datParser() {
            $::datParser::$$dtor(this);
        };

        AGE_API datParser * AddParser(LPCSTR name) {
            // 'callback' unused (see AddRecord above)
            return $::datParser::AddParser(this, name, NULL);
        };

        void AddToken(LPCSTR name, char *tokenPtr, int tokenLength) {
            auto &record = AddRecord(0, name, tokenPtr);

            // MM2 doesn't have a way of adding this kind of record
            // so we need to manually set it here
            // (otherwise it won't work)
            record.dataLength = tokenLength;
        };

        template <int N>
        inline void AddString(LPCSTR name, char (*tokenPtr)[N]) {
            AddToken(name, tokenPtr[0], N);
        }

        /*
            Support for adding arrays of values
        */

        AGE_API void AddValue(LPCSTR name, bool *values, int count)     { AddRecord(1, name, values, count); };
        AGE_API void AddValue(LPCSTR name, char *values, int count)     { AddRecord(2, name, values, count); };
        AGE_API void AddValue(LPCSTR name, short *values, int count)    { AddRecord(3, name, values, count); };
        AGE_API void AddValue(LPCSTR name, int *values, int count)      { AddRecord(4, name, values, count); };
        AGE_API void AddValue(LPCSTR name, float *values, int count)    { AddRecord(5, name, values, count); };
        AGE_API void AddValue(LPCSTR name, Vector2 *values, int count)  { AddRecord(6, name, values, count); };
        AGE_API void AddValue(LPCSTR name, Vector3 *values, int count)  { AddRecord(7, name, values, count); };
        AGE_API void AddValue(LPCSTR name, Vector4 *values, int count)  { AddRecord(8, name, values, count); };

        /*
            Inline methods for adding a single value instead of an array
        */

        inline void AddValue(LPCSTR name, bool *value)      { AddValue(name, value, 1); };
        inline void AddValue(LPCSTR name, char *value)      { AddValue(name, value, 1); };
        inline void AddValue(LPCSTR name, short *value)     { AddValue(name, value, 1); };
        inline void AddValue(LPCSTR name, int *value)       { AddValue(name, value, 1); };
        inline void AddValue(LPCSTR name, float *value)     { AddValue(name, value, 1); };
        inline void AddValue(LPCSTR name, Vector2 *value)   { AddValue(name, value, 1); };
        inline void AddValue(LPCSTR name, Vector3 *value)   { AddValue(name, value, 1); };
        inline void AddValue(LPCSTR name, Vector4 *value)   { AddValue(name, value, 1); };

        AGE_API bool Load(Stream *stream, LPCSTR filename) {
            return $::datParser::Load$1(this, stream, filename);
        };

        AGE_API bool Load(LPCSTR directory, LPCSTR filename) {
            return $::datParser::Load$2(this, directory, filename);
        };

        AGE_API bool Load(LPCSTR directory, LPCSTR filename, LPCSTR extension) {
            return $::datParser::Load$3(this, directory, filename, extension);
        };

        /*
            Binary saving is indefinitely disabled since it's bugged
        */

        AGE_API bool Save(Stream *stream, LPCSTR filename) {
            return $::datParser::Save$1(this, stream, filename, false);
        };

        AGE_API bool Save(LPCSTR directory, LPCSTR filename) {
            return $::datParser::Save$2(this, directory, filename, false);
        };

        AGE_API bool Save(LPCSTR directory, LPCSTR filename, LPCSTR extension) {
            return $::datParser::Save$3(this, directory, filename, extension, false);
        };
    };

    class datTimeManager {
    public:
        AGE_API static void Reset(void) {
            $::datTimeManager::Reset();
        };

        AGE_API static void Update(void) {
            $::datTimeManager::Update();
        };
    };

    template<>
    void luaAddModule<module_data>(LuaState L) {
        luaBind<datOutput>(L);
    }
}