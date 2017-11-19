#pragma once
#include "common.h"
#include "patch.h"
#include "mm2.h"

#include "discord-rpc.h"

struct mm2RichPresenceInfo {
    bool inRace;
    bool inMultiplayer;

    const char *race;
    const char *vehicle;
    const char *city;
};

static mm2RichPresenceInfo g_mm2Info;

class discordHandler {
public:
    int GameInit(void);
    void GameBeDone(int);

	static void Install(void);
    static void Release(void);
};