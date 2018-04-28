#pragma once
#include "mm2_common.h"
#include "mm2_inst.h"
#include "mm2_ai.h"
#include "mm2_base.h"
#include "mm2_audio.h"
#include "mm2_stream.h"
#include "mm2_level.h"

namespace MM2
{
    // Forward declarations
    class CArrayList;

    class aiMap;
    class aiCableCar;
    class aiCityData;
    class aiCTFRacer;
    class aiRaceData;
    class aiIntersection;
    class aiPath;
    class aiPedestrian;
    class aiPedestrianInstance;
    class aiPoliceForce;
    class aiPoliceOfficer;
    class aiRaceData;
    class aiRouteRacer;
    class aiSubway;
    class aiVehicle;
    class aiVehicleAmbient;
    class aiVehicleManager;
    class aiVehiclePlayer;
    class aiVehicleSpline;

    class lvlAiMap;
    class lvlAiRoad;

    class mcHookman;

    //External declarations
    extern class asNode;
    extern class Stream;

    namespace $
    {
        namespace aiVehicleSpline {
            declhook(0x568410, _MemberFunc<void>, UpdateObstacleMap);
            declhook(0x551CB0, _MemberFunc<void>, StopVoice);
            declhook(0x551CA0, _MemberFunc<void>, PlayHorn);
        }
        namespace aiPath {
            declhook(0x544150, _MemberFunc<void>, UpdatePedestrians);            
        }
        namespace aiPedestrian {
            declhook(0x54B9C0, _MemberFunc<void>, Update);
        }
        namespace aiPoliceForce {
            declhook(0x550EF0, _MemberFunc<void>, Reset);
        }
        namespace aiPoliceOfficer {
            declhook(0x53DBF0, _MemberFunc<void>, StartSiren);
            declhook(0x53DFD0, _MemberFunc<void>, DetectPerpetrator);
        }
        namespace aiVehicleManager {
            declhook(0x6B31AC, _Type<MM2::aiVehicleManager *>, Instance);
        }
    }

    class CArrayList {
    private:
        short count;
        int *data;
    public:
        CArrayList(void) {
            count = 0;
            data = nullptr;
        }

        ~CArrayList(void) {
            count = 0;
            data = nullptr;
        }

        inline int Count() const {
            return count;
        }

        inline int Get(int index) {
            return data[index];
        }

        AGE_API void Append(int value)                      { ageHook::Thunk<0x543D50>::Call<void>(this, value); }

        AGE_API void ReadBinary(Stream *stream)             { ageHook::Thunk<0x543DE0>::Call<void>(this, stream); }
        AGE_API void SaveBinary(Stream *stream)             { ageHook::Thunk<0x543DB0>::Call<void>(this, stream); }
    };

    class aiPedestrian {
    protected:
        ageHook::Field<0xC0, aiPedAudio> _audio;
    public:
        inline aiPedAudio* getAudio(void) const {
            return _audio.ptr(this);
        };
    };

    class aiVehicleManager : public asNode {
    public:
        static ageHook::Type<aiVehicleManager *> Instance;
    };

    class lvlAiRoad {
    public:
        AGE_API void LoadBinary(Stream* a1) { ageHook::Thunk<0x45D870>::Call<void>(this, a1); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<lvlAiRoad>("lvlAiRoad")
                .addFunction("LoadBinary", &LoadBinary)
            .endClass();
        }
    };

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

    class aiCityData {
    private:
        byte _buffer[0x30];
    public:
        aiCityData(void)                                    DONOTCALL;
        aiCityData(const aiCityData &&)                     DONOTCALL;

        virtual ~aiCityData(void)                           DONOTCALL;
    }; 

    class aiIntersection {
    private:
        byte _buffer[0x2C];
    public:
        aiIntersection(void)                                DONOTCALL;
        aiIntersection(const aiIntersection &&)             DONOTCALL;
    };

    class aiPath {
    private:
        byte _buffer[0x164];
    public:
        aiPath(void)                                        DONOTCALL;
        aiPath(const aiPath &&)                             DONOTCALL;
    };

    class aiPedestrianInstance : public lvlInstance {
    private:
        byte _buffer[0x14];
    public:
        aiPedestrianInstance(void)                          DONOTCALL;
        aiPedestrianInstance(const aiPedestrianInstance &&) DONOTCALL;
    };
    
    class aiPoliceForce {
    private:
        byte _buffer[0x3C];
    public:
        aiPoliceForce(void)                                 DONOTCALL;
        aiPoliceForce(const aiPoliceForce &&)               DONOTCALL;
    };
    
    class aiRaceData {
    private:
        byte _buffer[0xC4];
    public:
        aiRaceData(void)                                    DONOTCALL;
        aiRaceData(const aiRaceData &&)                     DONOTCALL;

        virtual ~aiRaceData(void) DONOTCALL;
    };

    class aiObstacle {
    public:
        aiObstacle(void)                                    DONOTCALL;
        aiObstacle(const aiObstacle &&)                     DONOTCALL;

        virtual BOOL InAccident(void)                       { return ageHook::Thunk<0x53F5D0>::Call<BOOL>(this); }
        virtual void Position(Vector3 &a1)                  PURE;
        virtual float Speed(void)                           PURE;
        virtual float BreakThreshold(void)                  { return ageHook::Thunk<0x53F5E0>::Call<float>(this); }
        virtual BOOL Drivable(void)                         { return ageHook::Thunk<0x53F5F0>::Call<BOOL>(this); }
        virtual int CurrentRoadIdx(aiPath **a1, const bool *a2,int *a3)
                                                            PURE;
        virtual int CurrentRdVert(void)                     PURE;
        virtual void PreAvoid(const Vector3 &a1, const Vector3 &a2, float a3, Vector3 &a4, Vector3 &a5)
                                                            PURE;
        virtual float IsBlockingTarget(const Vector3 &a1, const Vector3 &a2, float a3, float a4)
                                                            PURE;
    };

    class aiVehicle : public aiObstacle {
    public:
        aiVehicle(void)                                     DONOTCALL;
        aiVehicle(const aiVehicle &&)                       DONOTCALL;

        void PreAvoid(const Vector3 &a1, const Vector3 &a2, float a3, Vector3 &a4, Vector3 &a5) override
                                                            FORWARD_THUNK;
        float IsBlockingTarget(const Vector3 &a1, const Vector3 &a2, float a3, float a4) override
                                                            FORWARD_THUNK;
        
        virtual void Update(void)                           { return ageHook::Thunk<0x556230>::Call<void>(this); };
        virtual void Reset(void)                            { return ageHook::Thunk<0x556210>::Call<void>(this); };
        virtual int Type(void)                              PURE;
        virtual Matrix34 & GetMatrix(void)                  PURE;
        virtual float FrontBumperDistance(void)             PURE;
        virtual float BackBumperDistance(void)              PURE;
        virtual float LSideDistance(void)                   PURE;
        virtual float RSideDistance(void)                   PURE;
        virtual int CurrentLane(void)                       PURE;
        virtual int CurrentRoadId(void)                     PURE;
        virtual void DrawId(void)                           PURE;
        virtual void ReplayDebug(void)                      { return ageHook::Thunk<0x556D00>::Call<void>(this); }
    };

    class aiVehicleSpline : public aiVehicle {
    protected:
        byte _buffer[0x188];
    public:
        aiVehicleSpline(void)                               DONOTCALL;
        aiVehicleSpline(const aiVehicleSpline &&)           DONOTCALL;

        void Position(Vector3 &a1) override                 FORWARD_THUNK;
        float Speed(void) override                          FORWARD_THUNK;
        int CurrentRoadIdx(aiPath **a1, const bool *a2, int *a3) override
                                                            FORWARD_THUNK;
        int CurrentRdVert(void) override                    FORWARD_THUNK;

        void Update(void) override                          FORWARD_THUNK;
        void Reset(void) override                           FORWARD_THUNK;
        int Type(void) override                             FORWARD_THUNK;
        Matrix34 & GetMatrix(void) override                 FORWARD_THUNK;
        float FrontBumperDistance(void) override            FORWARD_THUNK;
        float BackBumperDistance(void) override             FORWARD_THUNK;
        float LSideDistance(void) override                  FORWARD_THUNK;
        float RSideDistance(void) override                  FORWARD_THUNK;
        int CurrentLane(void) override                      FORWARD_THUNK;
        int CurrentRoadId(void) override                    FORWARD_THUNK;
        void DrawId(void) override                          FORWARD_THUNK;

        virtual void Impact(int a1)                         FORWARD_THUNK;
        virtual class AudImpact * GetAudImpactPtr(void)     FORWARD_THUNK;
        virtual void PlayHorn(float a1, float a2)           FORWARD_THUNK;
        virtual void StopVoice(void)                        FORWARD_THUNK;
    };

    class aiVehicleAmbient : public aiVehicleSpline {
    private:
        byte _buffer[0x14];
    public:
        aiVehicleAmbient(void)                              DONOTCALL;
        aiVehicleAmbient(const aiVehicleAmbient &&)         DONOTCALL;

        void Update(void) override                          FORWARD_THUNK;
        void Reset(void) override                           FORWARD_THUNK;
        int Type(void) override                             FORWARD_THUNK;
        void DrawId(void) override                          FORWARD_THUNK;
        void ReplayDebug(void) override                     FORWARD_THUNK;

        void Impact(int a1) override                        FORWARD_THUNK;
    };
    
    class aiVehiclePlayer : public aiVehicle {
    private:
        byte _buffer[0x2C];
    public:
        aiVehiclePlayer(void)                               DONOTCALL;
        aiVehiclePlayer(const aiVehiclePlayer &&)           DONOTCALL;

        void Position(Vector3 &a1) override                 FORWARD_THUNK;
        float Speed(void) override                          FORWARD_THUNK;
        int CurrentRoadIdx(aiPath **a1, const bool *a2, int *a3) override
                                                            FORWARD_THUNK;
        int CurrentRdVert(void) override                    FORWARD_THUNK;

        void Update(void) override                          FORWARD_THUNK;
        void Reset(void) override                           FORWARD_THUNK;
        int Type(void) override                             FORWARD_THUNK;
        Matrix34 & GetMatrix(void) override                 FORWARD_THUNK;
        float FrontBumperDistance(void) override            FORWARD_THUNK;
        float BackBumperDistance(void) override             FORWARD_THUNK;
        float LSideDistance(void) override                  FORWARD_THUNK;
        float RSideDistance(void) override                  FORWARD_THUNK;
        int CurrentLane(void) override                      FORWARD_THUNK;
        int CurrentRoadId(void) override                    FORWARD_THUNK;
        void DrawId(void) override                          FORWARD_THUNK;
    };

    class aiVehiclePhysics : public aiVehicle {
    private:
        byte _buffer[0x976C];
    public:
        aiVehiclePhysics(void)                              DONOTCALL;
        aiVehiclePhysics(const aiVehiclePhysics &&)         DONOTCALL;

        void Position(Vector3 &a1) override                 FORWARD_THUNK;
        float Speed(void) override                          FORWARD_THUNK;
        int CurrentRoadIdx(aiPath **a1, const bool *a2, int *a3) override
                                                            FORWARD_THUNK;
        int CurrentRdVert(void) override                    FORWARD_THUNK;

        void Update(void) override                          FORWARD_THUNK;
        void Reset(void) override                           FORWARD_THUNK;
        int Type(void) override                             FORWARD_THUNK;
        Matrix34 & GetMatrix(void) override                 FORWARD_THUNK;
        float FrontBumperDistance(void) override            FORWARD_THUNK;
        float BackBumperDistance(void) override             FORWARD_THUNK;
        float LSideDistance(void) override                  FORWARD_THUNK;
        float RSideDistance(void) override                  FORWARD_THUNK;
        int CurrentLane(void) override                      FORWARD_THUNK;
        int CurrentRoadId(void) override                    FORWARD_THUNK;
        void DrawId(void) override                          FORWARD_THUNK;
        void ReplayDebug(void) override                     FORWARD_THUNK;
    };

    class aiRouteRacer : public aiVehiclePhysics {
    private:
        byte _buffer[0x24];
    public:
        aiRouteRacer(void)                                  DONOTCALL;
        aiRouteRacer(const aiRouteRacer &&)                 DONOTCALL;
    };

    class mcHookman : public aiVehiclePhysics {
    private:
        byte _buffer[0x100];
    public:
        mcHookman(void)                                     DONOTCALL;
        mcHookman(const mcHookman &&)                       DONOTCALL;
    };

    class aiCTFRacer : public Base {
    private:
        byte _buffer1[0x28];
        aiVehiclePhysics physics;
        byte _buffer2[0xDC];
    public:
        aiCTFRacer(void)                                    DONOTCALL;
        aiCTFRacer(const aiCTFRacer &&)                     DONOTCALL;
    };

    class aiPoliceOfficer {
    private:
        int unk_00;
        aiVehiclePhysics physics;
        byte _buffer[0xFC];
    public:
        aiPoliceOfficer(void)                               DONOTCALL;
        aiPoliceOfficer(const aiPoliceOfficer &&)           DONOTCALL;
    };

    class aiCableCar : public aiVehicle {
    private:
        byte _buffer[0x180];
    public:
        aiCableCar(void)                                    DONOTCALL;
        aiCableCar(const aiCableCar &&)                     DONOTCALL;
        ~aiCableCar(void)                                   DONOTCALL;

        void Position(Vector3 &a1) override                 FORWARD_THUNK;
        float Speed(void) override                          FORWARD_THUNK;
        int CurrentRoadIdx(aiPath **a1, const bool *a2, int *a3) override
                                                            FORWARD_THUNK;
        int CurrentRdVert(void) override                    FORWARD_THUNK;

        void Update(void) override                          FORWARD_THUNK;
        void Reset(void) override                           FORWARD_THUNK;
        int Type(void) override                             FORWARD_THUNK;
        Matrix34 & GetMatrix(void) override                 FORWARD_THUNK;
        float FrontBumperDistance(void) override            FORWARD_THUNK;
        float BackBumperDistance(void) override             FORWARD_THUNK;
        float LSideDistance(void) override                  FORWARD_THUNK;
        float RSideDistance(void) override                  FORWARD_THUNK;
        int CurrentLane(void) override                      FORWARD_THUNK;
        int CurrentRoadId(void) override                    FORWARD_THUNK;
        void DrawId(void) override                          FORWARD_THUNK;
    };

    class aiSubway : public aiVehicle {
    private:
        byte _buffer[0x148];
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
    public:
        static ageHook::Type<aiMap> Instance;

        AGE_API void Dump(void) {
            ageHook::Thunk<0x538840>::Call<void>(this);
        };

        AGE_API aiPedestrian* Pedestrian(int num) {
            return ageHook::Thunk<0x534AB0>::Call<aiPedestrian *>(this, num);
        }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<aiMap>("aiMap")
                .addFunction("Dump", &Dump)
                .addFunction("Pedestrian", &Pedestrian)
                .addStaticFunction("Instance", [] { return Instance; })
            .endClass();
        }
    };

    template<>
    void luaAddModule<module_ai>(LuaState L) {
        luaBind<aiMap>(L);
        luaBind<lvlAiMap>(L);
        luaBind<lvlAiRoad>(L);
    }

    ASSERT_SIZEOF(aiCableCar, 0x184);
    ASSERT_SIZEOF(aiCityData, 0x34);
    ASSERT_SIZEOF(aiCTFRacer, 0x9878);
    ASSERT_SIZEOF(aiIntersection, 0x2C);
    ASSERT_SIZEOF(aiMap, 0x1CC);
    ASSERT_SIZEOF(aiPath, 0x164);
    ASSERT_SIZEOF(aiPedestrianInstance, 0x3C);
    ASSERT_SIZEOF(aiPoliceOfficer, 0x9870);
    ASSERT_SIZEOF(aiRaceData, 0xC8);
    ASSERT_SIZEOF(aiSubway, 0x14C);
    ASSERT_SIZEOF(aiRouteRacer, 0x9794);
    ASSERT_SIZEOF(aiVehicleAmbient, 0x1A0);
    ASSERT_SIZEOF(aiVehiclePlayer, 0x30);
    ASSERT_SIZEOF(aiVehiclePhysics, 0x9770);
    ASSERT_SIZEOF(aiVehicleSpline, 0x18C);
    ASSERT_SIZEOF(mcHookman, 0x9870);
}