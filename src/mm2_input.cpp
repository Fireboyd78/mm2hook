#include "mm2_input.h"
#include "mm2_lua.h"
#include "mm2_game.h"

using namespace MM2;

bool isConsoleOpen = false;

MM2FnHook<bool> ioEventQueue::$Pop      ( NULL, NULL, 0x4BA930 );
MM2FnHook<bool> ioEventQueue::$Peek     ( NULL, NULL, 0x4BA980 );
MM2FnHook<void> ioEventQueue::$Queue    ( NULL, NULL, 0x4BA9D0 );
MM2FnHook<void> ioEventQueue::$Command  ( NULL, NULL, 0x4BAA50 );



bool inputCallback::HandleKeyPress(DWORD vKey)
{
    // Inform Lua of any changes beforehand
    MM2Lua::OnKeyPress(vKey);

    switch (vKey)
    {
        // '~'
        case VK_OEM_2:
        case VK_OEM_3:
        {
            // tell the game to open a chat box,
            // and then use a local variable to check if it's open

            mmGameManager *mgr = mmGameManager::Instance();
            auto gamePtr = mgr->getGame();

            if (gamePtr != NULL)
            {
                auto popup = gamePtr->getPopup();

                if (popup != NULL)
                {
                    // don't try opening it again if it's already open
                    if (popup->IsEnabled() && isConsoleOpen)
                        return true;

                    popup->ProcessChat();
                    isConsoleOpen = true;
                }
            }
        } return true;
    }
    return false;
}

void inputCallback::ChatHandler::Process(char *message)
{
    if (isConsoleOpen)
    {
        MM2Lua::SendCommand(message);

        // return normal chatbox behavior
        isConsoleOpen = false;
    }
    else
    {
        LogFile::Format("Got chat message: %s\n", message);
    }
}