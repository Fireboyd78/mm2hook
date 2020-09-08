local M = {}
local imgui = require("imgui")

--exposed vars
M.useCache = false

--local vars
local modsPath = "lua/mods"

--mods table
local mods = {}

--init!
local function loadMod(path)
    local convertedPath = path:gsub("%/", "\\"):sub(1,-5)
    if M.useCache == false then
        package.loaded[convertedPath] = nil
    end

    Displayf("Loading mod " .. path)
    local loadedMod = require(convertedPath)
    if loadedMod == nil then
        Errorf("Failed to load mod: " .. convertedPath .. ".")
    elseif type(loadedMod) ~= "table" then
        Errorf("Failed to load mod: " .. convertedPath .. ", got invalid return value.")
    else
        table.insert(mods, loadedMod)
        Displayf("Loaded mod " .. path .. ".")
    end
end

local function ends_with(str, ending)
   return ending == "" or str:sub(-#ending) == ending
end

local function loadMods(path)
    for file in lfs.dir(path) do
        if file ~= "." and file ~= ".." then
            local fullPath = path..'/'..file

            --check if this is a mod
            if ends_with(fullPath:lower(), ".lua") then
                loadMod(fullPath)
            end

            --recursive search mods
            local attr = lfs.attributes (fullPath)
            if attr ~= nil and attr.mode == "directory" then
                loadMods (fullPath)
            end
        end
    end
end

local function init()
    local attrib = lfs.attributes(modsPath)
    if attrib == nil then
        lfs.mkdir("lua/mods")
        attrib = lfs.attributes(modsPath)
    end

    if attrib == nil or attrib.mode ~= "directory" then
        Errorf("modsystem: cannot load mods because mods isn't a directory??")
    else
        Warningf("modsystem.init: loading mods")
        loadMods(modsPath)
        Warningf("modsystem.init: loading mods complete")
    end
end

--mods functions
local function tick()
    for _, mod in ipairs(mods) do
        if mod.tick ~= nil then mod.tick() end
    end
end

local function initMods()
    for _, mod in ipairs(mods) do
        if mod.init ~= nil then mod.init() end
    end
end

local function onChatMessage(message)
    for _, mod in ipairs(mods) do
        if mod.onChatMessage ~= nil then mod.onChatMessage(message) end
    end
end

local function onGameEnd()
    for _, mod in ipairs(mods) do
        if mod.onGameEnd ~= nil then mod.onGameEnd() end
    end
end

local function onGameInit()
    for _, mod in ipairs(mods) do
        if mod.onGameInit ~= nil then mod.onGameInit() end
    end
end

local function onSessionCreate(name, password, maxPlayers, details)
    for _, mod in ipairs(mods) do
        if mod.onSessionCreate ~= nil then mod.onSessionCreate(name, password, maxPlayers, details) end
    end
end

local function onSessionJoin(a2, a3, a4)
    for _, mod in ipairs(mods) do
        if mod.onSessionJoin ~= nil then mod.onSessionJoin(a2, a3, a4) end
    end
end

local function onDisconnect()
    for _, mod in ipairs(mods) do
        if mod.onDisconnect ~= nil then mod.onDisconnect() end
    end
end

local function onReset()
    for _, mod in ipairs(mods) do
        if mod.onReset ~= nil then mod.onReset() end
    end
end

local function onRenderUi()
    --Render the main bar
    if imgui.BeginMainMenuBar() then
      for _, mod in ipairs(mods) do
          if mod.drawMenuBar ~= nil then mod.drawMenuBar() end
      end
      imgui.EndMainMenuBar()
    end

    --Render mod windows etc
    for _, mod in ipairs(mods) do
        if mod.onRenderUi ~= nil then mod.onRenderUi() end
    end
end

local function restart()
    for _, mod in ipairs(mods) do
        if mod.restart ~= nil then mod.restart() end
    end
end

local function shutdown()
    for _, mod in ipairs(mods) do
        if mod.shutdown ~= nil then mod.shutdown() end
    end
end

--exports
M.init = init

M.initMods = initMods
M.onChatMessage = onChatMessage
M.onRenderUi = onRenderUi
M.tick = tick
M.onGameInit = onGameInit
M.onGameEnd = onGameEnd
M.onSessionCreate = onSessionCreate
M.onSessionJoin = onSessionJoin
M.onDisconnect = onDisconnect
M.onReset = onReset
M.restart = restart
M.shutdown = shutdown

return M