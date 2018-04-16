MM2Hook
=======
[![Build Status](https://ci.appveyor.com/api/projects/status/github/Fireboyd78/mm2hook?branch=master&svg=true)](https://ci.appveyor.com/project/Fireboyd78/mm2hook)

A simple DLL-injection module for hooking into Midtown Madness 2, allowing for further control of the game.

Also included are complimentary bug-fixes for random crashes, as well as a fix for the infamous **heap overrun** error.

## Features
<img align="right" src="https://i.imgur.com/A5ZMdzB.png" alt="Discord Rich Presence support for MM2Hook" />

 * Console output window
 * Lua scripting
 * Plugins (TODO)
 
 * ### [Discord Rich Presence](https://discordapp.com/rich-presence) Integration:
   * Dynamically updates based on the state of your game (playing a race, cruising around, in the main menu, etc.)
   * Shows a thumbnail picture for default vehicles/cities
     - Thumbnail text shows the full name (mods included!)
   * May support Multiplayer in the future!
 
## Installation
1) Grab the latest version [here](https://ci.appveyor.com/project/Fireboyd78/mm2hook/build/artifacts).
2) **Optional:** Download the latest version of Discord Rich Presence [here](https://github.com/discordapp/discord-rpc/releases/latest), and grab `discord-rpc.dll` from the `win32-dynamic\bin` folder.
3) Place all files in your Midtown Madness 2 installation directory and enjoy!
 
## Contributing
If you'd like to contribute to the project, please [fork](https://help.github.com/articles/about-forks/) the project and submit a [pull request](https://help.github.com/articles/about-pull-requests/).
 
All contributions are welcomed, including ones that fix something as simple as a typo.
 
#### Coding Style
Please use spaces instead of tabs, and try to keep your code consistent with the rest of the project.
