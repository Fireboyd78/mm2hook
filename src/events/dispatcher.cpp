#include "dispatcher.h"
#include "..\..\discord-rpc\discord-presence.h"
#include "..\mm2_network.h"
#include "..\handlers\feature_handlers.h"

using namespace MM2;

void GameEventDispatcher::onGameEnd(int a1)
{
    if (MM2Lua::IsEnabled())
        MM2Lua::OnGameEnd();
    TextureVariantHandler::Reset();
    cityTimeWeatherLightingHandler::Reset();
}

void GameEventDispatcher::onGameInit()
{
    if (MM2Lua::IsEnabled())
        MM2Lua::OnGameInit();
    dgBangerInstanceHandler::Reset();
}

void GameEventDispatcher::onChatMessage(char * message)
{
    if (MM2Lua::IsEnabled())
        MM2Lua::OnChatMessage(message);
}

void GameEventDispatcher::onTick()
{
    if(MM2Lua::IsEnabled())
        MM2Lua::OnTick();
}

void GameEventDispatcher::onSessionCreate(char *sessionName, char *sessionPassword, int sessionMaxPlayers, NETSESSION_DESC *sessionData) 
{
    if (MM2Lua::IsEnabled())
        MM2Lua::OnSessionCreate(sessionName, sessionPassword, sessionMaxPlayers, sessionData);
}

void GameEventDispatcher::onSessionJoin(char *a2, GUID *a3, char *a4) 
{
    if (MM2Lua::IsEnabled())
     MM2Lua::OnSessionJoin(a2, a3, a4);
}

void GameEventDispatcher::onDisconnect()
{
    if (MM2Lua::IsEnabled())
        MM2Lua::OnDisconnect();
}

void GameEventDispatcher::onReset() {
    if (MM2Lua::IsEnabled())
        MM2Lua::OnReset();

    //call original
    auto game = mmGameManager::Instance->getGame();
    ageHook::Thunk<0x413D40>::ThisCall<void>(game);
}

void GameEventDispatcher::Install() {
    InstallCallback("mmGame::Reset", "Register onGameReset with dispatcher.",
        &GameEventDispatcher::onReset, {
            cbHook<CALL>(0x433B3C),      //mmGameSingle::Reset
            cbHook<CALL>(0x43A6B7),     //mmGameMulti::Reset
        }
    );
}