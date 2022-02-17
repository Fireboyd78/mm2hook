#pragma once
#include <modules\phys.h>

namespace MM2
{
    // Forward declarations
    class phPhysManager;
    class dgPhysManager;
    struct PhysicsStats;

    // External declarations
    extern class dgPhysEntity;
    extern class lvlInstance;
    extern struct lvlSegment;
    extern struct phIntersectionPoint;
    extern struct lvlIntersection;

    // Class definitions
    struct LuaRaycastResult 
    {
    private:
        lvlSegment* segment;
        lvlIntersection* isect;
    private:
        inline Vector3 getIntersectionPoint()
        {
            return isect->IntersectionPoint.Point;
        }

        inline Vector3 getNormal()
        {
            return isect->IntersectionPoint.Normal;
        }

        inline float getNormalizedDistance()
        {
            return isect->IntersectionPoint.NormalizedDistance;
        }

        inline float getPenetration()
        {
            return isect->IntersectionPoint.Penetration;
        }

        inline phBound* getBound()
        {
            return isect->Bound;
        }

        inline phPolygon* getPolygon()
        {
            return isect->Poly;
        }
    public:
        LuaRaycastResult(lvlSegment* segment, lvlIntersection* isect) 
        {
            this->segment = segment;
            this->isect = isect;
        }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<LuaRaycastResult>("LuaRaycastResult")
                .addPropertyReadOnly("NormalizedDistance", &getNormalizedDistance)
                .addPropertyReadOnly("Penetration", &getPenetration)
                .addPropertyReadOnly("Point", &getIntersectionPoint)
                .addPropertyReadOnly("Bound", &getBound)
                .addPropertyReadOnly("Normal", &getNormal)
                .addPropertyReadOnly("Polygon", &getPolygon)
                .endClass();
        }
    };

    struct PhysicsStats
    {
    public:
        float TotalUpdateTime;
        float ObjPairCollectingTime;
        float TotalCollisionTime;
        float MoverUpdateTime;
        float MoverGatherTime;
        float ObjToObjCollisionTime;
        float ObjToObjImpactTime;
        float CarImpactTime;
        float ObjTerrainCollisionTime;
        float ObjTerrainImpactTime;
        float PostCollisionTime;
        float CollisionsTime;

        int MoverVsMover;
        int MoverVsCollidable;

        int TotalProbes;
        float TotalProbeTime;
        int ExternalProbes;
        float ExternalProbeTime;
        
        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<PhysicsStats>("PhysicsStats")
                .addVariableRef("TotalUpdateTime", &PhysicsStats::TotalUpdateTime)
                .addVariableRef("ObjPairCollectingTime", &PhysicsStats::ObjPairCollectingTime)
                .addVariableRef("TotalCollisionTime", &PhysicsStats::TotalCollisionTime)
                .addVariableRef("MoverUpdateTime", &PhysicsStats::MoverUpdateTime)
                .addVariableRef("MoverGatherTime", &PhysicsStats::MoverGatherTime)
                .addVariableRef("ObjToObjCollisionTime", &PhysicsStats::ObjToObjCollisionTime)
                .addVariableRef("ObjToObjImpactTime", &PhysicsStats::ObjToObjImpactTime)
                .addVariableRef("CarImpactTime", &PhysicsStats::CarImpactTime)
                .addVariableRef("ObjTerrainCollisionTime", &PhysicsStats::ObjTerrainCollisionTime)
                .addVariableRef("ObjTerrainImpactTime", &PhysicsStats::ObjTerrainImpactTime)
                .addVariableRef("PostCollisionTime", &PhysicsStats::PostCollisionTime)
                .addVariableRef("TotalProbes", &PhysicsStats::TotalProbes)
                .addVariableRef("TotalProbeTime", &PhysicsStats::TotalProbeTime)
                .addVariableRef("ExternalProbes", &PhysicsStats::ExternalProbes)
                .addVariableRef("ExternalProbeTime", &PhysicsStats::ExternalProbeTime)
                .addVariableRef("MoverVsMover", &PhysicsStats::MoverVsMover)
                .addVariableRef("MoverVsCollidable", &PhysicsStats::MoverVsCollidable)
                .addVariableRef("CollisionsTime", &PhysicsStats::CollisionsTime)
                .endClass();
        }
    };

    class phPhysicsManager {
    public:
        virtual bool TestProbe(phSegment const &a1, phIntersection* a2, unsigned int a3, int a4)             PURE;
        virtual bool TestSphere(Vector3 const &a1, float a2, class phImpact* a3, unsigned int a4, int a5)    PURE;
        virtual void PromoteInstance(class phInstance* a1)                                                   PURE;
        virtual void DemoteInstance(class phInstance* a1)                                                    PURE;
        virtual void KillInstance(class phInstance* a1)                                                      PURE;
        virtual void DisableInstance(class phInstance* a1)                                                   PURE;
        virtual void EnableInstance(class phInstance* a1, int a2)                                            PURE;
        virtual class phColliderBase *  GetCollider(class phInstance* a1)                                    PURE;
        virtual class phInstance* CreateInstance()                                                           PURE;
    };

    class dgPhysManager : phPhysicsManager, asCullable {
    public:
        struct CollisionTableEntry
        {
        private:
            lvlInstance* Instance;
            dgPhysEntity* PhysEntity;
            short CollidablesCount;
            lvlInstance* Collidables[32];
            short Flags;
            byte byte_8e;
            byte byte_8f;
        public:
            inline short getFlags()
            {
                return this->Flags;
            }

            inline lvlInstance* getInstance()
            {
                return this->Instance;
            }

            inline int getCollidablesCount()
            {
                return this->CollidablesCount;
            }

            inline lvlInstance* getCollidable(int num)
            {
                if (num < 0 || num >= this->getCollidablesCount())
                    return nullptr;
                return this->Collidables[num];
            }

            static void BindLua(LuaState L) {
                LuaBinding(L).beginClass<dgPhysManager>("dgPhysManager")
                    .beginClass<CollisionTableEntry>("CollisionTableEntry")
                        .addPropertyReadOnly("NumCollidables", &getCollidablesCount)
                        .addPropertyReadOnly("Instance", &getInstance)
                        .addFunction("GetCollidable", &getCollidable)
                        .endClass();
            }
        };
    private:
        int NumActiveRooms;
        int NumCulledMovers;
        int ActiveRoomIds[20];
        int NumSamples;
        float perfTotalUpdateTime;
        float perfObjObjCollision;
        float perfObjObjImpact;
        float perfObjTerrainCollision;
        float perfObjTerrainImpact;
        int dword_78;
        float CarDamageImpactTime;
        int NumMovers;
        int MoverVsMover; // unused
        int MoverVsCollidable;
        int ExternalProbes; // unused
        float TotalProbeTime; // unused
        float ExternalProbeTime; // unused
        BOOL IsUpdating;
        class phContactMgr * ContactMgr;
        int LinesMode; // unused
        dgPhysManager::CollisionTableEntry Table[32];
        int NumActiveMovers;
        int MaxSamples;
        float SampleStep;
    private:
        std::shared_ptr<LuaRaycastResult> collideLua(Vector3 start, Vector3 end)
        {
            lvlSegment segment;
            lvlIntersection isect;
            segment.Set(start, end, 0, nullptr);
            
            bool collided = dgPhysManager::Collide(segment, &isect, 0, nullptr, 0x20, 0);
            LuaRaycastResult* result = (collided) ? new LuaRaycastResult(&segment, &isect) : nullptr;
            return std::shared_ptr<LuaRaycastResult>(result);
        }
    public:
        inline PhysicsStats getStats()
        {
            auto stats = PhysicsStats();
            stats.CarImpactTime = this->CarDamageImpactTime;
            stats.CollisionsTime = dgPhysManager::perfPhysCollide.get();
            stats.MoverVsCollidable = this->MoverVsCollidable;
            stats.MoverVsMover = this->MoverVsMover;
            stats.MoverGatherTime = dgPhysManager::perfPhysGathering.get();
            stats.MoverUpdateTime = dgPhysManager::perfMoverUpdate.get();
            stats.ObjPairCollectingTime = dgPhysManager::perfObjPairCollecting.get();
            stats.ObjTerrainCollisionTime = this->perfObjTerrainCollision;
            stats.ObjTerrainImpactTime = this->perfObjTerrainImpact;
            stats.ObjToObjCollisionTime = this->perfObjObjCollision;
            stats.ObjToObjImpactTime = this->perfObjObjImpact;
            stats.PostCollisionTime = dgPhysManager::perfPostCollision.get();
            stats.TotalCollisionTime = this->perfTotalCollisionTime;
            stats.TotalUpdateTime = this->perfTotalUpdateTime;

            //MM2 doesn't keep track of these
            //would need to rewrite dgPhysManager
            stats.ExternalProbes = 0;
            stats.ExternalProbeTime = 0.f;
            stats.TotalProbes = 0;
            stats.TotalProbeTime = 0.f;

            return stats;
        }

        inline int getMoverCount()
        {
            return this->NumMovers;
        }

        inline int getActiveRoomCount()
        {
            return this->NumActiveRooms;
        }

        inline int getActiveRoomId(int index)
        {
            if (index > 20 || index < 0)
                return 0;
            return this->ActiveRoomIds[index];
        }

        inline int getNumSamples()
        {
            return this->NumSamples;
        }

        inline int getNumCulledMovers()
        {
            return this->NumCulledMovers;
        }

        inline int getMaxSamples()
        {
            return this->MaxSamples;
        }

        inline void setMaxSamples(int samples)
        {
            this->MaxSamples = samples;
        }

        inline float getSampleStep()
        {
            return this->SampleStep;
        }

        inline void setSampleStep(float step)
        {
            this->SampleStep = step;
        }


        CollisionTableEntry* getMover(int num)
        {
            if (num < 0 || num >= this->getMoverCount())
                return nullptr;
            return &this->Table[num];
        }

        CollisionTableEntry* findMover(lvlInstance* instance)
        {
            int count = this->getMoverCount();
            for (int i = 0; i < count; i++) 
            {
                auto entry = getMover(i);
                if (entry->getInstance() == instance)
                    return entry;
            }
            return nullptr;
        }
    
        static inline float getGravity() 
        {
            return dgPhysManager::Gravity.get();
        }

        static inline void setGravity(float gravity) 
        {
            dgPhysManager::Gravity.set(gravity);
        }
    public:
        static hook::Type<lvlInstance *> PlayerInst;
        static hook::Type<dgPhysManager *> Instance;
        static hook::Type<float> Gravity;
        static hook::Type<int> OpponentOptimization;

        static hook::Type<float> perfObjPairCollecting;
        static hook::Type<float> perfMoverUpdate;
        static hook::Type<float> perfPhysGathering;
        static hook::Type<float> perfPhysCollide;
        static hook::Type<float> perfTotalCollisionTime;
        static hook::Type<float> perfPostCollision;

        AGE_API void IgnoreMover(lvlInstance* instance)                  { hook::Thunk<0x468860>::Call<void>(this, instance); }
        AGE_API void DeclareMover(lvlInstance* instance, int a2, int a3) { hook::Thunk<0x468370>::Call<void>(this, instance, a2, a3); }
        AGE_API bool Collide(lvlSegment& segment, lvlIntersection* intersection, int roomId, lvlInstance* ignoreInstance, ushort flags1, int flags2)
                                                                         { return hook::Thunk<0x468E40>::Call<bool>(this, &segment, intersection, roomId, ignoreInstance, flags1, flags2); }
        AGE_API bool CollideProbe(lvlSegment& segment, lvlIntersection* intersection, lvlInstance* instance)
                                                                         { return hook::Thunk<0x469110>::Call<bool>(this, &segment, intersection, instance); }
        AGE_API bool TrivialCollideInstances(lvlInstance* bangerInstance, lvlInstance* bangerInstance2)
                                                                         { return hook::Thunk<0x4692E0>::Call<bool>(this, bangerInstance, bangerInstance2); }
        AGE_API bool CollideInstances(lvlInstance* instance, lvlInstance* bangerInstance)
                                                                         { return hook::Thunk<0x469620>::Call<bool>(this, instance, bangerInstance); }
        AGE_API bool CollideTerrain(dgPhysManager::CollisionTableEntry* entry)
                                                                         { return hook::Thunk<0x469A60>::Call<bool>(this, entry); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<dgPhysManager>("dgPhysManager")
                //statics
                .addStaticProperty("Instance", [] { return (dgPhysManager*)Instance; })

                //properties
                .addPropertyReadOnly("Stats", &getStats)

                .addPropertyReadOnly("NumMovers", &getMoverCount)
                .addPropertyReadOnly("NumActiveRooms", &getActiveRoomCount)
                .addPropertyReadOnly("NumCulledMovers", &getNumCulledMovers)
                .addPropertyReadOnly("NumSamples", &getNumSamples)

                .addProperty("MaxSamples", &getMaxSamples, &setMaxSamples)
                .addProperty("SampleStep", &getSampleStep, &setSampleStep)

                .addStaticProperty("Gravity", &getGravity, &setGravity)
                .addStaticProperty("PlayerInst", [] { return (lvlInstance*)dgPhysManager::PlayerInst; })

                //functions
                .addFunction("Collide", &collideLua)
                .addFunction("IgnoreMover", &IgnoreMover)
                .addFunction("DeclareMover", &DeclareMover)

                .addFunction("FindMover", &findMover)
                .addFunction("GetMover", &getMover)
                .addFunction("GetActiveRoomID", &getActiveRoomId)

                .endClass();
        }
    };

    ASSERT_SIZEOF(dgPhysManager, 0x12B0);

    // Lua initialization

}