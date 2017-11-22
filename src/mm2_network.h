#pragma once
#include "mm2_common.h"

#define DPID DWORD

namespace MM2 {
    //Forward declarations
    class DPACCOUNTDESC;
    class DPAPPLICATIONDESC;
    class DPAPPLICATIONDESC;
    class DPCAPS;
    class DPCHAT;
    class DPCOMPORTADDRESS;
    class DPCOMPOUNDADDRESSELEMENT;
    class DPCREDENTIALS;
    class DPLAPPINFO;
    class DPLCONNECTION;
    class DPNAME;
    class DPSECURITYDESC;
    class DPSESSIONDESC2;
    class IDirectPlay4;

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

        union {                             //Pointer to a string containing the location of the executable's current directory
            LPSTR lpszCurrentDirectoryA;    //ANSI string
            LPWSTR lpszCurrentDirectory;    //Unicode string
        };

        LPSTR lpszDescriptionA;     //ANSI string containing the localized description of the application
        LPWSTR lpszDescription;     //Unicode string containing the localized description of the application
    };

    class DPAPPLICATIONDESC2 {
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

        union {                             //Pointer to a string containing the location of the executable's current directory
            LPSTR lpszCurrentDirectoryA;    //ANSI string
            LPWSTR lpszCurrentDirectory;    //Unicode string
        };

        LPSTR lpszDescriptionA;     //ANSI string containing the localized description of the application
        LPWSTR lpszDescription;     //Unicode string containing the localized description of the application

        union {                             //Pointer to a string containing the name of the laucher executable
            LPSTR lpszAppLauncherNameA;     //ANSI string
            LPWSTR lpszAppLauncherName;     //Unicode string
        };
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

    class DPCOMPORTADDRESS {
    public:
        DWORD dwComPort;            //Indicates the number of COM ports to use
        DWORD dwBaudRate;           //Indicates the baud of the COM port
        DWORD dwStopBits;           //Indicates the number of stop bits
        DWORD dwParity;             //Indicates the parity used on the COM port
        DWORD dwFlowControl;        //Indicates the method of flow used on the COM port
    };

    class DPCOMPOUNDADDRESSELEMENT {
    public:
        GUID guidDataType;          //GUID identifying the type of data contained in this structure
        DWORD dwDataSize;           //Size of the data in bytes
        LPVOID lpData;              //Pointer to a buffer containing the data
    };

    class DPCREDENTIALS {
    public:
        DWORD dwSize;               //The size of the DPCREDENTIALS structure
        DWORD dwFlags;              //Deprecated, must be 0

        union {                     //Pointer to string containing the user name
            LPWSTR lpszUsername;    //Unicode string
            LPSTR lpszUsernameA;    //ANSI string
        };

        union {                     //Pointer to string containing the user password
            LPWSTR lpszPassword;    //Unicode string
            LPSTR lpszPasswordA;    //ANSI string
        };

        union {                     //Pointer to string containing the user domain
            LPWSTR lpszDomain;      //Unicode string
            LPSTR lpszDomainA;      //ANSI string
        };
    };

    class DPLAPPINFO {
    public:
        DWORD dwSize;               //The size of the DPLAPPINFO structure
        GUID guidApplication;       //GUID of the application

        union {                     //Pointer to string containing the application name
            LPSTR lpszAppNameA;     //ANSI string
            LPWSTR lpszAppName;     //Unicode string
        };
    };

    class DPLCONNECTION {
    public:
        DWORD dwSize;                   //The size of the DPLCONNECTION structure
        DWORD dwFlags;                  //Indicates how to open a session (join or host)
        DPSESSIONDESC2 *lpSessionDesc;  //Pointer to a DPSESSIONDESC2 structure
        DPNAME *lpPlayerName;           //Pointer to a DPNAME structure holding the name of the player to be created
        GUID guidSP;                    //GUID of the service provider
        LPVOID lpAddress;               //Pointer to a DirectPlay address that contains the information that the service provider needs to connect to a session
        DWORD dwAddressSize;            //Size in bytes of the address data
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

    class DPSECURITYDESC {
    public:
        DWORD dwSize;                   //The size of the DPSECURITYDESC structure
        DWORD dwFlags;                  //Deprecated, must be 0

        union {                         //Pointer to string describing the Security Support Provider Insterface package
            LPWSTR lpszSSPIProvider;    //Unicode string
            LPSTR lpszSSPIProviderA;    //ANSI string
        };

        union {                         //Pointer to string describing the CryptoAPI package to use
            LPWSTR lpszCAPIProvider;    //Unicode string
            LPSTR lpszCAPIProviderA;    //ANSI string
        };

        DWORD dwCAPIProviderType;       //CryptoAPO service provider type
        DWORD dwEncryptionAlgorithm;    //Encryption algorithm to use
    };

    class DPSESSIONDESC2 {
    public:
        DWORD dwSize;                   //The size of the DPSESSIONDESC2 structure
        DWORD dwFlags;                  //Flags to describe the session
        GUID guidInstance;              //GUID of the session instance
        GUID guidApplication;           //GUID of the application running the session
        DWORD dwMaxPlayers;             //Maximum number of players allowed in the session
        DWORD dwCurrentPlayers;         //Number of players currently in the session

        union {                         //Pointer to string containing the name of the session
            LPWSTR lpszSessionName;     //Unicode string
            LPSTR lpszSessionNameA;     //ANSI string
        };

        union {                         //Pointer to string containing the session password
            LPWSTR lpszPassword;        //Unicode string
            LPSTR lpszPasswordA;        //ANSI string
        };

        DWORD dwReserved1;              //Must be 0
        DWORD dwReserved2;              //Must be 0
        DWORD dwUser1;                  //Application specific
        DWORD dwUser2;                  //Application specific
        DWORD dwUser3;                  //Application specific
        DWORD dwUser4;                  //Application specific
    };

    class IDirectPlay4 {
    public:
        DPACCOUNTDESC dpAccountDesc;
        DPAPPLICATIONDESC dpApplicationDesc;
        DPAPPLICATIONDESC2 dpApplicationDesc2;
        DPCAPS dpCaps;
        DPCHAT dpChat;
        DPCOMPORTADDRESS dpComportAddress;
        DPCOMPOUNDADDRESSELEMENT dpCoumpoundAddressElement;
        DPCREDENTIALS dpCredentials;
        DPLAPPINFO dplAppInfo;
        DPLCONNECTION dplConnection;
        DPNAME dpName;
        DPSECURITYDESC dpSecurityDesc;
        DPSESSIONDESC2 dpSessionDesc2;
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

        AGE_API int Initialize(int a2, int a3, int a4)                                                                  { return ageHook::Thunk<0x56FDC0>::Call<int>(this, a2, a3, a4); }
        AGE_API int InitializeLobby(int a2, int a3)                                                                     { return ageHook::Thunk<0x56FE20>::Call<int>(this, a2, a3); }
        AGE_API void WaitForLobbyConnection(int a2)                                                                     { return ageHook::Thunk<0x56FE90>::Call<void>(this, a2); }
        AGE_API int JoinLobbySession(void)                                                                              { return ageHook::Thunk<0x56FEF0>::Call<int>(this); }
        AGE_API void SendLobbyProperty(void *a2, int a3)                                                                { return ageHook::Thunk<0x5700E0>::Call<void>(this, a2, a3); }
        AGE_API void Logout(void)                                                                                       { return ageHook::Thunk<0x5701C0>::Call<void>(this); }
        AGE_API void Deallocate(void)                                                                                   { return ageHook::Thunk<0x5702C0>::Call<void>(this); }
        AGE_API int CreateInterface(void)                                                                               { return ageHook::Thunk<0x570300>::Call<int>(this); }
        AGE_API int GetNetworkCaps(void)                                                                                { return ageHook::Thunk<0x570320>::Call<int>(this); }
        AGE_API void GetProtocols(void)                                                                                 { return ageHook::Thunk<0x5703D0>::Call<void>(this); }
        AGE_API int SetProtocol(int a2, struct NETCOMMPACK *a3)                                                         { return ageHook::Thunk<0x570400>::Call<int>(this, a2, a3); }
        AGE_API int SetProtocol(int a2)                                                                                 { return ageHook::Thunk<0x570760>::Call<int>(this, a2); }
        AGE_API void Disconnect(void)                                                                                   { return ageHook::Thunk<0x570870>::Call<void>(this); }
        AGE_API char * GetEnumProtocol(int a2)                                                                          { return ageHook::Thunk<0x5708C0>::Call<char *>(this, a2); }
        AGE_API int GetEnumProtocols(void)                                                                              { return ageHook::Thunk<0x5708F0>::Call<int>(this); }
        AGE_API signed int CreatePlayer(WCHAR *a2, LPVOID lpData, DWORD dwDataSize)                                     { return ageHook::Thunk<0x570900>::Call<int>(this, a2, lpData, dwDataSize); }
        AGE_API void DestroyPlayer(void)                                                                                { return ageHook::Thunk<0x5709C0>::Call<void>(this); }
        AGE_API void BootPlayer(DPID idPlayer)                                                                          { return ageHook::Thunk<0x570A00>::Call<void>(this, idPlayer); }
        AGE_API int GetPlayers(LPGUID lpguidInstance)                                                                   { return ageHook::Thunk<0x570A30>::Call<int>(this, lpguidInstance); }
        AGE_API char * GetEnumPlayer(int a2)                                                                            { return ageHook::Thunk<0x570A60>::Call<char *>(this, a2); }
        AGE_API unsigned long GetPlayerID(int a2)                                                                       { return ageHook::Thunk<0x570A90>::Call<unsigned long>(this, a2); }
        AGE_API int GetNumPlayers(void)                                                                                 { return ageHook::Thunk<0x570AD0>::Call<int>(this); }
        AGE_API int GetPlayerName(DPID idPlayer)                                                                        { return ageHook::Thunk<0x570B90>::Call<int>(this, idPlayer); }
        AGE_API int GetPlayerName(DPID idPlayer, char *a3)                                                              { return ageHook::Thunk<0x570C60>::Call<int>(this, idPlayer, a3); }
        AGE_API void SetPlayerData(DPID idPlayer, void *lpData, DWORD dwDataSize)                                       { return ageHook::Thunk<0x570D70>::Call<void>(this, idPlayer, lpData, dwDataSize); }
        AGE_API signed int GetEnumPlayerData(int a2, void *lpData, DWORD lpdwDataSize)                                  { return ageHook::Thunk<0x570E10>::Call<int>(this, a2, lpData, lpdwDataSize); }
        AGE_API int GetPlayerdata(DPID idPlayer, void *lpData, DWORD lpdwDataSize)                                      { return ageHook::Thunk<0x570EE0>::Call<int>(this, idPlayer, lpData, lpdwDataSize); }
        AGE_API IDirectPlay4 * Ping(DPID idPlayer)                                                                      { return ageHook::Thunk<0x570F90>::Call<IDirectPlay4 *>(this, idPlayer); }
        AGE_API int CreateSession(char *a2, char *a3, int a4, struct NETSESSION_DESC *a5)                               { return ageHook::Thunk<0x571070>::Call<int>(this, a2, a3, a4, a5); }
        AGE_API int JoinSession(char *a2, struct _GUID *a3, char *a4)                                                   { return ageHook::Thunk<0x571170>::Call<int>(this, a2, a3, a4); }
        AGE_API void CloseSession(void)                                                                                 { return ageHook::Thunk<0x571550>::Call<void>(this); }
        AGE_API char * GetEnumSession(int a2)                                                                           { return ageHook::Thunk<0x571590>::Call<char *>(this); }
        AGE_API int GetEnumSessionID(int a2)                                                                            { return ageHook::Thunk<0x5715C0>::Call<int>(this, a2); }
        AGE_API int GetNumSessions(void)                                                                                { return ageHook::Thunk<0x5715F0>::Call<int>(this); }
        AGE_API void SealSession(void)                                                                                  { return ageHook::Thunk<0x571600>::Call<void>(this); }
        AGE_API void UnSealSession(void)                                                                                { return ageHook::Thunk<0x571700>::Call<void>(this); }
        AGE_API void GetSessionData(struct NETSESSION_DESC *a2)                                                         { return ageHook::Thunk<0x571800>::Call<void>(this); }
        AGE_API void SetSessionData(struct NETSESSION_DESC *a2, char *a3)                                               { return ageHook::Thunk<0x5718E0>::Call<void>(this); }
        AGE_API int GetSessionSynch(int a2)                                                                             { return ageHook::Thunk<0x571A00>::Call<int>(this, a2); }
        AGE_API void GetSessionAsynch(void)                                                                             { return ageHook::Thunk<0x571AB0>::Call<void>(this); }
        AGE_API void StopSessionsAsynch(void)                                                                           { return ageHook::Thunk<0x571B10>::Call<void>(this); }
        AGE_API int PollLobby(void)                                                                                     { return ageHook::Thunk<0x571B90>::Call<int>(this); }
        AGE_API int Update(void)                                                                                        { return ageHook::Thunk<0x571C30>::Call<int>(this); }
        AGE_API void HandleAppMessage(struct NET_TXHEAD *a2, unsigned long a3, unsigned long a4)                        { return ageHook::Thunk<0x571D50>::Call<void>(this, a2, a3, a4); }
        AGE_API void HandleSysMessage(struct DPMSG_GENERIC *a2)                                                         { return ageHook::Thunk<0x571D90>::Call<void>(this, a2); }
        AGE_API void GetMEssageQueue(unsigned long a2, unsigned long a3, unsigned int &a4, unsigned int &a5, int a6)    { return ageHook::Thunk<0x572090>::Call<void>(this, a2, a3, a4, a5, a6); }
        AGE_API void DumpRxQueueInfo(void)                                                                              { return ageHook::Thunk<0x5720E0>::Call<void>(this); }
        AGE_API void DumpTxQueueInfo(void)                                                                              { return ageHook::Thunk<0x572100>::Call<void>(this); }
        AGE_API void ChatMessage(DPID idTo, char *a3)                                                                   { return ageHook::Thunk<0x572120>::Call<void>(this, idTo, a3); }
        AGE_API int Send(DPID idTo, void *a3, const void *a4, unsigned int a5, int a6)                                  { return ageHook::Thunk<0x572200>::Call<int>(this, idTo, a3, a4, a5, a6); }
        AGE_API int GetMyLogicalIndex(void)                                                                             { return ageHook::Thunk<0x5723F0>::Call<int>(this); }
        AGE_API int GetGameVersion(int a2)                                                                              { return ageHook::Thunk<0x572430>::Call<int>(this, a2); }
        AGE_API int GetEnumSessionStatus(int a2)                                                                        { return ageHook::Thunk<0x572460>::Call<int>(this, a2); }
        AGE_API int GetEnumSessionLock(int a2)                                                                          { return ageHook::Thunk<0x572490>::Call<int>(this, a2); }
        AGE_API void SetEnumSessionLock(int a2, int a3)                                                                 { return ageHook::Thunk<0x5724C0>::Call<void>(this, a2, a3); }
        AGE_API void SetTime(float a2)                                                                                  { return ageHook::Thunk<0x5724E0>::Call<void>(this, a2); }
        AGE_API float GetTime(void)                                                                                     { return ageHook::Thunk<0x572500>::Call<float>(this); }
        AGE_API float GetTicks(void)                                                                                    { return ageHook::Thunk<0x572540>::Call<float>(this); }
        AGE_API int JoinSession(int a2, char *a3)                                                                       { return ageHook::Thunk<0x572730>::Call<int>(this, a2, a3); }
        AGE_API int GetNumModems(void)                                                                                  { return ageHook::Thunk<0x5727B0>::Call<int>(this); }
        AGE_API char * GetEnumModem(int a2)                                                                             { return ageHook::Thunk<0x5727C0>::Call<char *>(this, a2); }
        AGE_API int QueryModems(void)                                                                                   { return ageHook::Thunk<0x5727F0>::Call<int>(this); }
    };
}