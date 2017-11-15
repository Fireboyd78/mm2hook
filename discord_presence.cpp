#include "discord_presence.h"

#include "logfile.h"
#include "mm2_utils.h"
#include "mm2_common.h"
#include "discord-rpc.h"
#include "util.h"
#include "common.h"

#include <stdio.h>
#include <string.h>

declhook(0x4011E0, _Func<void>, $Main);

using namespace MM2;

static const char* APPLICATION_ID = "379767166267817984";

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

void handleDiscordJoinRequest(const DiscordJoinRequest * request) {
	Warningf("Handling Discord join request...\n");
}

//--discordRichPresence--
void discordRichPresence::InitDiscord() {
	DiscordEventHandlers handlers;
	memset(&handlers, 0, sizeof(handlers));
	handlers.ready = handleDiscordReady;
	handlers.errored = handleDiscordError;
	handlers.disconnected = handleDiscordDisconnected;
	handlers.joinGame = handleDiscordJoinGame;
	handlers.spectateGame = handleDiscordSpectateGame;
	handlers.joinRequest = handleDiscordJoinRequest;
	//Discord_Initialize(APPLICATION_ID, &handlers, 1, NULL);

	//$Main();
}

void discordRichPresence::Install() {
	InstallCallback("discordRichPresence::InitDiscord", "Inits Discord Rich Presence.",
		&InitDiscord, {
			cbHook<CALL>(0x4022F1),
		}
	);
}