#pragma once
#include <modules\ai.h>

namespace MM2
{
    // Forward declarations
    class aiPath;

    // External declarations
    extern class aiPedestrian;
    extern class aiVehicleAmbient;
    extern class aiVehicleSpline;
    extern class aiIntersection;
    extern class aiObstacle;

    // Class definitions

    class aiPath {
    private:
        int field_0;
        int RInterRoadIdx;
        short Count;
        short Id;
        short Flags;
        short NumBlocks;
        short* Blocks;
        float HalfWidth;
        float SpeedLimit;
        int NumPeds;
        aiPedestrian* PedFirst;
        int NumRoadTurns;
        void* RoadTurns;
        char AmbPlayerFlags;
        char PedPlayerFlags;
        int field_30;
        int field_34;
        short LNumLanes;
        short LNumTrams;
        short LNumTrains;
        short LUnk1;
        short LAmbientType;
        float* LLaneDistances;
        float* LOuterDistances;
        float LUnk2[10];
        float* LLaneShit;
        void* LTramData;
        void* LTrainData;
        void* LUnk3;
        aiVehicleAmbient* LAmbients;
        aiVehicleSpline* LSplines;
        int LUnk4;
        aiObstacle* LObstacles;
        void* LBangers;
        aiPedestrian* LPedestrians;
        short RNumLanes;
        short RNumTrams;
        short RNumTrains;
        short RUnk1;
        short RAmbientType;
        float* RLaneDistances;
        float* ROuterDistances;
        float RUnk2[10];
        float* RLaneShit;
        void* RTramData;
        void* RTrainData;
        void* RUnk3;
        aiVehicleAmbient* RAmbients;
        aiVehicleSpline* RSplines;
        int RUnk4;
        aiObstacle* RObstacles;
        void* RBangers;
        aiPedestrian* RPedestrians;
        float RSectionDist;
        int SectionOrigin;
        Vector3 SectionAxis;
        int Tangents;
        aiIntersection* LIntersection;
        short LInterUnk1;
        short LInterVehicleRule;
        short LInterRoadIdx;
        short LInterUnk3;
        Vector3 LInterTrafficLightOrigin;
        Vector3 LInterTrafficLightAxis;
        aiIntersection* RIntersection;
        short RInterUnk1;
        short RInterVehicleRule;
        short RInterUnk2;
        short RInterUnk3;
        Vector3 RInterTrafficLightOrigin;
        Vector3 RInterTrafficLightAxis;
        short AllwaysGo;
        short AllwaysStop;
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