local M = {}
M.node = nil

--init keytable when this is first loaded
local keyStateTable = {}
local keyDownTable = {}
local keyUpTable = {}
for i=0, 255 do
  keyStateTable[i] = 0
  keyDownTable[i] = false
  keyUpTable[i] = false
end

--update function
local function Update()
  --go through keys and compare states
  --print("keystatehelper START")
  for i=0, 255 do
    local curKeyState = ioKeyboard.GetKeyState(i)
    local lastKeyState = keyStateTable[i]
    
    if curKeyState ~= 0 and lastKeyState == 0 then
      --key was just pressed
      keyDownTable[i] = true
      keyUpTable[i] = false
    elseif curKeyState == 0 and lastKeyState ~= 0 then
      --key was just let go of
      keyUpTable[i] = true
      keyDownTable[i] = false
    else
      keyDownTable[i] = false
      keyUpTable[i] = false
    end
    
    keyStateTable[i] = curKeyState
  end
  --print("keystatehelper END")
end

--helpers
local function CreateNode(parent)
  if M.node ~= nil then return end 
  
  M.node = luaNode("KeyStateHelper")
  M.node:SetUpdateFunction(Update)
  if parent ~= nil then
    parent:AddChild(M.node)
  else
    Game:AddChild(M.node) --set as a child of the game mode root
  end
end

local function GetKeyHeld(key)
  return ioKeyboard.GetKeyState(key) ~= 0
end

local function GetKeyUp(key)
  return keyUpTable[key] or false
end

local function GetKeyDown(key)
  return keyDownTable[key] or false
end

--interface
M.Update = Update
M.GetKeyUp = GetKeyUp
M.GetKeyDown = GetKeyDown
M.GetKeyHeld = GetKeyHeld
M.CreateNode = CreateNode

return M