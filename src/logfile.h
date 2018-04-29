#pragma once
#include "common.h"

//
// Log file functions
//

#define __LOGFMT_BUF_SIZE 4096

class LogFileStream {
protected:
    LPCSTR m_filename;
    FILE *m_file;

    LogFileStream(LPCSTR filename, bool append = false);

    void SetAppendMode(bool append);
public:
    static LogFileStream* Create(LPCSTR filename, LPCSTR title);
    static LogFileStream* Create(LPCSTR filename);

    static LogFileStream* Open(LPCSTR filename);

    void Close(void);
    void Flush(bool force);

    void AppendLine(void);

    void Format(LPCSTR format, ...);

    void Write(LPCSTR str);
    void WriteLine(LPCSTR str);
};

namespace LogFile {
    void Initialize(LPCSTR filename);
    void Initialize(LPCSTR filename, LPCSTR title);

    void Close(void);
    void Flush(bool force);
    
    void AppendLine(void);

    void Write(LPCSTR str);
    void WriteLine(LPCSTR str);

    void Print(int level, LPCSTR str);
    void Printf(int level, LPCSTR format, ...);

    void Format(LPCSTR format, ...);
};