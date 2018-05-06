#pragma once
#include "common.h"

//
// Wrapper for console functionality
//

class ConsoleLog {
protected:
    static int Write(LPCSTR str, int length);
    static int Print(int level, LPCSTR str, int length);
public:
    static void Initialize();
    static void Close();
    
    static void SetTitle(LPCSTR title);
    static void SetOutputEnabled(bool enabled);

    static void AppendLine(void);

    static void Format(LPCSTR format, ...);

    static void Write(LPCSTR str);
    static void WriteLine(LPCSTR str);

    static void Print(int level, LPCSTR str);
    static void Printf(int level, LPCSTR format, ...);

    static HANDLE GetOutputHandle(void);
};