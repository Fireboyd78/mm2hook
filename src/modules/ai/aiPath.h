#pragma once
#include <modules\ai.h>

namespace MM2
{
    // Forward declarations
    class aiPath;

    // External declarations


    // Class definitions

    class aiPath {
    private:
        byte _buffer[0x164];
    private:
        //lua helper
        std::tuple<int, float> luaIsPosOnRoad(Vector3* pos, float margin)
        {
            float outVal = 0.f;
            int posOnRoadVal = this->IsPosOnRoad(pos, margin, &outVal);
            return std::tuple<int, float>(posOnRoadVal, outVal);
        }

    public:
        aiPath(void)                                        DONOTCALL;
        aiPath(const aiPath &&)                             DONOTCALL;

        /*
            aiPath
        */
        AGE_API void ClearAmbients()                        { hook::Thunk<0x547C00>::Call<void>(this); }
        AGE_API void ClearPeds()                            { hook::Thunk<0x547BD0>::Call<void>(this); }
        AGE_API bool HasCableCarLine(int roadSide)          { return hook::Thunk<0x549980>::Call<bool>(this, roadSide); }
        AGE_API bool HasSubwayLine(int roadSide)            { return hook::Thunk<0x5499B0>::Call<bool>(this, roadSide); }
        AGE_API int IsPosOnRoad(Vector3* pos, float margin, float* outDistanceFromCenter)
                                                            { return hook::Thunk<0x548370>::Call<int>(this, pos, margin, outDistanceFromCenter); }
        AGE_API int Lane(Vector3 *pos, int roadSide)        { return hook::Thunk<0x547900>::Call<int>(this, pos, roadSide); }
        AGE_API void UpdatePedestrians()                    { hook::Thunk<0x544150>::Call<void>(this); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<aiPath>("aiPath")
                .addFunction("ClearAmbients", &ClearAmbients)
                .addFunction("ClearPeds", &ClearPeds)
                .addFunction("HasCableCarLine", &HasCableCarLine)
                .addFunction("HasSubwayLine", &HasSubwayLine)
                .addFunction("IsPosOnRoad", &luaIsPosOnRoad)
                .addFunction("Lane", &Lane)
                .addFunction("UpdatePedestrians", &UpdatePedestrians)
                .endClass();
        }
    };

    ASSERT_SIZEOF(aiPath, 0x164);

    // Lua initialization

}