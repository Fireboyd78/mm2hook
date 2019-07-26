package.loaded.modsystem = nil
local modsystem = require("modsystem")

--MODSYSTEM FORWARDERS
function init()
    local vms, massagems = pcall(modsystem.init)
    if not vms then Errorf(massagems) end

    modsystem.initMods()
end

function tick()
    modsystem.tick()
end

function onChatMessage(message)
    modsystem.onChatMessage(message)
end

function onGameEnd()
    modsystem.onGameEnd()
end

function onGameInit()
    modsystem.onGameInit()
end

function onSessionCreate(name, password, maxPlayers, details)
    modsystem.onSessionCreate(name, password, maxPlayers, details)
end

function onSessionJoin(a2, a3, a4)
    modsystem.onSessionJoin(a2, a3, a4)
end

function onDisconnect()
    modsystem.onDisconnect()
end

function onReset()
    modsystem.onReset()
end

function restart()
    modsystem.restart()
end

function shutdown()
    modsystem.shutdown()
end