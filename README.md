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
**Note for Wine users:** In order to use this on Wine, you must open winecfg, go to the Libraries tab, and add an override for dinput. Set the override to either "native" or "native then builtin"
 
## Contributing
If you'd like to contribute to the project, please [fork](https://help.github.com/articles/about-forks/) the project and submit a [pull request](https://help.github.com/articles/about-pull-requests/).
 
All contributions are welcomed, including ones that fix something as simple as a typo.
 
#### Coding Style
Please use spaces instead of tabs, and try to keep your code consistent with the rest of the project.

## Arguments

mm2hook supports some custom parameters to enable or disable functions and patches. These are the most commonly used:

| Argument | Parameters | Description |
| :------- | :--------: | :---------- |
| -nopause | *N/A*      | Allows the game to keep running even when unfocused. |
| -width | `size_in_pixels` | Sets the window width in pixels. |
| -height | `size_in_pixels` | Sets the window height in pixels. |
| -sw | *N/A* | Forces the game to run in software mode. |
| -borderless | *N/A* | Sets the window to borderless mode. |
| -novsync | *N/A* | Disables vertical sync. |
| -nohwtnl | *N/A* | Disables hardware T&L. |
| -windowed | *N/A* | Forces the game to windowed mode. |
| -fullscreen | *N/A* | Forces the game to fullscreen mode. |
| -bitdepth | `bit_depth` | Sets the bit depth in bits. |
| -heapsize | `size_in_mb` | Sets the heap size (in megabytes). |
