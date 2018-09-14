#pragma once
#include <modules\city.h>

namespace MM2
{
    // Forward declarations
    class lvlAiMap;
    class lvlAiRoad;

    // External declarations
    extern class Stream;

    // Class definitions

    class lvlAiMap {
    public:
        AGE_API static void SetRoad(lvlSDL const * a1, int a2, bool a3) { ageHook::StaticThunk<0x45D720>::Call<void>(a1, a2, a3); }
        AGE_API static lvlAiRoad* GetRoad(int a1) { return ageHook::StaticThunk<0x45D840>::Call<lvlAiRoad*>(a1); }
        AGE_API static int GetRoom(int a1) { return ageHook::StaticThunk<0x45DA00>::Call<int>(a1); }
        AGE_API static int GetNumRoads() { return ageHook::StaticThunk<0x45D860>::Call<int>(); }
        AGE_API static int GetNumRooms() { return ageHook::StaticThunk<0x45DA58>::Call<int>(); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<lvlAiMap>("lvlAiMap")
                .addStaticFunction("SetRoad", &SetRoad)
                .addStaticFunction("GetRoad", &GetRoad)
                .addStaticFunction("GetRoom", &GetRoom)
                .addStaticFunction("GetNumRoads", &GetNumRoads)
                .addStaticFunction("GetNumRooms", &GetNumRooms)
            .endClass();
        }
    };

    class lvlAiRoad {
    public:
        AGE_API void LoadBinary(Stream *a1) { ageHook::Thunk<0x45D870>::Call<void>(this, a1); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<lvlAiRoad>("lvlAiRoad")
                .addFunction("LoadBinary", &LoadBinary)
            .endClass();
        }
    };

    // Lua initialization

}