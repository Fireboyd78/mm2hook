#include "logfile.h"
#include "console.h"

/*
    LogFileStream
*/

static char g_LogBuffer[8192] { NULL };

LogFileStream::LogFileStream(LPCSTR filename, bool append)
{
    m_filename = filename;
    this->SetAppendMode(append);
}

void LogFileStream::Close(void)
{
    if (m_file != NULL)
        fclose(m_file);
}

void LogFileStream::SetAppendMode(bool append)
{
    if ((m_file = fopen(m_filename, (append) ? "a+" : "w")) == NULL) {
        debug((append) ? "Failed to open the log file!" : "Failed to create the log file!");
    } else {
        setvbuf(m_file, g_LogBuffer, _IOFBF, sizeof(g_LogBuffer));
    }
}

LogFileStream* LogFileStream::Create(LPCSTR filename, LPCSTR title)
{
    auto log = new LogFileStream(filename);

    if (title != NULL)
        log->WriteLine(title);

    log->SetAppendMode(true);

    return log;
}

LogFileStream* LogFileStream::Create(LPCSTR filename)
{
    return LogFileStream::Create(filename, NULL);
}

LogFileStream* LogFileStream::Open(LPCSTR filename)
{
    return new LogFileStream(filename, true);
}

void LogFileStream::Flush(bool force)
{
    if (force) {
        Close();
        SetAppendMode(true);
    } else {
        fflush(m_file);
    }
}

void LogFileStream::AppendLine(void)
{
    fputs("\n", m_file);
    Flush(true);
}

void LogFileStream::Format(LPCSTR format, ...)
{
    char buffer[__LOGFMT_BUF_SIZE] { NULL };
    
    va_list va;
    va_start(va, format);
    vsprintf_s(buffer, format, va);
    va_end(va);

    Write(buffer);
}

void LogFileStream::Write(LPCSTR str)
{
    int strLen = strlen(str);

    if (strLen > 0)
        fputs(str, m_file);
}

void LogFileStream::WriteLine(LPCSTR str)
{
    Write(str);
    AppendLine();
}

/*
    LogFile
*/

LogFileStream *g_logfile = NULL;
char g_logfile_buffer[__LOGFMT_BUF_SIZE] = { NULL };

void LogFile::Initialize(LPCSTR filename) {
    LogFile::Initialize(filename, NULL);
};

void LogFile::Initialize(LPCSTR filename, LPCSTR title) {
    g_logfile = LogFileStream::Create(filename, title);
};

void LogFile::Close(void) {
    g_logfile->Flush(false);
    g_logfile->Close();
};

void LogFile::Flush(bool force) {
    g_logfile->Flush(force);
}

void LogFile::AppendLine(void) {
    g_logfile->AppendLine();
    ConsoleLog::AppendLine();
};

void LogFile::Write(LPCSTR str) {
    g_logfile->Write(str);
    g_logfile->Flush(false);

    ConsoleLog::Write(str);
};

void LogFile::WriteLine(LPCSTR str) {
    g_logfile->WriteLine(str);
    ConsoleLog::WriteLine(str);
};

void LogFile::Format(LPCSTR format, ...) {
    va_list va;
    va_start(va, format);
    vsprintf(g_logfile_buffer, format, va);
    va_end(va);

    g_logfile->Write(g_logfile_buffer);
    g_logfile->Flush(true);

    ConsoleLog::Write(g_logfile_buffer);
};

void LogFile::Print(int level, LPCSTR str) {
    static char * Prefixes[5] = {
        "",                 // print
        "",                 // debug
        "Warning: ",        // warning
        "Error: ",          // error
        "FATAL ERROR: ",    // fatal error
    };
    
    sprintf_s(g_logfile_buffer, "%s%s", Prefixes[level], str);

    g_logfile->WriteLine(g_logfile_buffer);
    g_logfile->Flush(true);

    ConsoleLog::Print(level, g_logfile_buffer);
}

void LogFile::Printf(int level, LPCSTR format, ...) {
    char buffer[__LOGFMT_BUF_SIZE] { NULL };

    va_list va;
    va_start(va, format);
    vsprintf_s(buffer, format, va);
    va_end(va);

    Print(level, buffer);
}