#pragma once
#include <modules\data.h>

namespace MM2
{
    // Forward declarations
    class datArgParser;

    // External declarations


    // Class definitions

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
    }

    class datArgParser {
    public:
        static hook::Type<int> Argc;
        static hook::Type<char **> Argv;

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

    // Lua initialization

}