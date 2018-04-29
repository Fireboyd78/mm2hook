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

int ConsoleLog::Print(int level, LPCSTR str, int length) {
    static short PrintColors[5] = {
        TEXTCOLOR_LIGHTGRAY,    // print
        TEXTCOLOR_DARKGRAY,     // debug
        TEXTCOLOR_YELLOW,       // warning
        TEXTCOLOR_LIGHTRED,     // error
        TEXTCOLOR_LIGHTRED,     // fatal error
    };

    SetConsoleTextAttribute(hConsole, PrintColors[level]);
    int result = Write(str, length);
    SetConsoleTextAttribute(hConsole, TEXTCOLOR_LIGHTGRAY);

    return result;
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
    char buffer[4096]{ NULL };

    va_list va;
    va_start(va, format);
    vsprintf_s(buffer, format, va);
    va_end(va);

    Write(buffer);
}

void ConsoleLog::Print(int level, LPCSTR str) {
    if (cLog != NULL) {
        cLog->Print(level, str, strlen(str));
        cLog->AppendLine();
    }
}

void ConsoleLog::Printf(int level, LPCSTR format, ...) {
    char buffer[4096]{ NULL };

    va_list va;
    va_start(va, format);
    vsprintf_s(buffer, format, va);
    va_end(va);

    Print(level, buffer);
}

HANDLE ConsoleLog::GetOutputHandle() {
    return (cLog != NULL) ? cLog->hConsole : GetStdHandle(STD_OUTPUT_HANDLE);
}