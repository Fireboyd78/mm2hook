#include "discord-presence.h"

using namespace MM2;

static const char* APPLICATION_ID = "381733554725584896";

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
    // initialize rich presence detail
    presence.state = "Loading...";
    presence.largeImageKey = "canary-large";
    presence.smallImageKey = "ptb-small";
    presence.partyId = "party1234";
    presence.partySize = 1;
    presence.partyMax = 8;
    presence.matchSecret = "xyzzyx";
    presence.joinSecret = "join";
    presence.spectateSecret = "spectate";
    presence.instance = 0;

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

    char stateBuf[256];
    char details[256];

    if (mm2Info.inRace) {
        state = (mm2Info.inMultiplayer) ? "In a multiplayer race" : "In a race";

        if ((mm2Info.vehicle != NULL) && (mm2Info.city != NULL)) {
            sprintf(details, "%s | %s", mm2Info.vehicle, mm2Info.city);
        }
    } else {
        state = "In main menu";
    }

    if (mm2Info.race != NULL) {
        sprintf(stateBuf, "%s: %s", state, mm2Info.race);
    } else {
        sprintf(stateBuf, "%s", state);
    }

    presence.state = state;
    presence.details = details;

    LogFile::WriteLine("[discord] Updating presence...");
    Discord_UpdatePresence(&presence);
}

int discordHandler::GameInit(void) {
    LogFile::WriteLine("[discord] GameInit called.");

    ageHook::Thunk<0x412710>::Call<int>(this); // mmGame::init

    g_mm2Info.city = cityName; // TODO: get localised name
    g_mm2Info.vehicle = vehicleName;
    g_mm2Info.race = raceName;
    g_mm2Info.inRace = true;
    g_mm2Info.inMultiplayer = false; // TODO: update this properly
    UpdateDiscord(g_mm2Info);

    return 1;
}

void discordHandler::GameBeDone(int) {
    LogFile::WriteLine("[discord] GameBeDone called.");

    g_mm2Info.city = NULL;
    g_mm2Info.vehicle = NULL;
    g_mm2Info.race = NULL;
    g_mm2Info.inRace = false;
    g_mm2Info.inMultiplayer = false;
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
}

void discordHandler::Release() {
    LogFile::WriteLine("[discord] Shutting down...");
    Discord_Shutdown();
}