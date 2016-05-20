#include "logfile.h"

namespace LogFile {
    bool bCreateLog = true;

    bool bAppendLine = false;
    bool bIgnoreBuffer = false;

    void Write(LPCSTR str)
    {
        if (!bIgnoreBuffer)
        {
            // also output a debug message
            OutputDebugStringA(str);
        }

        FILE *logFile = fopen("mm2hook.log", ((bCreateLog) ? "w" : "a+"));

        if (logFile == NULL)
        {
            debug("Failed to open the log file!");
            return;
        }

        if (bCreateLog)
        {
            fprintf(logFile, "--<< MM2Hook log file >>--\n\n");
            bCreateLog = false;
        }

        if (!bIgnoreBuffer)
        {
            int strLen = strlen(str);

            if (strLen > 0)
                fwrite(str, 1, strLen, logFile);
        }

        if (bAppendLine)
        {
            fprintf(logFile, "\n");
            bAppendLine = false; // disable since we're done
        }

        fclose(logFile);
    }

    void WriteLine(LPCSTR str)
    {
        bAppendLine = true; // disabled after use
        Write(str);
    }

    void Format(LPCSTR format, ...)
    {
        char buf[__LOGFMT_BUF_SIZE];
        va_list va;
        va_start(va, format);
        _vsnprintf(buf, sizeof(buf), format, va);
        va_end(va);

        Write(buf);
    }

    void AppendLine(void)
    {
        bAppendLine = true;
        bIgnoreBuffer = true;
        Write(NULL);
        bIgnoreBuffer = false;
    }
}