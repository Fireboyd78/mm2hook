#pragma once
#include <modules\ai.h>

#include "ArrayList.h"

#include "aiCTFRacer.h"
#include "aiCableCar.h"
#include "aiCityData.h"
#include "aiData.h"
#include "aiIntersection.h"
#include "aiPath.h"
#include "aiPedestrian.h"
#include "aiPoliceForce.h"
#include "aiPoliceOfficer.h"
#include "aiRouteRacer.h"
#include "aiSubway.h"
#include "aiVehicleMGR.h"
#include "aiVehicleAmbient.h"
#include "aiVehiclePlayer.h"
#include "aiVehicleSpline.h"
#include "mcHookman.h"

namespace MM2
{
    // Forward declarations
    struct aiMapStats;
    class aiMap;
    
    // External declarations
    extern class asNode;

    // Class definitions
    enum aiMapComponentType
    {
        None = 0x0,
        Road = 0x1,
        Shortcut = 0x2,
        Intersection = 0x3,
    };

    struct aiMapStats {
    public:
        float _fSubwayUpdate;
        float _fCableCarUpdate;
        float _fCTFOppUpdate;
        float _fPedUpdate;
        float _fAmbientUpdate;
        float _fCopUpdate;
        int _nPedQty;
        float _fOppUpdate;
        int _nAmbientQty;
        float _fTotUpdate;

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<aiMapStats>("aiMapStats")
                .addVariable("SubwayUpdateTime", &aiMapStats::_fSubwayUpdate, false)
                .addVariable("CableCarUpdateTime", &aiMapStats::_fCableCarUpdate, false)
                .addVariable("CTFOpponentUpdateTime", &aiMapStats::_fCTFOppUpdate, false)
                .addVariable("PedUpdateTime", &aiMapStats::_fPedUpdate, false)
                .addVariable("AmbientUpdateTime", &aiMapStats::_fAmbientUpdate, false)
                .addVariable("CopUpdateTime", &aiMapStats::_fCopUpdate, false)
                .addVariable("OpponentUpdateTime", &aiMapStats::_fOppUpdate, false)
                .addVariable("PedQuantity", &aiMapStats::_nPedQty, false)
                .addVariable("AmbientQuantity", &aiMapStats::_nAmbientQty, false)
                .addVariable("TotalUpdateTime", &aiMapStats::_fTotUpdate, false)
                .endClass();
        }
    };

    struct aiMapComponentListEntry
    {
        int Id;
        aiMapComponentType Type;
    };

    struct aiMapComponentList
    {
        ushort Count;
        aiMapComponentListEntry** Components;
    };

    class aiMap : public asNode {
    public:
        aiVehicleManager *vehicleManager;

        aiRaceData *raceData;
        aiCityData *cityData;

        aiIntersection **intersections;
        int numIntersections;

        aiPath **paths;
        int numPaths;
        int pathDebug; // educated guess (unused)

        aiVehicleAmbient **ambients;
        float trafficDensity;
        int numAmbientVehicles;
        aiVehicleSpline *ambientSpline;

        aiPoliceForce *policeForce;
        aiPoliceOfficer **policeOfficers;
        short numCops;
        short numCopStartPos;
        short copStartPos[10];

        aiRouteRacer **opponents;
        int numOpponents;
        aiCTFRacer **ctfRacers;
        int numCTFRacers;
        mcHookman **hookmen;
        int numHookmen;
        
        aiPedestrian **pedestrians;
        int numPedestrians;
        aiPedestrian *lastPed;
        
        int unk_8C;
        
        int numCableCars;
        aiCableCar **cableCars;
        
        int numSubways;
        aiSubway **subways;
        void *subwayData;

        short unk_A4;
        short unk_A6;

        short gameMode;
        short numLaps;
        short numShortcuts; // educated guess
        short numPlayers;

        aiVehiclePlayer players[4];
        aiVehiclePlayer *lastPlayer;

        CArrayList **arrayList_1;
        CArrayList **arrayList_2;

        aiPath *ambsPath;
        aiPath *pedsPath;

        void *routingNodes;
        void *lastRoutingNode;
        void *routingStuff;
        aiMapComponentList* componentMap;

        bool drawHeadlights;

        float avoidThing_1;
        float avoidThing_2;

        Vector3 racerThing;
        Vector3 curWaypoint;

        short drawMap;
        short drawAppRoads;
        short drawPathIds;
        short unk_1BE;
        short unk_1C0;
        short unk_1C2;
        short pathThing_1; // passed to AllwaysGreen parameter
        short pathThing_2; // passed to AllwaysRed parameter
        short unk_1C8;

        short numAmbients;
    private:
        //helpers
        inline int getAmbientCount() {
            return numAmbientVehicles;
        }

        inline int getPathsCount() {
            return numPaths;
        }

        inline int getIntersectionCount() {
            return numIntersections;
        }

        inline std::tuple<int, int> mapComponentTypeLua(int room) {
            int outId;
            int componentType = this->MapComponentType(room, &outId);
            return std::make_tuple(componentType, outId);
        }

        inline std::tuple<int, int> mapComponentLua(const Vector3& position, int room) {
            short outId, outType;
            this->MapComponent(position, &outId, &outType, room);
            return std::make_tuple((int)outType, (int)outId);
        }

        inline std::tuple<int, int, int> positionToAIMapCompLua(const Vector3& position) {
            short outId, outType, outRoom;
            auto res = this->PositionToAIMapComp(position, &outId, &outType, &outRoom, -1);

            if (res == FALSE)
                return std::make_tuple((int)aiMapComponentType::None, 0, 0);
            return std::make_tuple((int)outType, (int)outId, (int)outRoom);
        }

        aiMapStats getStats() {
            aiMapStats stats;
            stats._fSubwayUpdate = *this->_fSubwayUpdate;
            stats._fCableCarUpdate = *this->_fCableCarUpdate;
            stats._fCTFOppUpdate = *this->_fCTFOppUpdate;
            stats._fPedUpdate = *this->_fPedUpdate;
            stats._fAmbientUpdate = *this->_fAmbientUpdate;
            stats._fCopUpdate = *this->_fCopUpdate;
            stats._nPedQty = *this->_nPedQty;
            stats._fOppUpdate = *this->_fOppUpdate;
            stats._nAmbientQty = *this->_nAmbientQty;
            stats._fTotUpdate = *this->_fTotUpdate;
            return stats;
        }
    private:
        //profiling hooks
        static hook::Type<float> _fSubwayUpdate;
        static hook::Type<float> _fCableCarUpdate;
        static hook::Type<float> _fCTFOppUpdate;
        static hook::Type<float> _fPedUpdate;
        static hook::Type<float> _fAmbientUpdate;
        static hook::Type<float> _fCopUpdate;
        static hook::Type<int>   _nPedQty;
        static hook::Type<float> _fOppUpdate;
        static hook::Type<int>   _nAmbientQty;
        static hook::Type<float> _fTotUpdate;
    public:
        static hook::Type<aiMap> Instance;

        /*
            asNode virtuals
        */

        AGE_API void Cull() override                { hook::Thunk<0x5374F0>::Call<void>(this); }
        AGE_API void Update() override              { hook::Thunk<0x536E50>::Call<void>(this); }
        AGE_API void UpdatePaused() override        { hook::Thunk<0x5374E0>::Call<void>(this); }
        AGE_API void Reset() override               { hook::Thunk<0x536A30>::Call<void>(this); }
        
        /*
            aiMap
        */
        AGE_API void Dump(void)                              { hook::Thunk<0x538840>::Call<void>(this); }
        AGE_API void TestProbes(BOOL a2)                     { hook::Thunk<0x53B870>::Call<void>(this, a2); }
        AGE_API aiRouteRacer * Opponent(int num)             { return hook::Thunk<0x534940>::Call<aiRouteRacer *>(this, num); }
        AGE_API aiPoliceOfficer * Police(int num)            { return hook::Thunk<0x5348F0>::Call<aiPoliceOfficer *>(this, num); }
        AGE_API aiVehiclePlayer * Player(int num)            { return hook::Thunk<0x534AF0>::Call<aiVehiclePlayer *>(this, num); }
        AGE_API aiVehicleAmbient * Vehicle(int num)          { return hook::Thunk<0x5348B0>::Call<aiVehicleAmbient *>(this, num); }
        AGE_API aiPedestrian * Pedestrian(int num)           { return hook::Thunk<0x534AB0>::Call<aiPedestrian *>(this, num); }
        AGE_API aiIntersection* Intersection(int num)        { return hook::Thunk<0x534880>::Call<aiIntersection*>(this, num); }
        AGE_API aiPath* Path(int num)                        { return hook::Thunk<0x534850>::Call<aiPath*>(this, num); }
        aiMapComponentType MapComponentType(int room, int* outId)
                                                             { return hook::Thunk<0x537600>::Call<aiMapComponentType>(this, room, outId); }
        int MapComponent(const Vector3& position, short* outId, short* outType, int room)
                                                             { return hook::Thunk<0x537680>::Call<int>(this, &position, outId, outType, room); }
        BOOL PositionToAIMapComp(const Vector3& position, short* outId, short* outType, short* outRoom, short wantedRoadId)
                                                             { return hook::Thunk<0x5377B0>::Call<BOOL>(this, &position, outId, outType, outRoom, wantedRoadId); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<aiMap, asNode>("aiMap")
                .addFunction("MapComponentType", &mapComponentTypeLua)
                .addFunction("MapComponent", &mapComponentLua)
                .addFunction("PositionToAIMapComp", &positionToAIMapCompLua)
                .addFunction("Dump", &Dump)
                .addFunction("TestProbes", &TestProbes, LUA_ARGS(bool))
                .addFunction("Pedestrian", &Pedestrian)
                .addFunction("Path", &Path)
                .addFunction("Police", &Police)
                .addFunction("Player", &Player)
                .addFunction("Opponent", &Opponent)
                .addFunction("Intersection", &Intersection)
                .addFunction("Vehicle", &Vehicle)
                .addPropertyReadOnly("Stats", &getStats)
                .addPropertyReadOnly("NumAmbientVehicles", &getAmbientCount)
                .addPropertyReadOnly("NumPaths", &getPathsCount)
                .addPropertyReadOnly("NumIntersections", &getIntersectionCount)
                .addStaticProperty("Instance", [] { return  &aiMap::Instance; })
            .endClass();
        }
    };

    ASSERT_SIZEOF(aiMap, 0x1CC);

    // Lua initialization

}