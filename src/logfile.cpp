#include "logfile.h"

LogFileStream::LogFileStream(LPCSTR filename, bool append)
{
    debugf("LogFileStream::LogFileStream(\"%s\", %s)", filename, (append) ? "true" : "false");
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

HANDLE hConsole = NULL;
LogFileStream *g_logfile = NULL;
char g_logfile_buffer[__LOGFMT_BUF_SIZE] = { NULL };

void write_console(LPCSTR str, int length = 0) {
    if (hConsole == NULL)
        return;

    DWORD count = 0;
    
    if (length == 0)
        length = strlen(str);

    WriteConsole(hConsole, str, length, &count, NULL);
}

void LogFile::Initialize(LPCSTR filename) {
    LogFile::Initialize(filename, NULL);
};

void LogFile::Initialize(LPCSTR filename, LPCSTR title) {
    g_logfile = LogFileStream::Create(filename, title);
    CONSOLE_SCREEN_BUFFER_INFO cInfo;
    
    AllocConsole();
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    GetConsoleScreenBufferInfo(hConsole, &cInfo);
    cInfo.dwSize.Y = 2500;
    SetConsoleScreenBufferSize(hConsole, cInfo.dwSize);
};

void LogFile::Close(void) {
    g_logfile->Flush(false);
    g_logfile->Close();

    if (hConsole != NULL)
        FreeConsole();
};

void LogFile::AppendLine(void) {
    g_logfile->AppendLine();

    write_console("\n", 1);
};

void LogFile::Write(LPCSTR str) {
    g_logfile->Write(str);
    g_logfile->Flush(false);

    write_console(str);
};

void LogFile::WriteLine(LPCSTR str) {
    g_logfile->WriteLine(str);

    write_console(str);
    write_console("\n", 1);
};

void LogFile::Format(LPCSTR format, ...) {
    va_list va;
    va_start(va, format);
    vsprintf(g_logfile_buffer, format, va);
    va_end(va);

    g_logfile->Write(g_logfile_buffer);
    g_logfile->Flush(true);

    write_console(g_logfile_buffer);
};