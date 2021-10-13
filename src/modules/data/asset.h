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
        AGE_API static Stream * Open(LPCSTR filename, LPCSTR extension, bool a3, bool a4) {
            return hook::StaticThunk<0x4C5870>::Call<Stream*>(filename, extension, a3, a4);
        };

        AGE_API static Stream * Open(LPCSTR directory, LPCSTR filename, LPCSTR extension, bool a4, bool a5) {
            return hook::StaticThunk<0x4C58C0>::Call<Stream*>(directory, filename, extension, a4, a5);
        };

        AGE_API static Stream * Create(LPCSTR filename, LPCSTR extension, bool a3) {
            return hook::StaticThunk<0x4C5910>::Call<Stream*>(filename, extension, a3);
        };

        AGE_API static Stream * Create(LPCSTR directory, LPCSTR filename, LPCSTR extension, bool a4) {
            return hook::StaticThunk<0x4C5960>::Call<Stream*>(directory, filename, extension, a4);
        };

        static hook::Type<char *> sm_Path;

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