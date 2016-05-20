#pragma once
#include "common.h"

//
// Log file functions
//

#define __LOGFMT_BUF_SIZE 1024

namespace LogFile {
    extern bool bCreateLog;

    extern bool bAppendLine;
    extern bool bIgnoreBuffer;

    void Write(LPCSTR str);
    void WriteLine(LPCSTR str);

    void Format(LPCSTR format, ...);

    void AppendLine(void);
}