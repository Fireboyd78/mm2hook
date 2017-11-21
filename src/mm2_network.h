#pragma once
#include "mm2_common.h"

#define DPID DWORD

namespace MM2 {
    //Forward declarations
    class DPACCOUNTDESC;
    class DPAPPLICATIONDESC;
    class DPCAPS;
    class DPCHAT;
    class IDirectPlay4;
    class DPNAME;

    class asNetwork;

    class DPACCOUNTDESC {
    public:
        DWORD dwSize;               //The size of the DPACCOUNTDESC structure
        DWORD dwFlags;              //Deprecated, must be 0
        
        union {                     //Pointer to a string containing the account identifier
            LPWSTR lpszAccountID;   //Unicode string
            LPSTR lpszAccountIDA;   //ANSI string
        };
    };

    class DPAPPLICATIONDESC {
    public:
        DWORD dwSize;               //The size of the DPAPPLICATIONDESC structure
        DWORD dwFlags;              //Deprecated, must be 0

        union {                         //Pointer to a string containing the name of the application
            LPSTR lpszApplicationNameA; //ANSI String
            LPWSTR lpszApplicationName; //Unicode string
        };

        GUID guidApplication;       //GUID of the application, duh

        union {                     //Pointer to a string containing the name of the executable
            LPSTR lpszFilenameA;    //ANSI string
            LPWSTR lpszFilename;    //Unicode string
        };

        union {                     //Pointer to a string containing the path of the executable
            LPSTR lpszPathA;        //ANSI string
            LPWSTR lpszPath;        //Unicode string
        };

        union {                             //Point to a string containing the  location of the executable's current directory
            LPSTR lpszCurrentDirectoryA;    //ANSI string
            LPWSTR lpszCurrentDirectory;    //Unicode string
        };

        LPSTR lpszDescriptionA;     //ANSI string containing the localized description of the application
        LPWSTR lpszDescription;     //Unicode string containing the localized description of the application
    };

    class DPCAPS {
    public:
        DWORD dwSize;               //The size of the DCAPS structure
        DWORD dwFlags;              //Flags that indicate the properties of the DirectPlay object
        DWORD dwMaxBufferSize;      //Maximum number of bytes that can be sent by this service provider
        DWORD dwMaxQueueSize;       //Deprecated it seems
        DWORD dwMaxPlayers;         //Maximum number of players supported
        DWORD dwHundredBaud;        //Bandwith specified in multiples of 100 bits per second
        DWORD dwLatency;            //Estimate of latency by the service provider in milliseconds
        DWORD dwMaxLocalPlayers;    //Maximum number of local players supported in a session
        DWORD dwHeaderLength;       //Size in bytes of the header that will be added to player messages
        DWORD dwTimeout;            //Service provider's suggested time-out value
    };

    class DPCHAT {
    public:
        DWORD dwSize;               //The size of the DPCHAT structure
        DWORD dwFlags;              //Deprecated, must be 0

        union {                     //Pointer to string containing the message to be sent
            LPWSTR lpszMessage;     //Unicode string
            LPSTR lpszMessageA;     //ANSI string
        };
    };

    class DPNAME {
    public:
        DWORD dwSize;               //The size of the DPNAME structure
        DWORD dwFlags;              //Structure specific flags

        union {                     //Pointer to a string containing the short (friendly) name of a player
            LPWSTR lpszShortName;   //Unicode string
            LPSTR lpszShortNameA;   //ANSI string
        };

        union {                     //Pointer to a string containing the long (formal) name of a player
            LPWSTR lpszLongName;    //Unicode string
            LPSTR lpszLongNameA;    //ANSI string
        };
    };

    class IDirectPlay4 {
    public:
        DPACCOUNTDESC dpAccountDesc;
        DPAPPLICATIONDESC dpApplicationDesc;
        //DPAPPLICATIONDESC2 dpApplicationDesc2;
        DPCAPS dpCaps;
        DPCHAT dpChat;
        //DPCOMPORTADDRESS dpComportAddress;
        //DPCOMPOUNDADDRESSELEMENT dpCoumpoundAddressElement;
        //DPCREDENTIALS dpCredentials;
        //DPLAPPINFO dplAppInfo;
        //DPLCONNECTION dplConnection;
        DPNAME dpName;
        //DPSECURITYDESC dpSecurityDesc;
        //DPSESSIONDESC2 dpSessionDesc2;
    };

    class asNetwork {
    public:
        AGE_API asNetwork(void) {
            PUSH_VTABLE();
            ageHook::Thunk<0x56FCD0>::Call<void>(this);
            POP_VTABLE();
        }

        virtual AGE_API ~asNetwork(void) {
            PUSH_VTABLE();
            ageHook::Thunk<0x56FD70>::Call<void>(this);
            POP_VTABLE();
        }

        AGE_API int Initialize(int a2, int a3, int a4)                                  { return ageHook::Thunk<0x56FDC0>::Call<int>(this, a2, a3, a4); }

        AGE_API int InitializeLobby(int a2, int a3)                                     { return ageHook::Thunk<0x56FE20>::Call<int>(this, a2, a3); }

        AGE_API void WaitForLobbyConnection(int a2)                                     { return ageHook::Thunk<0x56FE90>::Call<void>(this, a2); }

        AGE_API int JoinLobbySession(void)                                              { return ageHook::Thunk<0x56FEF0>::Call<int>(this); }

        AGE_API void SendLobbyProperty(const void *a2, unsigned int a3)                 { return ageHook::Thunk<0x5700E0>::Call<void>(this, a2, a3); }

        AGE_API int Logout(void)                                                        { return ageHook::Thunk<0x5701C0>::Call<int>(this); }

        AGE_API void Deallocate(void)                                                   { return ageHook::Thunk<0x5702C0>::Call<void>(this); }

        AGE_API int CreatePlayer(WCHAR *a2, LPVOID lpData, DWORD dwDataSize)            { return ageHook::Thunk<0x570900>::Call<int>(this, a2, lpData, dwDataSize); }

        AGE_API int GetPlayer(LPGUID lpguidInstance)                                    { return ageHook::Thunk<0x570A30>::Call<int>(this, lpguidInstance); }
    
        AGE_API IDirectPlay4 Ping(DPID idPlayer)                                        { return ageHook::Thunk<0x570F90>::Call<IDirectPlay4>(this, idPlayer); }

        AGE_API int GetENumPlayerData(int a2, LPVOID lpData, DWORD lpdwDataSize)        { return ageHook::Thunk<0x570E10>::Call<int>(this, a2, lpData, lpdwDataSize); }

        AGE_API HRESULT CreateInterface(void)                                           { return ageHook::Thunk<0x570300>::Call<HRESULT>(this); }

        AGE_API int GetPlayerName(DPID idPlayer)                                        { return ageHook::Thunk<0x570B90>::Call<int>(this, idPlayer); }

        AGE_API void SetPlayerData(DPID idPlayer, LPVOID lpData, DWORD dwDataSize)      { return ageHook::Thunk<0x570D70>::Call<void>(this, idPlayer, lpData, dwDataSize); }

        AGE_API int Send(DPID idTo, void *a3, const void *a4, unsigned int a5, int a6)  { return ageHook::Thunk<0x572200>::Call<int>(this, idTo, a3, a4, a5, a6); }
    };
}