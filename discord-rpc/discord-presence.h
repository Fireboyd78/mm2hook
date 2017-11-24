#pragma once
#include "common.h"
#include "patch.h"
#include "mm2.h"
#include "mm2_network.h"

#include "discord-rpc.h"

struct mm2RichPresenceInfo {
    bool inRace;
    bool inMultiplayer;

    int lobbyCurrentPlayers;
    int lobbyMaxPlayers;

    const char *city;
    const char *cityImageKey;
    const char *vehicle;
    const char *vehicleImageKey;

    const char *raceName;
};

static mm2RichPresenceInfo g_mm2Info;

class discordHandler {
public:
    int GameInit(void);
    void GameBeDone(int);
    int DetectHostMPLobby(char *, char *, int, MM2::NETSESSION_DESC *);
    int DetectJoinMPLobby(char *, _GUID *, char *);
    void DetectDisconnectMPLobby(void);
    void DetectDisconnectMPGame(void);
    int RefreshNumPlayersLobby(void);

	static void Install(void);
    static void Release(void);
};