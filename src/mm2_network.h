#pragma once
#include "mm2_common.h"

#include <dplay.h>
#include <dplobby.h>

namespace MM2 {
    //Forward declarations
    class asNetwork;

    namespace $ {
        namespace asNetwork {
            declhook(0x571070, _Func<int>::ThisCall, CreateSession);
            declhook(0x571170, _Func<int>::ThisCall, JoinSession);
            declhook(0x570870, _Func<void>::ThisCall, Disconnect);
            declhook(0x570AD0, _Func<int>::ThisCall, GetNumPlayers);
            declhook(0x571550, _Func<void>::ThisCall, CloseSession);
        }
    }
    
    typedef int DPID; // player ID

    struct NETSESSION_DESC {
        char info[256];
        DWORD version; // default: 3
        struct {
            int timeOfDay       : 4;
            int weatherType     : 4;
            int raceId          : 4;
            int gameMode        : 4;
            int extraData       : 16;
        } details;
        union {
            // raw settings data
            DWORD _settings;

            /*
                General settings
                Valid for any game mode.
            */
            struct {
                BOOL noRespawn  : 1;
                int skillLevel  : 3;
                int userData    : 12;
                int extraData   : 16;
            } settings;
            /*
                Race settings
                Valid for racing game modes.
            */
            struct {
                int             : 4; // s_settings
                int numLaps     : 4;
                int timeLimit   : 8;
                int extraData   : 16;
            } race_settings;
            /*
                Cops 'N Robbers settings
                Valid for the Cops 'n Robbers game mode only.
            */
            struct {
                int             : 4; // s_settings
                // TODO: limits?
                int cr_00       : 2;
                int cr_01       : 2;
                int goldMass    : 2;
                int teamType    : 22; // 2 +20 bits (no extra data)
            } cr_settings;
        };
        DWORD pedestrianDensity;
    };

    ASSERT_SIZEOF(NETSESSION_DESC, 0x110);
    
    class asNetwork {
    private:
        byte _buffer[0x78];
    protected:
        ageHook::Field<0x18, IDirectPlay4 *> _dplay;
        ageHook::Field<0x1C, IDirectPlayLobby3 *> _dplobby;
    public:
        datCallback SysMessageCB;
        datCallback AppMessageCB;
        IDirectPlay4A *pDPlay;
        IDirectPlayLobby3A *pLobby;
        DPID PlayerID;
        DPLCONNECTION *pConnection;
        GUID *pGUID;
        void *nRecvBufSize;
        uint32_t pRecvBuf;
        float float34;
        uint32_t MaxPlayers;
        uint32_t SessionOpen;
        uint32_t InLobby;
        uint32_t dword44;
        uint32_t dword48;
        uint8_t IsHost;
        Timer Time;
        float Elapsed;
        netZoneScore NetScore;

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

        inline IDirectPlay4 * getDirectPlay(void) {
            return _dplay.get(this);
        }

        inline IDirectPlayLobby3 * getDirectPlayLobby(void) {
            return _dplobby.get(this);
        }

        AGE_API int Initialize(int a2, int a3, int a4)      { return ageHook::Thunk<0x56FDC0>::Call<int>(this, a2, a3, a4); }
        AGE_API int InitializeLobby(int a2, int a3)         { return ageHook::Thunk<0x56FE20>::Call<int>(this, a2, a3); }
        AGE_API void WaitForLobbyConnection(int a2)         { return ageHook::Thunk<0x56FE90>::Call<void>(this, a2); }
        AGE_API int JoinLobbySession(void)                  { return ageHook::Thunk<0x56FEF0>::Call<int>(this); }
        AGE_API void SendLobbyProperty(void *a2, int a3)    { return ageHook::Thunk<0x5700E0>::Call<void>(this, a2, a3); }
        AGE_API void Logout(void)                           { return ageHook::Thunk<0x5701C0>::Call<void>(this); }
        AGE_API void Deallocate(void)                       { return ageHook::Thunk<0x5702C0>::Call<void>(this); }
        AGE_API int CreateInterface(void)                   { return ageHook::Thunk<0x570300>::Call<int>(this); }
        AGE_API int GetNetworkCaps(void)                    { return ageHook::Thunk<0x570320>::Call<int>(this); }
        AGE_API void GetProtocols(void)                     { return ageHook::Thunk<0x5703D0>::Call<void>(this); }
        AGE_API int SetProtocol(int a2, struct NETCOMMPACK *a3)
                                                            { return ageHook::Thunk<0x570400>::Call<int>(this, a2, a3); }
        AGE_API int SetProtocol(int a2)                     { return ageHook::Thunk<0x570760>::Call<int>(this, a2); }
        AGE_API void Disconnect(void)                       { return ageHook::Thunk<0x570870>::Call<void>(this); }
        AGE_API char * GetEnumProtocol(int a2)              { return ageHook::Thunk<0x5708C0>::Call<char *>(this, a2); }
        AGE_API int GetEnumProtocols(void)                  { return ageHook::Thunk<0x5708F0>::Call<int>(this); }
        AGE_API signed int CreatePlayer(WCHAR *a2, LPVOID lpData, DWORD dwDataSize)
                                                            { return ageHook::Thunk<0x570900>::Call<int>(this, a2, lpData, dwDataSize); }
        AGE_API void DestroyPlayer(void)                    { return ageHook::Thunk<0x5709C0>::Call<void>(this); }
        AGE_API void BootPlayer(DPID idPlayer)              { return ageHook::Thunk<0x570A00>::Call<void>(this, idPlayer); }
        AGE_API int GetPlayers(LPGUID lpguidInstance)       { return ageHook::Thunk<0x570A30>::Call<int>(this, lpguidInstance); }
        AGE_API char * GetEnumPlayer(int a2)                { return ageHook::Thunk<0x570A60>::Call<char *>(this, a2); }
        AGE_API ulong GetPlayerID(int a2)                   { return ageHook::Thunk<0x570A90>::Call<ulong>(this, a2); }
        AGE_API int GetNumPlayers(void)                     { return ageHook::Thunk<0x570AD0>::Call<int>(this); }
        AGE_API int GetPlayerName(DPID idPlayer)            { return ageHook::Thunk<0x570B90>::Call<int>(this, idPlayer); }
        AGE_API int GetPlayerName(DPID idPlayer, char *a3)  { return ageHook::Thunk<0x570C60>::Call<int>(this, idPlayer, a3); }
        AGE_API void SetPlayerData(DPID idPlayer, void *lpData, DWORD dwDataSize)
                                                            { return ageHook::Thunk<0x570D70>::Call<void>(this, idPlayer, lpData, dwDataSize); }
        AGE_API signed int GetEnumPlayerData(int a2, void *lpData, DWORD lpdwDataSize)
                                                            { return ageHook::Thunk<0x570E10>::Call<int>(this, a2, lpData, lpdwDataSize); }
        AGE_API int GetPlayerData(DPID idPlayer, void *lpData, DWORD lpdwDataSize)
                                                            { return ageHook::Thunk<0x570EE0>::Call<int>(this, idPlayer, lpData, lpdwDataSize); }
        AGE_API IDirectPlay4 * Ping(DPID idPlayer)          { return ageHook::Thunk<0x570F90>::Call<IDirectPlay4 *>(this, idPlayer); }
        AGE_API int CreateSession(char *name, char *password, int maxPlayers, NETSESSION_DESC *sessionData)
                                                            { return ageHook::Thunk<0x571070>::Call<int>(this, name, password, maxPlayers, sessionData); }
        AGE_API int JoinSession(char *a2, GUID *a3, char *a4)
                                                            { return ageHook::Thunk<0x571170>::Call<int>(this, a2, a3, a4); }
        AGE_API void CloseSession(void)                     { return ageHook::Thunk<0x571550>::Call<void>(this); }
        AGE_API char * GetEnumSession(int a2)               { return ageHook::Thunk<0x571590>::Call<char *>(this); }
        AGE_API int GetEnumSessionID(int a2)                { return ageHook::Thunk<0x5715C0>::Call<int>(this, a2); }
        AGE_API int GetNumSessions(void)                    { return ageHook::Thunk<0x5715F0>::Call<int>(this); }
        AGE_API void SealSession(void)                      { return ageHook::Thunk<0x571600>::Call<void>(this); }
        AGE_API void UnSealSession(void)                    { return ageHook::Thunk<0x571700>::Call<void>(this); }
        AGE_API void GetSessionData(NETSESSION_DESC *sessionData)
                                                            { return ageHook::Thunk<0x571800>::Call<void>(this, sessionData); }
        AGE_API void SetSessionData(NETSESSION_DESC *sessionData, char *a3)
                                                            { return ageHook::Thunk<0x5718E0>::Call<void>(this, sessionData, a3); }
        AGE_API int GetSessionSynch(int a2)                 { return ageHook::Thunk<0x571A00>::Call<int>(this, a2); }
        AGE_API void GetSessionAsynch(void)                 { return ageHook::Thunk<0x571AB0>::Call<void>(this); }
        AGE_API void StopSessionsAsynch(void)               { return ageHook::Thunk<0x571B10>::Call<void>(this); }
        AGE_API int PollLobby(void)                         { return ageHook::Thunk<0x571B90>::Call<int>(this); }
        AGE_API int Update(void)                            { return ageHook::Thunk<0x571C30>::Call<int>(this); }
        AGE_API void HandleAppMessage(struct NET_TXHEAD *a2, ulong a3, ulong a4)
                                                            { return ageHook::Thunk<0x571D50>::Call<void>(this, a2, a3, a4); }
        AGE_API void HandleSysMessage(DPMSG_GENERIC *a2)    { return ageHook::Thunk<0x571D90>::Call<void>(this, a2); }
        AGE_API void GetMEssageQueue(ulong a2, ulong a3, uint &a4, uint &a5, int a6)
                                                            { return ageHook::Thunk<0x572090>::Call<void>(this, a2, a3, a4, a5, a6); }
        AGE_API void DumpRxQueueInfo(void)                  { return ageHook::Thunk<0x5720E0>::Call<void>(this); }
        AGE_API void DumpTxQueueInfo(void)                  { return ageHook::Thunk<0x572100>::Call<void>(this); }
        AGE_API void ChatMessage(DPID idTo, char *a3)       { return ageHook::Thunk<0x572120>::Call<void>(this, idTo, a3); }
        AGE_API int Send(DPID idTo, void *a3, const void *a4, uint a5, int a6)
                                                            { return ageHook::Thunk<0x572200>::Call<int>(this, idTo, a3, a4, a5, a6); }
        AGE_API int GetMyLogicalIndex(void)                 { return ageHook::Thunk<0x5723F0>::Call<int>(this); }
        AGE_API int GetGameVersion(int a2)                  { return ageHook::Thunk<0x572430>::Call<int>(this, a2); }
        AGE_API int GetEnumSessionStatus(int a2)            { return ageHook::Thunk<0x572460>::Call<int>(this, a2); }
        AGE_API int GetEnumSessionLock(int a2)              { return ageHook::Thunk<0x572490>::Call<int>(this, a2); }
        AGE_API void SetEnumSessionLock(int a2, int a3)     { return ageHook::Thunk<0x5724C0>::Call<void>(this, a2, a3); }
        AGE_API void SetTime(float a2)                      { return ageHook::Thunk<0x5724E0>::Call<void>(this, a2); }
        AGE_API float GetTime(void)                         { return ageHook::Thunk<0x572500>::Call<float>(this); }
        AGE_API float GetTicks(void)                        { return ageHook::Thunk<0x572540>::Call<float>(this); }
        AGE_API int JoinSession(int a2, char *a3)           { return ageHook::Thunk<0x572730>::Call<int>(this, a2, a3); }
        AGE_API int GetNumModems(void)                      { return ageHook::Thunk<0x5727B0>::Call<int>(this); }
        AGE_API char * GetEnumModem(int a2)                 { return ageHook::Thunk<0x5727C0>::Call<char *>(this, a2); }
        AGE_API int QueryModems(void)                       { return ageHook::Thunk<0x5727F0>::Call<int>(this); }
    };

    declhook(0x6B3968, _Type<asNetwork>, NETMGR);

    template<>
    void luaAddModule<module_network>(LuaState L) {

    }
}