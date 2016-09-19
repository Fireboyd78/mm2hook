#include "loader.h"

LPFNDIRECTINPUTCREATE lpDICreate;

// Export as 'DirectInputCreateA/W' so we can hook into MM2
// (Apparently DirectInputCreateW gets called sometimes...)
#pragma comment(linker, "/EXPORT:DirectInputCreateA=_DirectInputCreateImpl")
#pragma comment(linker, "/EXPORT:DirectInputCreateW=_DirectInputCreateImpl")
HRESULT NAKED DirectInputCreateImpl(HINSTANCE hinst, DWORD dwVersion, LPVOID *ppDI, LPUNKNOWN punkOuter)
{
    _asm jmp dword ptr ds:lpDICreate
}

template<class T>
struct ClassRegistration
{
    template<class T>
    struct MemberData
    {
        int (T::*pFunction)();
        LPCSTR name;

        template<class TT>
        MemberData(TT(T::*pFunc), LPCSTR name) : pFunction((int(T::*)())pFunc), name(name) {};
    };

    LPCSTR name;

    int numMembers = 0;

    MemberData<T>** pFunctions = { NULL };

    ClassRegistration(LPCSTR name) {
        pFunctions = new MemberData<T>*[64];
    };

    template<class TT>
    ClassRegistration<T>& AddFunction(TT(T::*pFunc), LPCSTR name) {
        pFunctions[numMembers++] = new MemberData<T>(pFunc, name);
        return *this;
    };
};

struct MyTestClass {
public:
    void MyTestThing(int p1, bool p2) {
        if (p1 == 1 || p2 == false) {
            LogFile::WriteLine("Woah, man");
        }
    };
};

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
        {
            debug("Attempting to hook into MM2...");

            // Initialize the log file
            LogFile::Initialize("mm2loader.log", "--<< MM2Hook log file >>--\n");
            LogFile::WriteLine("Loader initialized.");

            HMODULE hDIModule = NULL;

            if (LoadSystemLibrary("dinput.dll", &hDIModule) &&
                GetHookProcAddress(hDIModule, "DirectInputCreateA", (FARPROC*)&lpDICreate))
            {
                LogFile::WriteLine("Loader has successfully injected into DirectInputCreateA.");

                // TESTING
                ClassRegistration<MyTestClass>("MyTestClass")
                    .AddFunction(&MyTestClass::MyTestThing, "MyTestThing");

                /*
                    TODO: Parse DLLs and look for compatible MM2Hook plugins.
                */
            }
            else
            {
                LogFile::WriteLine("Loader injection FAILED.");
            }

            LogFile::Close();
        } break;
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}