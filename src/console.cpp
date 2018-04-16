#include "console.h"

/*
    ConsoleLog
*/

ConsoleLog *cLog;

int ConsoleLog::Write(LPCSTR str, int length) {
    DWORD count = 0;

    if (outputEnabled && (hConsole != NULL) && (length > 0))
        WriteConsole(hConsole, str, length, &count, NULL);

    return count;
}

ConsoleLog::ConsoleLog() {
    AllocConsole();
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    GetConsoleScreenBufferInfo(hConsole, &cInfo);
    cInfo.dwSize.Y = 2500;
    SetConsoleScreenBufferSize(hConsole, cInfo.dwSize);

    outputEnabled = true;
}

ConsoleLog::~ConsoleLog() {
    if (hConsole != NULL) {
        FreeConsole();
        hConsole = NULL;
    }
}

void ConsoleLog::Initialize() {
    if (cLog != NULL) {
        debug("Tried to allocate more than one console!");
    } else {
        cLog = new ConsoleLog();
    }
}

void ConsoleLog::Close() {
    if (cLog != NULL) {
        cLog->~ConsoleLog();
        cLog = NULL;
    }
}

void ConsoleLog::SetTitle(LPCSTR title) {
    if (cLog != NULL)
        SetConsoleTitle(title);
}

void ConsoleLog::SetOutputEnabled(bool enabled) {
    if (cLog != NULL)
        cLog->outputEnabled = enabled;
}

void ConsoleLog::AppendLine(void) {
    if (cLog != NULL)
        cLog->Write("\n", 1);
}

void ConsoleLog::Write(LPCSTR str) {
    if (cLog != NULL)
        cLog->Write(str, strlen(str));
}

void ConsoleLog::WriteLine(LPCSTR str) {
    Write(str);
    AppendLine();
}

void ConsoleLog::Format(LPCSTR format, ...) {
    char buffer[1024]{ NULL };

    va_list va;
    va_start(va, format);
    vsprintf(buffer, format, va);
    va_end(va);

    Write(buffer);
}

HANDLE ConsoleLog::GetOutputHandle() {
    return (cLog != NULL) ? cLog->hConsole : GetStdHandle(STD_OUTPUT_HANDLE);
}