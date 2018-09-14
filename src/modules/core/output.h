#pragma once
#include <modules\core.h>

namespace MM2
{
    // Forward declarations
    class datOutput;

    // External declarations

    // Class definitions

    class datOutput {
    public:
        static ageHook::Type<Stream *> sm_Stream;

        static ageHook::Type<bool> ShowPopupErrors;
        static ageHook::Type<bool> ShowPopupQuits;

        static ageHook::Type<bool> OutputSent;

        AGE_API static bool OpenLog(LPCSTR filename) {
            return ageHook::StaticThunk<0x4C95A0>::Call<bool>(filename);
        };

        AGE_API static bool OpenLog(LPCSTR filename, const coreFileMethods *fileMethods) {
            return ((sm_Stream = Stream::Create(filename, fileMethods)) != NULL);
        };

        AGE_API static void CloseLog(void) {
            ageHook::StaticThunk<0x4C9530>::Call<void>();
        };

        AGE_API static void CallBeforeMsgBoxFunction(void) {
            ageHook::StaticThunk<0x4C9570>::Call<void>();
        }

        AGE_API static void CallAfterMsgBoxFunction(void) {
            ageHook::StaticThunk<0x4C9580>::Call<void>();
        }

        AGE_API static void SetBeforeMsgBoxFunction(void (*lpFunc)(void)) {
            ageHook::StaticThunk<0x4C9550>::Call<void>(lpFunc);
        }

        AGE_API static void SetAfterMsgBoxFunction(void (*lpFunc)(void)) {
            ageHook::StaticThunk<0x4C9560>::Call<void>(lpFunc);
        }
        
        AGE_API static void SetOutputMask(UINT mask) {
            ageHook::StaticThunk<0x4C9590>::Call<void>(mask);
        };

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<datOutput>("datOutput")
                .addStaticFunction("SetOutputMask", &SetOutputMask)
            .endClass();
        }
    };

    // Lua initialization

}