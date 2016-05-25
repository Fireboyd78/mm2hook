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

void LogFileStream::AppendLine(void)
{
    fprintf(m_file, "\n");
    fflush(m_file);
}

void LogFileStream::Write(LPCSTR str)
{
#ifndef LOGFILE_NO_DEBUG
    // also output a debug message
    OutputDebugStringA(str);
#endif

    int strLen = strlen(str);

    if (strLen > 0)
    {
        fwrite(str, 1, strLen, m_file);
        fflush(m_file);
    }
}

void LogFileStream::WriteLine(LPCSTR str)
{
    Write(str);
    AppendLine();
}

LogFileStream *g_logfile = NULL;
char g_logfile_buffer[__LOGFMT_BUF_SIZE] = { NULL };

void LogFile::Initialize(LPCSTR filename) {
    g_logfile = LogFileStream::Create(filename);
};

void LogFile::Initialize(LPCSTR filename, LPCSTR title) {
    g_logfile = LogFileStream::Create(filename, title);
};

void LogFile::Close(void) {
    g_logfile->Close();
};

void LogFile::AppendLine(void) {
    g_logfile->AppendLine();
};

void LogFile::Write(LPCSTR str) {
    g_logfile->Write(str);
};

void LogFile::WriteLine(LPCSTR str) {
    g_logfile->WriteLine(str);
};

void LogFile::Format(LPCSTR format, ...) {
    va_list va;
    va_start(va, format);
    vsprintf(g_logfile_buffer, format, va);
    va_end(va);

    g_logfile->Write(g_logfile_buffer);
};