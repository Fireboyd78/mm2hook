#pragma once
#include "mm2.h"

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
        static MM2FnHook<bool> $Get_$1;
        static MM2FnHook<bool> $Get_$2;
        static MM2FnHook<bool> $Get_$3;
        static MM2FnHook<bool> $Get_$4;
    public:
        static bool Get(LPCSTR arg) {
            return $Get_$1(arg);
        };

        static bool Get(LPCSTR arg, UINT index, int *out) {
            return $Get_$2(arg, index, out);
        };

        static bool Get(LPCSTR arg, UINT index, float *out) {
            return $Get_$3(arg, index, out);
        };

        static bool Get(LPCSTR arg, UINT index, LPCSTR *out) {
            return $Get_$4(arg, index, out);
        };
    };

    class datAssetManager {
    protected:
        static MM2FnHook<void> $FullPath_$1;
        static MM2FnHook<void> $FullPath_$2;
        static MM2FnHook<bool> $Exists_$1;
        static MM2FnHook<bool> $Exists_$2;
    public:
        /* TODO?
        static Stream * Open(char const *,char const *,bool,bool);
        static Stream * Open(char const *,char const *,char const *,bool,bool);
        static Stream * Create(char const *,char const *,bool);
        static Stream * Create(char const *,char const *,char const *,bool);
        */

        static void FullPath(char *buffer, int length, LPCSTR directory, LPCSTR filename) {
            $FullPath_$1(buffer, length, directory, filename);
        };

        static void FullPath(char *buffer, int length, LPCSTR directory, LPCSTR filename, LPCSTR extension) {
            $FullPath_$2(buffer, length, directory, filename, extension);
        };

        // these don't work for files outside of archives
        static bool Exists(LPCSTR directory, LPCSTR filename) {
            return $Exists_$1(directory, filename);
        };

        static bool Exists(LPCSTR directory, LPCSTR filename, LPCSTR extension) {
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
        static MM2FnHook<void> $$ctor$void;

        static MM2FnHook<void> $$ctor$ThisCB_$1;
        static MM2FnHook<void> $$ctor$ThisCB_$2;
        static MM2FnHook<void> $$ctor$ThisCB_$3;

        static MM2FnHook<void> $$ctor$CB_$1;
        static MM2FnHook<void> $$ctor$CB_$2;
        static MM2FnHook<void> $$ctor$CB_$3;

        static MM2FnHook<void> $Call;
    public:
        datCallback(void) {
            $$ctor$void(this);
        };

        datCallback(LPDATCALLBACK_THIS callback, Base *base) {
            $$ctor$ThisCB_$1(this, callback, base);
        };
        datCallback(LPDATCALLBACK_THIS_1 callback, Base *base, void *context) {
            $$ctor$ThisCB_$2(this, callback, base, context);
        };
        datCallback(LPDATCALLBACK_THIS_2 callback, Base *base, void *context) {
            $$ctor$ThisCB_$3(this, callback, base, context);
        };

        datCallback(LPDATCALLBACK callback, Base *base) {
            $$ctor$CB_$1(this, callback, base);
        };
        datCallback(LPDATCALLBACK_1 callback, Base *base, void *context) {
            $$ctor$CB_$2(this, callback, base, context);
        };
        datCallback(LPDATCALLBACK_2 callback, Base *base, void *context) {
            $$ctor$CB_$3(this, callback, base, context);
        };

        void Call(void *arg) {
            $Call(this, arg);
        };
    };

    class datOutput {
    protected:
        static MM2FnHook<void> $CloseLog;
        static MM2FnHook<bool> $OpenLog;
        static MM2FnHook<void> $SetOutputMask;
    public:
        static bool OpenLog(LPCSTR filename) {
            return $OpenLog(filename);
        };

        static void CloseLog(void) {
            $CloseLog();
        };

        /* TODO: Add these?
        static void SetBeforeMsgBoxFunction(void(__cdecl *lpFunc)(void));
        static void SetAfterMsgBoxFunction(void(__cdecl *lpFunc)(void));
        */

        static void SetOutputMask(UINT mask) {
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
        static MM2FnHook<void> $$ctor;
        static MM2FnHook<void> $$dtor;

        static MM2FnHook<datParser *> $AddParser;
        static MM2FnHook<datParserRecord &> $AddRecord;

        static MM2FnHook<bool> $Load_$1;
        static MM2FnHook<bool> $Load_$2;
        static MM2FnHook<bool> $Load_$3;

        static MM2FnHook<bool> $Save_$1;
        static MM2FnHook<bool> $Save_$2;
        static MM2FnHook<bool> $Save_$3;
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
        datParserRecord & AddRecord(int type, LPCSTR name, void *dataPtr, int count) {
            // last parameter is the 'callback' which is never actually used
            return $AddRecord(this, type, name, dataPtr, count, NULL);
        };

        inline datParserRecord & AddRecord(int type, LPCSTR name, void *dataPtr) {
            return AddRecord(type, name, dataPtr, 1);
        };
    public:
        datParser(const char *filename) {
            $$ctor(this, filename);
        };

        ~datParser() {
            $$dtor(this);
        };

        datParser * AddParser(LPCSTR name) {
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

        void AddValue(LPCSTR name, bool *values, int count)     { AddRecord(1, name, values, count); };
        void AddValue(LPCSTR name, char *values, int count)     { AddRecord(2, name, values, count); };
        void AddValue(LPCSTR name, short *values, int count)    { AddRecord(3, name, values, count); };
        void AddValue(LPCSTR name, int *values, int count)      { AddRecord(4, name, values, count); };
        void AddValue(LPCSTR name, float *values, int count)    { AddRecord(5, name, values, count); };
        void AddValue(LPCSTR name, Vector2 *values, int count)  { AddRecord(6, name, values, count); };
        void AddValue(LPCSTR name, Vector3 *values, int count)  { AddRecord(7, name, values, count); };
        void AddValue(LPCSTR name, Vector4 *values, int count)  { AddRecord(8, name, values, count); };

        /*
            Inline methods for adding a single value instead of an array
        */

        inline void AddValue(LPCSTR name, bool *value)          { AddValue(name, value, 1); };
        inline void AddValue(LPCSTR name, char *value)          { AddValue(name, value, 1); };
        inline void AddValue(LPCSTR name, short *value)         { AddValue(name, value, 1); };
        inline void AddValue(LPCSTR name, int *value)           { AddValue(name, value, 1); };
        inline void AddValue(LPCSTR name, float *value)         { AddValue(name, value, 1); };
        inline void AddValue(LPCSTR name, Vector2 *value)       { AddValue(name, value, 1); };
        inline void AddValue(LPCSTR name, Vector3 *value)       { AddValue(name, value, 1); };
        inline void AddValue(LPCSTR name, Vector4 *value)       { AddValue(name, value, 1); };

        bool Load(Stream *stream, LPCSTR filename) {
            return $Load_$1(this, stream, filename);
        };

        bool Load(LPCSTR directory, LPCSTR filename) {
            return $Load_$2(this, directory, filename);
        };

        bool Load(LPCSTR directory, LPCSTR filename, LPCSTR extension) {
            return $Load_$3(this, directory, filename, extension);
        };

        /*
            Binary saving is indefinitely disabled since it's bugged
        */

        bool Save(Stream *stream, LPCSTR filename) {
            return $Save_$1(this, stream, filename, false);
        };

        bool Save(LPCSTR directory, LPCSTR filename) {
            return $Save_$2(this, directory, filename, false);
        };

        bool Save(LPCSTR directory, LPCSTR filename, LPCSTR extension) {
            return $Save_$3(this, directory, filename, extension, false);
        };
    };

    class datTimeManager {
    protected:
        static MM2FnHook<void> $Reset;
        static MM2FnHook<void> $Update;
    public:
        static void Reset(void) {
            $Reset();
        };

        static void Update(void) {
            $Update();
        };
    };
}