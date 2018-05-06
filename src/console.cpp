#include "console.h"

/*
    ConsoleLog
*/

static HANDLE hConsole;
static CONSOLE_SCREEN_BUFFER_INFO cInfo;

static bool bShowOutput;

static char g_Buffer[4096] { NULL };

int ConsoleLog::Write(LPCSTR str, int length) {
    DWORD count = 0;

    if (bShowOutput && (hConsole != nullptr) && (length > 0))
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

void ConsoleLog::Initialize() {
    if (hConsole != NULL) {
        debug("Tried to allocate more than one console!");
    } else {
        AllocConsole();
        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

        GetConsoleScreenBufferInfo(hConsole, &cInfo);
        cInfo.dwSize.Y = 2500;
        SetConsoleScreenBufferSize(hConsole, cInfo.dwSize);

        bShowOutput = true;
    }
}

void ConsoleLog::Close() {
    if (hConsole != nullptr) {
        FreeConsole();
        hConsole = nullptr;
    }
}

void ConsoleLog::SetTitle(LPCSTR title) {
    if (hConsole != nullptr)
        SetConsoleTitle(title);
}

void ConsoleLog::SetOutputEnabled(bool enabled) {
    if (hConsole != nullptr)
        bShowOutput = enabled;
}

void ConsoleLog::AppendLine(void) {
    if (hConsole != nullptr)
        Write("\n", 1);
}

void ConsoleLog::Write(LPCSTR str) {
    if (hConsole != nullptr)
        Write(str, strlen(str));
}

void ConsoleLog::WriteLine(LPCSTR str) {
    Write(str);
    AppendLine();
}

void ConsoleLog::Format(LPCSTR format, ...) {
    va_list va;
    va_start(va, format);
    vsprintf_s(g_Buffer, format, va);
    va_end(va);

    Write(g_Buffer);
}

void ConsoleLog::Print(int level, LPCSTR str) {
    if (hConsole != nullptr) {
        Print(level, str, strlen(str));
        AppendLine();
    }
}

void ConsoleLog::Printf(int level, LPCSTR format, ...) {
    va_list va;
    va_start(va, format);
    vsprintf_s(g_Buffer, format, va);
    va_end(va);

    Print(level, g_Buffer);
}

HANDLE ConsoleLog::GetOutputHandle() {
    return (hConsole != nullptr) ? hConsole : GetStdHandle(STD_OUTPUT_HANDLE);
}