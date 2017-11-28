#include "discord-presence.h"

using namespace MM2;

static const char* APPLICATION_ID = "379767166267817984";

static DiscordRichPresence presence;

void handleDiscordReady() {
	Warningf("Discord's ready...");
}

void handleDiscordError(int errorCode, const char *message) {
	Warningf("Discord error number %d: %s\n", errorCode, message);
}

void handleDiscordDisconnected(int errorCode, const char *message) {
	Warningf("Discord disconnected! Error number %d: %s\n", errorCode, message);
}

void handleDiscordJoinGame(const char *joinSecret) {
	Warningf("Joining game... invite: %s\n", joinSecret);
}

void handleDiscordSpectateGame(const char *spectateSecret) {
	Warningf("Spectating game... invite: %s\n", spectateSecret);
}

void handleDiscordJoinRequest(const DiscordJoinRequest *request) {
	Warningf("Handling Discord join request...\n");
}

void InitDiscord(void) {
    // initialize handlers
    DiscordEventHandlers handlers;
    memset(&handlers, 0, sizeof(handlers));
    handlers.ready = handleDiscordReady;
    handlers.errored = handleDiscordError;
    handlers.disconnected = handleDiscordDisconnected;
    handlers.joinGame = handleDiscordJoinGame;
    handlers.spectateGame = handleDiscordSpectateGame;
    handlers.joinRequest = handleDiscordJoinRequest;

    LogFile::WriteLine("[discord] Initializing...");
    Discord_Initialize(APPLICATION_ID, &handlers, 1, NULL);
}

char * carImageKeys[20] = {
    "vpcoop",
    "vpbug",
    "vpcab",
    "vpcaddie",
    "vpford",
    "vpmustang99",
    "vpcop",
    "vpbullet",
    "vppanoz",
    "vpbus",
    "vpddbus",
    "vpcentury",
    "vpcoop2k",
    "vpdune",
    "vpvwcup",
    "vp4x4",
    "vpauditt",
    "vpdb7",
    "vppanozgt",
    "vplafrance",
};

bool isCruiseMode(void) {
    return (gameMode == 0);
}

bool isCrashCourse(void) {
    return (gameMode == 6);
}

bool isRaceMode(void) {
    switch (gameMode) {
        case 1: // Checkpoint
        case 3: // Circut
        case 4: // Blitz
            return true;
    }

    return false;
}

const char * getGameModeName(void) {
    switch (gameMode) {
        case 0:     return "Cruise";
        case 1:     return "Checkpoint";
        case 2:     return "Cops N' Robbers";
        case 3:     return "Circuit";
        case 4:     return "Blitz";
        case 6:     return "Crash Course";
    }

    return NULL;
}

char * getRaceName(int raceId) {
    char buffer[512]{ NULL };
    
    mmCityInfo *cityInfo = CityListPtr->GetCurrentCity();

    if (cityInfo->GetRaceNames(gameMode, buffer) > 0) {
        string raceNames = string(buffer);
        int numRaces = raceNames.NumSubStrings();

        if (raceId < numRaces)
            return raceNames.SubString(raceId + 1);

        Errorf("RACENAME(%d) -- OVERFLOW!!!\n", raceId);
    }

    return "???";
}

char * getCarImageKey(mmVehInfo *vehInfo) {
    char *imageKey = NULL;
    char *baseName = vehInfo->GetBaseName();

    int vehicleId = VehicleListPtr->GetVehicleID(baseName);

    if (vehicleId < 20) {
        imageKey = carImageKeys[vehicleId];

        // FALLBACK: Check if vehicle exists in a non-default slot
        if (VehicleListPtr->GetVehicleID(imageKey) != vehicleId) {
            imageKey = NULL;

            // try finding it in the list
            for (int i = 0; i < 20; i++) {
                char *key = carImageKeys[i];

                if (strcmp(key, baseName) == 0) {
                    // update the vehicle id for clarity
                    vehicleId = VehicleListPtr->GetVehicleID(key);
                    imageKey = key;
                    break;
                }
            }
        }
    }

    return (imageKey != NULL) ? imageKey : "nocardesc";
}

char* getCityImageKey(mmCityInfo *cityInfo) {
    char* cityName = cityInfo->GetMapName();

    if ((strcmp(cityName, "london") == 0) || (strcmp(cityName, "sf") == 0))
        return cityName;

    return "nocitydesc";
}

void UpdateDiscord(mm2RichPresenceInfo &mm2Info) {
    LPCSTR state;

    LPCSTR partyID = NULL;
    int partySize = 0;
    int partyMax = 0;

    int8_t instance = 0;

    char stateBuf[256]{ NULL };
    char details[256]{ NULL };

    presence.largeImageKey = NULL;
    presence.largeImageText = NULL;
    presence.smallImageKey = NULL;
    presence.smallImageText = NULL;

    if (mm2Info.inRace) {
        if (mm2Info.inMultiplayer) {
            state = (isRaceMode()) ? "Racing online" : "In multiplayer";

            instance = 0;
            partySize = mm2Info.lobbyCurrentPlayers;
            partyMax = mm2Info.lobbyMaxPlayers;
        }
        else {
            state = (isRaceMode()) ? "In a race" : "In singleplayer";
        }

        if (isCruiseMode()) {
            sprintf(details, "Cruisin' around");
        } else if (isRaceMode() || isCrashCourse()) {
            sprintf(details, "%s: %s", getGameModeName(), mm2Info.raceName);
        } else {
            sprintf(details, "%s", getGameModeName());
        }

        presence.largeImageText = mm2Info.city;
        presence.largeImageKey = mm2Info.cityImageKey;
        presence.smallImageText = mm2Info.vehicle;
        presence.smallImageKey = mm2Info.vehicleImageKey;
    } else {
        if (mm2Info.inMultiplayer) {
            state = ("In multiplayer lobby");
            presence.largeImageKey = "mpmenu";
            presence.largeImageText = "Multiplayer lobby";

            instance = 0;
            partySize = mm2Info.lobbyCurrentPlayers;
            partyMax = mm2Info.lobbyMaxPlayers;
        }
        else {
            state = "In main menu";
            presence.largeImageKey = "menu";
            presence.largeImageText = "Main menu";
        }
    }

    presence.state = state;
    presence.details = details;

    //Multiplayer side, to implement
    presence.instance = instance;
    presence.partyId = partyID;
    presence.partyMax = partyMax;
    presence.partySize = partySize;

    LogFile::WriteLine("[discord] Updating presence...");
    Discord_UpdatePresence(&presence);
}

int discordHandler::GameInit(void) {
    LogFile::WriteLine("[discord] GameInit called.");

    get<mmGame>()->mmGame::Init();

    mmCityInfo * cityInfo = CityListPtr->GetCurrentCity();
    mmVehInfo * vehInfo = VehicleListPtr->GetVehicleInfo(vehicleName);

    g_mm2Info.inRace = true;
    g_mm2Info.city = cityInfo->GetLocalisedName();
    g_mm2Info.cityImageKey = getCityImageKey(cityInfo);
    g_mm2Info.vehicle = vehInfo->GetDescription();
    g_mm2Info.vehicleImageKey = getCarImageKey(vehInfo);
    g_mm2Info.raceName = getRaceName(raceId);
    UpdateDiscord(g_mm2Info);

    return 1;
}

void discordHandler::GameBeDone(int) {
    LogFile::WriteLine("[discord] GameBeDone called.");

    g_mm2Info.inRace = false;
    g_mm2Info.city = NULL;
    g_mm2Info.cityImageKey = NULL;
    g_mm2Info.vehicle = NULL;
    g_mm2Info.vehicleImageKey = NULL;
    g_mm2Info.raceName = NULL;
    UpdateDiscord(g_mm2Info);
}

int discordHandler::DetectHostMPLobby(char *sessionName, char *sessionPassword, int sessionMaxPlayers, NETSESSION_DESC *sessionData) {
    LogFile::WriteLine("Entered multiplayer lobby");
    g_mm2Info.inMultiplayer = true;
    g_mm2Info.lobbyCurrentPlayers = 1;
    g_mm2Info.lobbyMaxPlayers = sessionMaxPlayers;
    UpdateDiscord(g_mm2Info);

    return get<asNetwork>()->CreateSession(sessionName, sessionPassword, sessionMaxPlayers, sessionData);
}

byte data[sizeof(DPSESSIONDESC2)]{ NULL };

int discordHandler::DetectJoinMPLobby(char *a2, GUID *a3, char *a4) {
    LogFile::WriteLine("Entered multiplayer lobby");
    g_mm2Info.inMultiplayer = true;

    int result = get<asNetwork>()->JoinSession(a2, a3, a4);

    DWORD dataSize = 0;

    asNetwork *netmgr = NETMGR.ptr();
    IDirectPlay4 *dplay = netmgr->pDPlay;

    dplay->GetSessionDesc(NULL, &dataSize); //Get the data size

    dplay->GetSessionDesc(&data, &dataSize); //Populate our data buffer

    auto desc = (DPSESSIONDESC2*)data;

    g_mm2Info.lobbyMaxPlayers = desc->dwMaxPlayers;

    UpdateDiscord(g_mm2Info);

    return result;
}

void discordHandler::DetectDisconnectMPLobby(void) {
    LogFile::WriteLine("Exited multiplayer lobby");
    g_mm2Info.inMultiplayer = false;
    UpdateDiscord(g_mm2Info);

    get<asNetwork>()->Disconnect();
}

void discordHandler::DetectDisconnectMPGame(void) {
    LogFile::WriteLine("Exited multiplayer game");
    g_mm2Info.inMultiplayer = false;
    UpdateDiscord(g_mm2Info);

    get<asNetwork>()->CloseSession();
}

int discordHandler::RefreshNumPlayersLobby(void) {
    g_mm2Info.lobbyCurrentPlayers = get<asNetwork>()->GetNumPlayers();
    UpdateDiscord(g_mm2Info);
    return g_mm2Info.lobbyCurrentPlayers;
}

void discordHandler::Install() {
    InstallCallback("mmGame::Init", "Updates Discord Rich Presence when entering a race.",
        &GameInit, {
            cbHook<JMP>(0x433AA0),      //mmGameSingle::Init
            cbHook<CALL>(0x438F81),     //mmGameMulti::Init
        }
    );
    InstallCallback("mmGame::BeDone", "Updates Discord Rich Presence when exiting a race.",
        &GameBeDone, {
            cbHook<JMP>(0x414DF1),      //end of mmGame::BeDone
        }
    );
    InstallCallback("asNetwork::CreateSession", "Update the multiplayer status to on when creating the lobby.",
        &DetectHostMPLobby, {
            cbHook<CALL>(0x4117C5),     //mmInterface::CreateSession
        }
    );
    InstallCallback("asNetwork::JoinSession", "Update the multiplayer status to on when joining the lobby.",
        &DetectJoinMPLobby, {
            cbHook<CALL>(0x572782),     //asNetwork::JoinSession
        }
    );
    InstallCallback("asNetwork::Disconnect", "Update the multiplayer status to off when exiting the lobby.",
        &DetectDisconnectMPLobby, {
            cbHook<CALL>(0x40D394),     //mmInterface::Switch
        }
    );
    InstallCallback("asNetwork::CloseSession", "Update the multiplayer status to off when exiting the current game.",
        &DetectDisconnectMPLobby, {
            cbHook<CALL>(0x43B159),     //mmGameMulti::QuitNetwork
        }
    );
    InstallCallback("asNetwork::GetNumPlayers", "Updates the number of players in a lobby",
        &RefreshNumPlayersLobby, {
            cbHook<CALL>(0x4111B1),     //mmInterface::RefreshPlayers
        }
    );

    InitDiscord();
    UpdateDiscord(g_mm2Info);
}

void discordHandler::Release() {
    LogFile::WriteLine("[discord] Shutting down...");
    Discord_Shutdown();
}