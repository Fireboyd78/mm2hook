#pragma once
#include <modules\data.h>

namespace MM2
{
    // Forward declarations
    class datStack;

    // External declarations


    // Class definitions

    class datStack {
    private:
        static void TracebackLua(int length)
        {
            Traceback(length, nullptr);
        }

        static std::string LookupAddressLua(int address) {
            char buf[128];
            LookupAddress(buf, address);
            return std::string(buf);
        }
    public:
       
        AGE_API static char const* GetTimestamp() {
            return ageHook::StaticThunk<0x4C7120>::Call<char const*>();
        };

        AGE_API static void LookupAddress(char* buf, int address) {
            ageHook::StaticThunk<0x4C7470>::Call<void>(buf, address);
        };
        
        AGE_API static void DoTraceback(int length, DWORD* contextRecordEbpPtr, FILE* output, char const* lineSeperator) {
            char addressBuffer[128];
            DWORD* currentEbpPtr = contextRecordEbpPtr;
            for (int i = 0; i < length; i++)
            {
                auto v6 = currentEbpPtr[1];
                currentEbpPtr = (DWORD*)*currentEbpPtr;
                if (v6 < 0 || currentEbpPtr == nullptr)
                    break;
                datStack::LookupAddress(addressBuffer, v6);
                if (output)
                    fprintf(output, "%s,%s", addressBuffer, lineSeperator);
                else
                    Displayf("%s", addressBuffer);
            }
        };

        AGE_API static void Traceback(int length, FILE* output) {
            ageHook::StaticThunk<0x4C75C0>::Call<void>(length, output);
        };

        //custom extension for exception filter
        static int ExceptionFilter(EXCEPTION_POINTERS* eptrs) {
            char addressBuf[128];
            auto ctx = eptrs->ContextRecord;
            datStack::LookupAddress(addressBuf, ctx->Eip);
            Displayf(
                "EAX=%08x EBX=%08x ECX=%08x EDX=%08x\nESI=%08x EDI=%08x EBP=%08x ESP=%08x",
                ctx->Eax,
                ctx->Ebx,
                ctx->Ecx,
                ctx->Edx,
                ctx->Esi,
                ctx->Edi,
                ctx->Ebp,
                ctx->Esp);
            Displayf("Exception %x at EIP=%s", eptrs->ExceptionRecord->ExceptionCode, addressBuf);
            datStack::DoTraceback(16, &ctx->Ebp, nullptr, "\n");
            return 1;
        }

        static int ExceptionFilterWithMsgBox(EXCEPTION_POINTERS* eptrs) {
            auto ctx = eptrs->ContextRecord;

            //create our buffers (yuck!)
            char addressBuf[128], stackBuf[256], tracebackBuf[8192];
            char totalBuf[128 + 256 + 256 + 8192]; //address+stack+exception(below)+trace

            //create a temporary file for the traceback (yuck again, Windows, y u no have fmemopen)
            FILE* tempFile = tmpfile();
            if (tempFile == NULL) {
                Displayf("Couldn't create temp file for stack trace.");
                return 1;
            }
                
            //fill address and stack buf
            datStack::LookupAddress(addressBuf, ctx->Eip);
            sprintf(stackBuf, "EAX=%08x EBX=%08x ECX=%08x EDX=%08x\nESI=%08x EDI=%08x EBP=%08x ESP=%08x",
                              ctx->Eax,
                              ctx->Ebx,
                              ctx->Ecx,
                              ctx->Edx,
                              ctx->Esi,
                              ctx->Edi,
                              ctx->Ebp,
                              ctx->Esp);

            //do our traceback into tempFile
            datStack::DoTraceback(16, &ctx->Ebp, tempFile, "\n");

            //read the file to the traceback buf and close it
            long tempFileLen = ftell(tempFile);
            rewind(tempFile);           
            int readLength = fread(tracebackBuf, 1, tempFileLen, tempFile);
            tracebackBuf[readLength] = 0x00;
            fclose(tempFile);

            //finally combine everything and show the error
            sprintf(totalBuf, "%s\nException %x at EIP=%s\n%s", stackBuf, eptrs->ExceptionRecord->ExceptionCode, addressBuf, tracebackBuf);
            MessageBoxA(0, totalBuf, "Guru Meditation", MB_OK | MB_ICONSTOP);
            return 1;
        }

        static int ExceptionFilterCombined(EXCEPTION_POINTERS* eptrs) {
            ExceptionFilter(eptrs);
            ExceptionFilterWithMsgBox(eptrs);
            return 1;
        }

        //lua
        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<datStack>("datStack")
                .addStaticProperty("Timestamp", [] {return GetTimestamp(); })
                .addStaticFunction("Traceback", &TracebackLua)
                .addStaticFunction("LookupAddress", &LookupAddressLua)
                .endClass();
        }
    };

    // Lua initialization

}