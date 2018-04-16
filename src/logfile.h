#pragma once
#include "common.h"

//
// Log file functions
//

#define __LOGFMT_BUF_SIZE 1024

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

    void Write(LPCSTR str);
    void WriteLine(LPCSTR str);

    template <typename ...TArgs>
    __inline void Format(LPCSTR format, TArgs ...args) {
        char buf[__LOGFMT_BUF_SIZE];
        sprintf_s(buf, sizeof(buf), format, args...);
        
        Write(buf);
    };
};

namespace LogFile {
    void Initialize(LPCSTR filename);
    void Initialize(LPCSTR filename, LPCSTR title);

    void Close(void);
    void Flush(bool force);
    
    void AppendLine(void);

    void Write(LPCSTR str);
    void WriteLine(LPCSTR str);

    void Format(LPCSTR format, ...);
};