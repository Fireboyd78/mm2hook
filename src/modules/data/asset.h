#pragma once
#include <modules\data.h>

namespace MM2
{
    // Forward declarations
    class datAssetManager;

    // External declarations
    extern class Stream;

    // Class definitions

    namespace $
    {
        namespace datAssetManager
        {
            declhook(0x4C55E0, _Func<void>, FullPath$1);
            declhook(0x4C56F0, _Func<void>, FullPath$2);
            declhook(0x4C59B0, _Func<bool>, Exists$1);
            declhook(0x4C59E0, _Func<bool>, Exists$2);
        }
    }

    class datAssetManager {
    public:
        /* TODO?
        static Stream * Open(char const *,char const *,bool,bool);
        static Stream * Open(char const *,char const *,char const *,bool,bool);
        static Stream * Create(char const *,char const *,bool);
        static Stream * Create(char const *,char const *,char const *,bool);
        */
        static ageHook::Type<char *> sm_Path;

        AGE_API static void FullPath(char *buffer, int length, LPCSTR directory, LPCSTR filename) {
            $::datAssetManager::FullPath$1(buffer, length, directory, filename);
        };

        AGE_API static void FullPath(char *buffer, int length, LPCSTR directory, LPCSTR filename, LPCSTR extension) {
            $::datAssetManager::FullPath$2(buffer, length, directory, filename, extension);
        };

        // these don't work for files outside of archives
        AGE_API static bool Exists(LPCSTR directory, LPCSTR filename) {
            return $::datAssetManager::Exists$1(directory, filename);
        };

        AGE_API static bool Exists(LPCSTR directory, LPCSTR filename, LPCSTR extension) {
            return $::datAssetManager::Exists$2(directory, filename, extension);
        };
    };

    // Lua initialization

}