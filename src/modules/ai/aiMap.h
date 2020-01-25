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
    class aiMap;

    // External declarations
    extern class asNode;

    // Class definitions

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
        void *roomsThing;

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
        
    public:
        static ageHook::Type<aiMap> Instance;

        /*
            asNode virtuals
        */

        AGE_API void Cull() override                { ageHook::Thunk<0x5374F0>::Call<void>(this); }
        AGE_API void Update() override              { ageHook::Thunk<0x536E50>::Call<void>(this); }
        AGE_API void UpdatePaused() override        { ageHook::Thunk<0x5374E0>::Call<void>(this); }
        AGE_API void Reset() override               { ageHook::Thunk<0x536A30>::Call<void>(this); }
        
        /*
            aiMap
        */
        AGE_API void Dump(void)                              { ageHook::Thunk<0x538840>::Call<void>(this); }
        AGE_API void TestProbes(BOOL a2)                     { ageHook::Thunk<0x53B870>::Call<void>(this, a2); }
        AGE_API aiVehicleAmbient * Vehicle(int num)          { return ageHook::Thunk<0x5348B0>::Call<aiVehicleAmbient *>(this, num); }
        AGE_API aiPedestrian * Pedestrian(int num)           { return ageHook::Thunk<0x534AB0>::Call<aiPedestrian *>(this, num); }
        AGE_API aiIntersection* Intersection(int num)        { return ageHook::Thunk<0x534880>::Call<aiIntersection*>(this, num); }
        AGE_API aiPath* Path(int num)                        { return ageHook::Thunk<0x534850>::Call<aiPath*>(this, num); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<aiMap, asNode>("aiMap")
                .addFunction("Dump", &Dump)
                .addFunction("TestProbes", &TestProbes, LUA_ARGS(bool))
                .addFunction("Pedestrian", &Pedestrian)
                .addFunction("Path", &Path)
                .addFunction("Intersection", &Intersection)
                .addFunction("Vehicle", &Vehicle)
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