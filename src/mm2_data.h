#pragma once
#include "mm2.h"

namespace MM2
{
    // Forward declarations
    extern class datArgParser;
    extern class datAssetManager;
    extern class datOutput;
    extern class datTimeManager;

    class datArgParser {
    protected:
        static MM2FnHook<bool> $Get_$1;
        static MM2FnHook<bool> $Get_$2;
        static MM2FnHook<bool> $Get_$3;
        static MM2FnHook<bool> $Get_$4;
    public:
        static bool Get(LPCSTR arg) {
            return $Get_$1(arg);
        };

        static bool Get(LPCSTR arg, UINT index, int *out) {
            return $Get_$2(arg, index, out);
        };

        static bool Get(LPCSTR arg, UINT index, float *out) {
            return $Get_$3(arg, index, out);
        };

        static bool Get(LPCSTR arg, UINT index, LPCSTR *out) {
            return $Get_$4(arg, index, out);
        };
    };

    class datAssetManager {
    protected:
        static MM2FnHook<void> $FullPath_$1;
        static MM2FnHook<void> $FullPath_$2;
        static MM2FnHook<bool> $Exists_$1;
        static MM2FnHook<bool> $Exists_$2;
    public:
        /* TODO?
        static Stream * Open(char const *,char const *,bool,bool);
        static Stream * Open(char const *,char const *,char const *,bool,bool);
        static Stream * Create(char const *,char const *,bool);
        static Stream * Create(char const *,char const *,char const *,bool);
        */

        static void FullPath(char *buffer, int length, LPCSTR directory, LPCSTR filename) {
            $FullPath_$1(buffer, length, directory, filename);
        };

        static void FullPath(char *buffer, int length, LPCSTR directory, LPCSTR filename, LPCSTR extension) {
            $FullPath_$2(buffer, length, directory, filename, extension);
        };

        // these don't work for files outside of archives
        static bool Exists(LPCSTR directory, LPCSTR filename) {
            return $Exists_$1(directory, filename);
        };

        static bool Exists(LPCSTR directory, LPCSTR filename, LPCSTR extension) {
            return $Exists_$2(directory, filename, extension);
        };
    };

    class datOutput {
    protected:
        static MM2FnHook<void> $CloseLog;
        static MM2FnHook<bool> $OpenLog;
        static MM2FnHook<void> $SetOutputMask;
    public:
        static bool OpenLog(LPCSTR filename) {
            return $OpenLog(filename);
        };

        static void CloseLog(void) {
            $CloseLog();
        };

        /* TODO: Add these?
        static void SetBeforeMsgBoxFunction(void(__cdecl *lpFunc)(void));
        static void SetAfterMsgBoxFunction(void(__cdecl *lpFunc)(void));
        */

        static void SetOutputMask(UINT mask) {
            $SetOutputMask(mask);
        };
    };

    class datTimeManager {
    protected:
        static MM2FnHook<void> $Reset;
        static MM2FnHook<void> $Update;
    public:
        static void Reset(void) {
            $Reset();
        };

        static void Update(void) {
            $Update();
        };
    };
}