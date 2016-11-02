#pragma once
#include "mm2_common.h"

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

    class datArgParser {
    protected:
        static AGEHook<0x4C6190>::Func<bool> $Get_Any;
        static AGEHook<0x4C61C0>::Func<bool> $Get_Int;
        static AGEHook<0x4C6210>::Func<bool> $Get_Float;
        static AGEHook<0x4C6260>::Func<bool> $Get_String;
        static AGEHook<0x4C62A0>::Func<unsigned> $Get_WordCount;
    public:
        AGE_API static bool Get(LPCSTR arg) {
            return $Get_Any(arg);
        };

        AGE_API static bool Get(LPCSTR arg, UINT index, int *out) {
            return $Get_Int(arg, index, out);
        };

        AGE_API static bool Get(LPCSTR arg, UINT index, float *out) {
            return $Get_Float(arg, index, out);
        };

        AGE_API static bool Get(LPCSTR arg, UINT index, LPCSTR *out) {
            return $Get_String(arg, index, out);
        };

        AGE_API static unsigned GetWordCount(LPCSTR arg) {
            return $Get_WordCount(arg);
        };
    };

    class datAssetManager {
    protected:
        static AGEHook<0x4C55E0>::Func<void> $FullPath_$1;
        static AGEHook<0x4C56F0>::Func<void> $FullPath_$2;
        static AGEHook<0x4C59B0>::Func<bool> $Exists_$1;
        static AGEHook<0x4C59E0>::Func<bool> $Exists_$2;
    public:
        /* TODO?
        static Stream * Open(char const *,char const *,bool,bool);
        static Stream * Open(char const *,char const *,char const *,bool,bool);
        static Stream * Create(char const *,char const *,bool);
        static Stream * Create(char const *,char const *,char const *,bool);
        */

        AGE_API static void FullPath(char *buffer, int length, LPCSTR directory, LPCSTR filename) {
            $FullPath_$1(buffer, length, directory, filename);
        };

        AGE_API static void FullPath(char *buffer, int length, LPCSTR directory, LPCSTR filename, LPCSTR extension) {
            $FullPath_$2(buffer, length, directory, filename, extension);
        };

        // these don't work for files outside of archives
        AGE_API static bool Exists(LPCSTR directory, LPCSTR filename) {
            return $Exists_$1(directory, filename);
        };

        AGE_API static bool Exists(LPCSTR directory, LPCSTR filename, LPCSTR extension) {
            return $Exists_$2(directory, filename, extension);
        };
    };

    /*
        Callback type definitions
    */

    typedef void(__thiscall Base::*LPDATCALLBACK_THIS)(void);
    typedef void(__thiscall Base::*LPDATCALLBACK_THIS_1)(void *);
    typedef void(__thiscall Base::*LPDATCALLBACK_THIS_2)(void *, void *);

    typedef void(*LPDATCALLBACK)(void);
    typedef void(*LPDATCALLBACK_1)(void *);
    typedef void(*LPDATCALLBACK_2)(void *, void *);

    class datCallback {
    private:
        Base *base; // when not null, means this callback uses __thiscall

        /*
            by default, flags stored in upper 24 bits (mask: 0x3000000)

            BUG: hooked function callbacks will FAIL if our function is above 0xFFFFFF
            TODO: make a patch to fix crashes for hooked functions
        */
        unsigned int callback;

        /*
            this one is a bit confusing, but it's always guaranteed to be the first argument
            if 'base' is not null, this will be the "this" object in addition to the rules above
        */
        void *context;
    protected:
        static AGEHook<0x4C7A40>::MemberFunc<void> $$ctor$void;

        static AGEHook<0x4C7A50>::MemberFunc<void> $$ctor$ThisCB_$1;
        static AGEHook<0x4C7A80>::MemberFunc<void> $$ctor$ThisCB_$2;
        static AGEHook<0x4C7AA0>::MemberFunc<void> $$ctor$ThisCB_$3;

        static AGEHook<0x4C7AC0>::MemberFunc<void> $$ctor$CB_$1;
        static AGEHook<0x4C7AF0>::MemberFunc<void> $$ctor$CB_$2;
        static AGEHook<0x4C7B20>::MemberFunc<void> $$ctor$CB_$3;

        static AGEHook<0x4C7B50>::MemberFunc<void> $Call;
    public:
        AGE_API datCallback(void) {
            $$ctor$void(this);
        };

        AGE_API datCallback(LPDATCALLBACK_THIS callback, Base *base) {
            $$ctor$ThisCB_$1(this, callback, base);
        };
        AGE_API datCallback(LPDATCALLBACK_THIS_1 callback, Base *base, void *context) {
            $$ctor$ThisCB_$2(this, callback, base, context);
        };
        AGE_API datCallback(LPDATCALLBACK_THIS_2 callback, Base *base, void *context) {
            $$ctor$ThisCB_$3(this, callback, base, context);
        };

        AGE_API datCallback(LPDATCALLBACK callback, Base *base) {
            $$ctor$CB_$1(this, callback, base);
        };
        AGE_API datCallback(LPDATCALLBACK_1 callback, Base *base, void *context) {
            $$ctor$CB_$2(this, callback, base, context);
        };
        AGE_API datCallback(LPDATCALLBACK_2 callback, Base *base, void *context) {
            $$ctor$CB_$3(this, callback, base, context);
        };

        AGE_API void Call(void *arg) {
            $Call(this, arg);
        };
    };

    class datOutput {
    protected:
        static AGEHook<0x4C9530>::Func<void> $CloseLog;
        static AGEHook<0x4C9590>::Func<bool> $OpenLog;
        static AGEHook<0x4C95A0>::Func<void> $SetOutputMask;
    public:
        static AGEHook<0x6A3D40>::Type<Stream *> sm_Stream;

        AGE_API static bool OpenLog(LPCSTR filename) {
            return $OpenLog(filename);
        };

        AGE_API static void CloseLog(void) {
            $CloseLog();
        };

        /* TODO: Add these?
        static void SetBeforeMsgBoxFunction(void(__cdecl *lpFunc)(void));
        static void SetAfterMsgBoxFunction(void(__cdecl *lpFunc)(void));
        */

        AGE_API static void SetOutputMask(UINT mask) {
            $SetOutputMask(mask);
        };
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
    protected:
        static AGEHook<0x4A78E0>::MemberFunc<void> $$ctor;
        static AGEHook<0x4A7920>::MemberFunc<void> $$dtor;

        static AGEHook<0x4A7980>::MemberFunc<datParser *> $AddParser;
        static AGEHook<0x4A79D0>::MemberFunc<datParserRecord &> $AddRecord;

        static AGEHook<0x4A7A90>::MemberFunc<bool> $Load_$1;
        static AGEHook<0x4A7AF0>::MemberFunc<bool> $Load_$2;
        static AGEHook<0x4A7B40>::MemberFunc<bool> $Load_$3;

        static AGEHook<0x4A7B90>::MemberFunc<bool> $Save_$1;
        static AGEHook<0x4A7C00>::MemberFunc<bool> $Save_$2;
        static AGEHook<0x4A7C50>::MemberFunc<bool> $Save_$3;
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
            return $AddRecord(this, type, name, dataPtr, count, NULL);
        };

        inline datParserRecord & AddRecord(int type, LPCSTR name, void *dataPtr) {
            return AddRecord(type, name, dataPtr, 1);
        };
    public:
        AGE_API datParser(const char *filename) {
            $$ctor(this, filename);
        };

        AGE_API ~datParser() {
            $$dtor(this);
        };

        AGE_API datParser * AddParser(LPCSTR name) {
            // 'callback' unused (see AddRecord above)
            return $AddParser(this, name, NULL);
        };

        void AddToken(LPCSTR name, char *tokenPtr, int tokenLength) {
            auto record = AddRecord(0, name, tokenPtr);

            // MM2 doesn't have a way of adding this kind of record
            // so we need to manually set it here
            // (otherwise it won't work)
            record.dataLength = tokenLength;
        };

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
            return $Load_$1(this, stream, filename);
        };

        AGE_API bool Load(LPCSTR directory, LPCSTR filename) {
            return $Load_$2(this, directory, filename);
        };

        AGE_API bool Load(LPCSTR directory, LPCSTR filename, LPCSTR extension) {
            return $Load_$3(this, directory, filename, extension);
        };

        /*
            Binary saving is indefinitely disabled since it's bugged
        */

        AGE_API bool Save(Stream *stream, LPCSTR filename) {
            return $Save_$1(this, stream, filename, false);
        };

        AGE_API bool Save(LPCSTR directory, LPCSTR filename) {
            return $Save_$2(this, directory, filename, false);
        };

        AGE_API bool Save(LPCSTR directory, LPCSTR filename, LPCSTR extension) {
            return $Save_$3(this, directory, filename, extension, false);
        };
    };

    class datTimeManager {
    protected:
        static AGEHook<0x4C6300>::Func<void> $Reset;
        static AGEHook<0x4C6340>::Func<void> $Update;
    public:
        AGE_API static void Reset(void) {
            $Reset();
        };

        AGE_API static void Update(void) {
            $Update();
        };
    };
}
