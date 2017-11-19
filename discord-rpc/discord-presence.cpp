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

void UpdateDiscord(mm2RichPresenceInfo &mm2Info) {
    LPCSTR state;
    LPCSTR largeImageText = NULL;
    LPCSTR smallImageText = NULL;

    LPCSTR partyID = NULL;
    int partySize = NULL;
    int partyMax = NULL;

    int8_t instance = NULL;

    char stateBuf[256]{ NULL };
    char details[256]{ NULL };

    if (mm2Info.inRace) {
        if (mm2Info.inMultiplayer) {
            state = "In a multiplayer race";
            instance = 0;
            partySize = 1;
            partyMax = 8;
        }
        else {
            state = "In a race";
        }

        if ((mm2Info.vehicle != NULL) && (mm2Info.city != NULL)) {          
            largeImageText = mm2Info.city;
            smallImageText = mm2Info.vehicle;
        }

        if (strcmp(mm2Info.raceMode, "Cruise") == 0 || strcmp(mm2Info.raceMode, "Cops N' Robbers") == 0) {
            sprintf(details, "%s", mm2Info.raceMode);
        }
        else {
            sprintf(details, "%s: %s", mm2Info.raceMode, mm2Info.raceName);
        }
    } else {
        state = "In main menu";
        mm2Info.cityImageKey = "menu";
        largeImageText = "Main menu";
    }

    presence.largeImageKey = mm2Info.cityImageKey;
    presence.largeImageText = largeImageText;
    presence.smallImageKey = mm2Info.carImageKey;
    presence.smallImageText = smallImageText;
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

const char* getCarImageKey(mm2RichPresenceInfo &mm2Info) {
    const char* carDescription = mm2Info.vehicle;
    const char* carImageKey = "";

    //Compare the car descriptions so we can use the appropriate images
    if (strcmp(carDescription, "Mini Cooper Classic") == 0) {
        carImageKey = "vpcoop";
    }
    else if (strcmp(carDescription, "VW New Beetle") == 0) {
        carImageKey = "vpbug";
    }
    else if (strcmp(carDescription, "London Cab") == 0) {
        carImageKey = "vpcab";
    }
    else if (strcmp(carDescription, "Cadillac Eldorado") == 0) {
        carImageKey = "vpcaddie";
    }
    else if (strcmp(carDescription, "Ford F-350 Super Duty") == 0) {
        carImageKey = "vpford";
    }
    else if (strcmp(carDescription, "Ford Mustang GT") == 0) {
        carImageKey = "vpmustang99";
    }
    else if (strcmp(carDescription, "Ford Mustang Cruiser") == 0) {
        carImageKey = "vpcop";
    }
    else if (strcmp(carDescription, "Ford Mustang Fastback") == 0) {
        carImageKey = "vpbullet";
    }
    else if (strcmp(carDescription, "Panoz AIV Roadster") == 0) {
        carImageKey = "vppanoz";
    }
    else if (strcmp(carDescription, "City Bus") == 0) {
        carImageKey = "vpbus";
    }
    else if (strcmp(carDescription, "Double-Decker Bus") == 0) {
        carImageKey = "vpddbus";
    }
    else if (strcmp(carDescription, "Freightliner Century Class") == 0) {
        carImageKey = "vpcentury";
    }
    else if (strcmp(carDescription, "New Mini Cooper") == 0) {
        carImageKey = "vpcoop2k";
    }
    else if (strcmp(carDescription, "VW New Beetle Dune") == 0) {
        carImageKey = "vpdune";
    }
    else if (strcmp(carDescription, "VW New Beetle RSi") == 0) {
        carImageKey = "vpvwcup";
    }
    else if (strcmp(carDescription, "Light Tactical Vehicle") == 0) {
        carImageKey = "vp4x4";
    }
    else if (strcmp(carDescription, "Audi TT Coupe") == 0) {
        carImageKey = "vpauditt";
    }
    else if (strcmp(carDescription, "Aston Martin DB7 Vantage") == 0) {
        carImageKey = "vpdb7";
    }
    else if (strcmp(carDescription, "Panoz GTR-1") == 0) {
        carImageKey = "vppanozgt";
    }
    else if (strcmp(carDescription, "American LaFrance Eagle") == 0) {
        carImageKey = "vplafrance";
    }
    
    else {
        carImageKey = "nocardesc";
    }

    return carImageKey;
}

const char* getCityImageKey(mm2RichPresenceInfo &mm2Info) {
    const char* cityName = mm2Info.city;
    const char* cityImageKey = "";

    //Compare the city name so we can use the appropriate images
    if (strcmp(cityName, "San Francisco") == 0) {
        cityImageKey = "sf";
    }
    else if (strcmp(cityName, "London") == 0) {
        cityImageKey = "london";
    }
    else {
        cityImageKey = "nocitydesc";
    }

    return cityImageKey;
}

const char* getRaceMode() {
    const char* raceMode = NULL;

    switch (gameMode) {
    case 0: raceMode = "Cruise";            break;
    case 1: raceMode = "Checkpoint";        break;
    case 2: raceMode = "Cops N' Robbers";   break;
    case 3: raceMode = "Circuit";           break;
    case 4: raceMode = "Blitz";             break;
    case 6: raceMode = "Crash Course";      break;
    }

    return raceMode;
}

int discordHandler::GameInit(void) {
    LogFile::WriteLine("[discord] GameInit called.");

    ageHook::Thunk<0x412710>::Call<int>(this); // mmGame::init

    mmCityInfo * cityInfo = CityListPtr->GetCurrentCity();
    mmVehInfo * vehInfo = VehicleListPtr->GetVehicleInfo(vehicleName);

    g_mm2Info.city = cityInfo->GetLocalisedName(); // TODO: get localised name
    g_mm2Info.vehicle = vehInfo->GetDescription();
    g_mm2Info.carImageKey = getCarImageKey(g_mm2Info);
    g_mm2Info.cityImageKey = getCityImageKey(g_mm2Info);
    g_mm2Info.raceMode = getRaceMode();
    g_mm2Info.raceName = "Who knows???";
    g_mm2Info.inRace = true;
    g_mm2Info.inMultiplayer = false; // TODO: update this properly
    UpdateDiscord(g_mm2Info);

    return 1;
}

void discordHandler::GameBeDone(int) {
    LogFile::WriteLine("[discord] GameBeDone called.");

    g_mm2Info.city = NULL;
    g_mm2Info.vehicle = NULL;
    g_mm2Info.raceMode = NULL;
    g_mm2Info.raceName = NULL;
    g_mm2Info.inRace = false;
    g_mm2Info.inMultiplayer = false;
    g_mm2Info.cityImageKey = "menu";
    g_mm2Info.carImageKey = NULL;
    UpdateDiscord(g_mm2Info);
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

    InitDiscord();
    UpdateDiscord(g_mm2Info);
}

void discordHandler::Release() {
    LogFile::WriteLine("[discord] Shutting down...");
    Discord_Shutdown();
}