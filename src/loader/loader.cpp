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
    using TClass = T;

    template<typename TT = void, typename ...TArgs>
    struct MemberData
    {
        TT (TClass::*pFunction)(TArgs...);
        LPCSTR name;

        MemberData(decltype(pFunction) pFunc, LPCSTR name) : pFunction(pFunc), name(name) {};
    };

    LPCSTR name;

    ClassRegistration(LPCSTR name) : name(name) {};

    template<typename TT = void, typename ...TArgs>
    ClassRegistration<T>& AddFunction(TT (TClass::*pFunc)(TArgs...), LPCSTR name) {
        auto member = new MemberData<TT, TArgs...>(pFunc, name);
        
        /*
            TODO: Register the function somehow?
        */

        // number of arguments
        int x = sizeof...(TArgs);
        
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