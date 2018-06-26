#include "..\mm2.h"
#include "mm2_network.h"

class GameEventDispatcher {
public:
    static void onGameEnd(int a1);
    static void onGameInit();
    static void onChatMessage(char* message);
    static void onTick();
    static void onSessionCreate(char *sessionName, char *sessionPassword, int sessionMaxPlayers, MM2::NETSESSION_DESC *sessionData);
    static void onSessionJoin(char *a2, GUID *a3, char *a4);
    static void onDisconnect();
    static void onReset();

    static void Install();
};