#include "discord-presence.h"

using namespace MM2;

static const char* APPLICATION_ID = "379767166267817984";

static DiscordRichPresence presence;

static HMODULE g_DiscordModule = nullptr;

static decltype(Discord_Initialize) * _Discord_Initialize = nullptr;
static decltype(Discord_Shutdown) * _Discord_Shutdown = nullptr;
static decltype(Discord_UpdatePresence) * _Discord_UpdatePresence = nullptr;
static decltype(Discord_RunCallbacks) * _Discord_RunCallbacks = nullptr;

struct _discord_function {
    void *proc;
    const char *name;
} g_DiscordFunctions[] = {
    { &_Discord_Initialize, "Discord_Initialize" },
    { &_Discord_Shutdown, "Discord_Shutdown" },
    { &_Discord_UpdatePresence, "Discord_UpdatePresence" },
    { &_Discord_RunCallbacks, "Discord_RunCallbacks" },
    { NULL },
};

static bool LoadDiscordFunctions() {
    for (_discord_function *func = g_DiscordFunctions; func->name != nullptr; func++) {
        auto proc = GetProcAddress(g_DiscordModule, func->name);

        if (proc == nullptr) {
            Errorf("[discord] Failed to load '%s'!", func->name);
            return false;
        }

        *reinterpret_cast<void **>(func->proc) = proc;
    }
    return true;
}

static bool LoadDiscordModule() {
    g_DiscordModule = LoadLibraryA("discord-rpc.dll");

    if (g_DiscordModule != nullptr)
    {
        // try loading the required functions
        if (LoadDiscordFunctions())
            return true;

        // one or more required functions not found,
        // so free the library since we won't need it
        FreeLibrary(g_DiscordModule);
        g_DiscordModule = nullptr;
    }

    // failed to load
    return false;
}

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
    _Discord_Initialize(APPLICATION_ID, &handlers, 1, NULL);
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

bool isRaceMode() {
    switch (MMSTATE->GameMode) {
        case dgGameMode::Checkpoint:
        case dgGameMode::Circuit:
        case dgGameMode::Blitz:
            return true;
    }

    return false;
}

static const char * gameModeNames[] = {
    "Cruise",
    "Checkpoint",
    "Cops N' Robbers",
    "Circuit",
    "Blitz",
    "---",
    "Crash Course",
};

char * getRaceName(int raceId) {
    char buffer[512]{ NULL };
    
    mmCityInfo *cityInfo = CityListPtr->GetCurrentCity();

    if (cityInfo->GetRaceNames(MMSTATE->GameMode, buffer) > 0) {
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

void mm2RichPresenceInfo::UpdatePresence(DiscordRichPresence &presence) {
    LPCSTR state;
    string_buf<128> details;

    int8_t instance = 0;

    presence.largeImageKey = NULL;
    presence.largeImageText = NULL;
    presence.smallImageKey = NULL;
    presence.smallImageText = NULL;

    if (inGame) {
        bool raceMode = isRaceMode();
        
        if (inMultiplayer) {
            state = (raceMode) ? "Racing online" : "In multiplayer";
            instance = 0;
        }
        else {
            state = (raceMode) ? "In a race" : "In singleplayer";
        }

        switch (MMSTATE->GameMode) {
            case dgGameMode::Cruise:
            case dgGameMode::CRoam:
                details.set("Cruisin' around");
                break;
            case dgGameMode::Checkpoint:
            case dgGameMode::Circuit:
            case dgGameMode::Blitz:
            case dgGameMode::CrashCourse:
                details.format("%s: %s", gameModeNames[MMSTATE->GameMode], raceName);
                break;
            case dgGameMode::CnR:
                details.format("%s", gameModeNames[MMSTATE->GameMode]);
                break;
        }

        presence.largeImageText = city;
        presence.largeImageKey = cityImageKey;
        presence.smallImageText = vehicle;
        presence.smallImageKey = vehicleImageKey;
    } else {
        if (inMultiplayer) {
            state = "In multiplayer";
            details.set("In lobby");
            presence.largeImageKey = "mpmenu";
            presence.largeImageText = "Multiplayer lobby";

            instance = 0;
        }
        else {
            state = "In main menu";
            details.set("");
            presence.largeImageKey = "menu";
            presence.largeImageText = "Main menu";
        }
    }

    presence.state = state;
    presence.details = details;

    //Multiplayer side, to implement
    presence.instance = instance;
    presence.partyId = lobbyId;
    presence.partyMax = lobbyMaxPlayers;
    presence.partySize = lobbyNumPlayers;

    LogFile::WriteLine("[discord] Updating presence...");
    _Discord_UpdatePresence(&presence);
}

int discordHandler::GameInit(void) {
    LogFile::WriteLine("[discord] GameInit called.");

    get<mmGame>()->mmGame::Init();

    mmCityInfo * cityInfo = CityListPtr->GetCurrentCity();
    mmVehInfo * vehInfo = VehicleListPtr->GetVehicleInfo(MMSTATE->VehicleName);

    g_mm2Info.inGame = true;
    g_mm2Info.city = cityInfo->GetLocalisedName();
    g_mm2Info.cityImageKey = getCityImageKey(cityInfo);
    g_mm2Info.vehicle = vehInfo->GetDescription();
    g_mm2Info.vehicleImageKey = getCarImageKey(vehInfo);
    g_mm2Info.raceName = getRaceName(MMSTATE->RaceId);
    g_mm2Info.UpdatePresence(presence);

    return 1;
}

void discordHandler::GameBeDone(int) {
    LogFile::WriteLine("[discord] GameBeDone called.");

    g_mm2Info.inGame = false;
    g_mm2Info.city = NULL;
    g_mm2Info.cityImageKey = NULL;
    g_mm2Info.vehicle = NULL;
    g_mm2Info.vehicleImageKey = NULL;
    g_mm2Info.raceName = NULL;
    g_mm2Info.UpdatePresence(presence);
}

int discordHandler::DetectHostMPLobby(char *sessionName, char *sessionPassword, int sessionMaxPlayers, NETSESSION_DESC *sessionData) {
    int result = NETMGR->CreateSession(sessionName, sessionPassword, sessionMaxPlayers, sessionData);

    if (result) {
        LogFile::WriteLine("Entered multiplayer lobby");
        g_mm2Info.inMultiplayer = true;
        g_mm2Info.lobbyNumPlayers = 1;
        g_mm2Info.lobbyMaxPlayers = sessionMaxPlayers;
        g_mm2Info.UpdatePresence(presence);
    }

    return result;
}

byte data[sizeof(DPSESSIONDESC2)]{ NULL };

int discordHandler::DetectJoinMPLobby(char *a2, GUID *a3, char *a4) {
    int result = NETMGR->JoinSession(a2, a3, a4);

    if (result) {
        LogFile::WriteLine("Entered multiplayer lobby");
        g_mm2Info.inMultiplayer = true;

        DWORD dataSize = 0;

        IDirectPlay4 *dplay = NETMGR->pDPlay;

        dplay->GetSessionDesc(NULL, &dataSize); //Get the data size

        dplay->GetSessionDesc(&data, &dataSize); //Populate our data buffer

        auto desc = (DPSESSIONDESC2*)data;

        g_mm2Info.lobbyMaxPlayers = desc->dwMaxPlayers;
        g_mm2Info.UpdatePresence(presence);
    }

    return result;
}

int discordHandler::DetectJoinMPLobbySession(void) {
    int result = NETMGR->JoinLobbySession();

    if (result) {
        LogFile::WriteLine("Entered multiplayer lobby session");
        g_mm2Info.inMultiplayer = true;

        DWORD dataSize = 0;

        IDirectPlay4 *dplay = NETMGR->pDPlay;

        dplay->GetSessionDesc(NULL, &dataSize); //Get the data size

        dplay->GetSessionDesc(&data, &dataSize); //Populate our data buffer

        auto desc = (DPSESSIONDESC2*)data;

        g_mm2Info.lobbyMaxPlayers = desc->dwMaxPlayers;
        g_mm2Info.UpdatePresence(presence);
    }

    return result;
}

void discordHandler::DetectDisconnectMPLobby(void) {
    LogFile::WriteLine("Exited multiplayer lobby");
    g_mm2Info.inMultiplayer = false;
    g_mm2Info.lobbyMaxPlayers = NULL;
    g_mm2Info.lobbyNumPlayers = NULL;
    g_mm2Info.UpdatePresence(presence);

    NETMGR->Disconnect();
}

void discordHandler::DetectDisconnectMPGame(void) {
    LogFile::WriteLine("Exited multiplayer game");
    g_mm2Info.inMultiplayer = false;
    g_mm2Info.lobbyMaxPlayers = NULL;
    g_mm2Info.lobbyNumPlayers = NULL;
    g_mm2Info.UpdatePresence(presence);

    NETMGR->CloseSession();
}

int discordHandler::RefreshNumPlayersLobby(void) {
    g_mm2Info.lobbyNumPlayers = NETMGR->GetNumPlayers();
    g_mm2Info.UpdatePresence(presence);
    return g_mm2Info.lobbyNumPlayers;
}

void discordHandler::Install() {
    bool discordLoaded = (cfgUseRichPresence && LoadDiscordModule());

    if (discordLoaded)
    {
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
                cbHook<CALL>(0x572782),     //asNetwork::JoinSession(int, char *)
            }
        );
        InstallCallback("asNetwork::JoinLobbySession", "Update the multiplayer status to on when joining the lobby session.",
            &DetectJoinMPLobbySession, {
                cbHook<CALL>(0x409C6F),     //mmInterface::MessageCallback
                cbHook<CALL>(0x410126),     //mmInterface::InitLobby
            }
        );
        InstallCallback("asNetwork::Disconnect", "Update the multiplayer status to off when exiting the lobby.",
            &DetectDisconnectMPLobby, {
                cbHook<CALL>(0x40D394),     //mmInterface::Switch
            }
        );
        InstallCallback("asNetwork::CloseSession", "Update the multiplayer status to off when exiting the current game.",
            &DetectDisconnectMPGame, {
                cbHook<CALL>(0x43B159),     //mmGameMulti::QuitNetwork
            }
        );
        InstallCallback("asNetwork::GetNumPlayers", "Updates the number of players in a lobby",
            &RefreshNumPlayersLobby, {
                cbHook<CALL>(0x4111B1),     //mmInterface::RefreshPlayers
            }
        );

        InitDiscord();
        g_mm2Info.UpdatePresence(presence);

        LogFile::WriteLine("[discord] Rich Presence initialized successfully.");
    }
    else if (cfgUseRichPresence)
    {
        Warningf("**** Discord Rich Presence was NOT loaded! ****");
    }
}

void discordHandler::Release() {
    if (g_DiscordModule != nullptr)
    {
        LogFile::WriteLine("[discord] Shutting down...");
        _Discord_Shutdown();

        FreeLibrary(g_DiscordModule);
        g_DiscordModule = nullptr;
    }
}