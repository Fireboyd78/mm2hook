#pragma once
#include "common.h"
#include "patch.h"
#include "mm2.h"
#include "mm2_network.h"

#include "discord-rpc.h"

struct mm2RichPresenceInfo {
    bool inGame;
    bool inMultiplayer;

    int lobbyNumPlayers;
    int lobbyMaxPlayers;
	const char *lobbyId;

    const char *city;
    const char *cityImageKey;
    const char *vehicle;
    const char *vehicleImageKey;

    const char *raceName;

	void UpdatePresence(DiscordRichPresence &presence);
};

static mm2RichPresenceInfo g_mm2Info;

class discordHandler : handler_t {
public:
    int GameInit(void);
    void GameBeDone(int);
    int DetectHostMPLobby(char *, char *, int, MM2::NETSESSION_DESC *);
    int DetectJoinMPLobby(char *, GUID *, char *);
    int DetectJoinMPLobbySession(void);

    void DetectDisconnectMPLobby(void);
    void DetectDisconnectMPGame(void);
    int RefreshNumPlayersLobby(void);

	static void Install(void);
    static void Release(void);
};