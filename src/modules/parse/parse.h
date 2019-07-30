#pragma once
#include <modules\parse.h>

namespace MM2
{
    // Forward declarations
    class datParser;
    class datParserRecord;

    // External declarations
    extern class Stream;

    // Class definitions

    namespace $
    {
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
    }

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

    //TODO: figure out the inheritance that the compiler 
    //seems to have completely mess up
    class datAsciiTokenizer {
    public:
    };

    // Lua initialization

}