#pragma once
#include <modules\phys\bound.h>
#include <mm2_model.h>

namespace MM2
{
    // Forward declarations
    class lvlInstance;
    class lvlInstanceLuaIterator;

    // External declarations
    extern class cltLight;
    extern class dgPhysEntity;
    extern class gfxViewport;
    extern class modStatic;
    extern class modShader;
    extern class modPackage;

    // External declarations for Lua casting
    extern class dgBangerInstance;
    extern class vehCarModel;
    extern class vehTrailerInstance;
    extern class lvlFixedAny;
    extern class lvlFixedRotY;
    extern class lvlFixedMatrix;
    extern class lvlLandmark;
    extern class dgUnhitYBangerInstance;
    extern class phBoundBox;
    extern class phBoundGeometry;
    extern class phBoundHotdog;
    extern class phBoundSphere;
    extern class phBoundTerrain;
    extern class phBoundTerrainLocal;
    extern class phForceSphere;

    // Class definitions
    enum InstFlags : unsigned __int16
    {
        INST_FLAG_1 = 0x1,
        INST_FLAG_2 = 0x2,
        INST_FLAG_4 = 0x4,
        INST_FLAG_8 = 0x8,
        INST_FLAG_10 = 0x10,
        INST_FLAG_20 = 0x20,
        INST_FLAG_40 = 0x40,
        INST_FLAG_80 = 0x80,
        INST_FLAG_100 = 0x100,
        INST_FLAG_200 = 0x200,
    };

    class lvlInstance
    {
    private:
        byte Owner;
        byte Subtype;

        short room;
        ushort Flags;

        ushort GeomIndex;

        lvlInstance* Previous;
        lvlInstance* Next;

        int castLua(lua_State* L)
        {
            auto vtblPtr = *reinterpret_cast<uintptr_t*>(this);
            switch (vtblPtr) {
                case 0x5B150C: //dgUnhitYBangerInstance
                    LuaState(L).push(reinterpret_cast<dgUnhitYBangerInstance*>(this));
                    break;
                case 0x5B1494: //dgUnhitMtxBangerInstance
                case 0x5B54AC: //aiTrafficLightInstance
                case 0x5B5F8C: //gizBridge
                case 0x5B6180: //gizFerry
                case 0x5B60D4: //gizTrainCar
                case 0x5B15B8: //dgHitBangerInstance
                    LuaState(L).push(reinterpret_cast<dgBangerInstance*>(this));
                    break;
                case 0x5B2CB0: //vehCarModel
                    LuaState(L).push(reinterpret_cast<vehCarModel*>(this));
                    break;
                case 0x5B2F84: //vehTrailerInstance
                    LuaState(L).push(reinterpret_cast<vehTrailerInstance*>(this));
                    break;
                case 0x5B1B10: //lvlFixedAny
                    LuaState(L).push(reinterpret_cast<lvlFixedAny*>(this));
                    break;
                case 0x5B1B78: //lvlFixedRotY
                    LuaState(L).push(reinterpret_cast<lvlFixedRotY*>(this));
                    break;
                case 0x5B1BE0: //lvlFixedMatrix
                    LuaState(L).push(reinterpret_cast<lvlFixedMatrix*>(this));
                    break;
                case 0x5B1A88: //lvlLandmark
                    LuaState(L).push(reinterpret_cast<lvlLandmark*>(this));
                    break;
                default:
                    LuaState(L).push(this);
                    break;
            }
            return 1;
        }

        int getBoundLua(lua_State* L, int a1)
        {
            auto bound = this->GetBound(a1);
            if (bound == nullptr) {
                LuaState(L).push(nullptr);
                return 1;
            }

            switch (bound->getType()) 
            {
            case phBound::BoundType::Box:
                LuaState(L).push(reinterpret_cast<phBoundBox*>(bound));
                break;
            case phBound::BoundType::Geometry:
                LuaState(L).push(reinterpret_cast<phBoundGeometry*>(bound));
                break;
            case phBound::BoundType::Hotdog:
                LuaState(L).push(reinterpret_cast<phBoundHotdog*>(bound));
                break;
            case phBound::BoundType::Level:
                LuaState(L).push(nullptr);
                break;
            case phBound::BoundType::ForceSphere:
                LuaState(L).push(reinterpret_cast<phForceSphere*>(bound));
                break;
            case phBound::BoundType::Sphere:
                LuaState(L).push(reinterpret_cast<phBoundSphere*>(bound));
                break;
            case phBound::BoundType::Terrain:
                LuaState(L).push(reinterpret_cast<phBoundTerrain*>(bound));
                break;
            case phBound::BoundType::TerrainLocal:
                LuaState(L).push(reinterpret_cast<phBoundTerrainLocal*>(bound));
                break;
            default:
                LuaState(L).push(bound);
                break;
            }
            return 1;
        }
    protected:
        static AGE_API int GetGeomSet(char const * a1, char const * a2, int a3)
                                                            { return hook::StaticThunk<0x4632C0>::Call<int>(a1, a2, a3); }
        static AGE_API void CreateTempBounds()              { hook::StaticThunk<0x464680>::Call<void>(); }
        static AGE_API void DeleteTempBounds()              { hook::StaticThunk<0x4647E0>::Call<void>(); }
    public:
        struct GeomTableEntry
        {
            modStatic *LOD[4];
            modShader **pShaders;
            phBound *Bound;
            float Radius;
            char numShaders;
            char numShadersPerPaintjob;

            inline modStatic * GetLOD(int lod) const {
                if (lod < 0 || lod > 3)
                    return nullptr;

                return this->LOD[lod];
            }

            inline modStatic * GetHighestLOD() const {
                for (int i = 3; i >= 0; i--) {
                    if (this->LOD[i] != nullptr)
                        return this->LOD[i];
                }
                return nullptr;
            }

            inline modStatic * GetLowestLOD() const {
                for (int i = 0; i < 4; i++) {
                    if (this->LOD[i] != nullptr)
                        return this->LOD[i];
                }
                return nullptr;
            }

            inline modStatic * GetVeryLowLOD() const {
                return this->LOD[0];
            }

            inline modStatic * GetLowLOD() const {
                return this->LOD[1];
            }

            inline modStatic * GetMedLOD() const {
                return this->LOD[2];
            }

            inline modStatic * GetHighLOD() const {
                return this->LOD[3];
            }

            inline float GetRadius() const {
                return Radius;
            }

            inline phBound* GetBound() const {
                return this->Bound;
            }

            static void BindLua(LuaState L) {
                LuaBinding(L).beginClass<GeomTableEntry>("GeomTableEntry")
                    //fields
                    .addPropertyReadOnly("VL", &GetVeryLowLOD)
                    .addPropertyReadOnly("L", &GetLowLOD)
                    .addPropertyReadOnly("M", &GetMedLOD)
                    .addPropertyReadOnly("H", &GetHighLOD)
                    .addPropertyReadOnly("Radius", &GetRadius)
                    .addPropertyReadOnly("Bound", &GetBound)
                    .addFunction("GetLOD", &GetLOD)
                    .addFunction("GetHighestLOD", &GetHighestLOD)
                    .addFunction("GetLowestLOD", &GetLowestLOD)
                .endClass();
            }

        };
        ASSERT_SIZEOF(GeomTableEntry, 32);

        static char* GetGeomName(int id) {
            if (id >= GetGeomSetCount())
                return nullptr;
            return GetGeomNameTablePtr()[id];
        }

        static GeomTableEntry* GetGeomTableEntry(int id) {
            if (id >= GetGeomSetCount())
                return nullptr;
            return &GetGeomTablePtr()[id];
        }

        static GeomTableEntry* GetGeomTablePtr() {
            return reinterpret_cast<GeomTableEntry*>(0x6316D8);
        }

        static char** GetGeomNameTablePtr() {
            return reinterpret_cast<char**>(0x651760);
        }

        static int GetGeomSetCount() {
            return *reinterpret_cast<int*>(0x655764);
        }
    private:
        Matrix34 getMatrixLua() 
        {
            Matrix34 discard;
            auto matrix = this->GetMatrix(&discard);
            discard.Set(matrix);
            return discard;
        }

        Vector3 getPositionLua() 
        {
            auto position = this->GetPosition();
            return Vector3(position);
        }

        void setPositionLua(Vector3 position)
        {
            auto matrix = getMatrixLua();
            matrix.SetRow(3, position);
            this->SetMatrix(matrix);
        }
    public:
        LEVEL_ALLOCATOR

        AGE_API lvlInstance(void) 
        {
            scoped_vtable x(this);
            hook::Thunk<0x4631F0>::Call<void>(this);
        }

        AGE_API ~lvlInstance(void) 
        {
            scoped_vtable x(this);
            hook::Thunk<0x463220>::Call<void>(this);
        }

        inline lvlInstance* GetNext() const
        {
            return this->Next;
        }

        inline lvlInstance* GetPrevious() const
        {
            return this->Previous;
        }

        inline short GetRoomId() const
        {
            return room;
        }

        inline short GetGeomIndex() const
        {
            return GeomIndex;
        }

        inline void SetGeomIndex(unsigned short id)
        {
            GeomIndex = id;
        }

        inline short GetFlags() const 
        {
            return this->Flags;
        }

        inline void SetFlags(ushort flags) 
        {
            this->Flags = flags;
        }

        inline void SetFlag(ushort flag) 
        {
            this->Flags |= flag;
        }

        inline byte GetOwner(void) const {
            return this->Owner;
        }

        inline void SetOwner(byte owner) {
            this->Owner = owner;
        }

        inline byte GetSubType(void) const {
            return this->Subtype;
        }

        inline void SetSubType(byte subtype) {
            this->Subtype = subtype;
        }


        static AGE_API bool ComputeShadowMatrix(Matrix34 *outMatrix, int room, Matrix34 const *inMatrix)
                                                            { return hook::StaticThunk<0x464430>::Call<bool>(outMatrix, room, inMatrix); }
        static AGE_API void ResetInstanceHeap()             { hook::StaticThunk<0x4631A0>::Call<void>(); }
        static AGE_API void ResetAll()                      { hook::StaticThunk<0x4631E0>::Call<void>(); }
        static AGE_API void SetShadowBillboardMtx(Matrix44 &a1)
                                                            { hook::StaticThunk<0x463290>::Call<void>(&a1); }

        static AGE_API int AddSphere(float a1)              { return hook::StaticThunk<0x463D50>::Call<int>(a1); }

        AGE_API bool LoadBoundOnLastEntry(const char *a1)   { return hook::Thunk<0x463940>::Call<bool>(this, a1); }
        AGE_API Vector4 * GetBoundSphere(Vector4 *a1)       { return hook::Thunk<0x463A40>::Call<Vector4 *>(this, a1); }

        AGE_API bool BeginGeom(const char *a1, const char *a2, int a3)
                                                            { return hook::Thunk<0x463A80>::Call<bool>(this, a1, a2, a3); }
        AGE_API int AddGeom(const char *a1, const char *a2, int a3)
                                                            { return hook::Thunk<0x463BA0>::Call<int>(this, a1, a2, a3); }
        AGE_API void EndGeom()                              { hook::Thunk<0x463BC0>::Call<void>(this); }

        AGE_API bool InitBoundTerrain(const char *a1)       { return hook::Thunk<0x463DA0>::Call<bool>(this, a1); }
        AGE_API bool InitBoundTerrainLocal(const char *a1)  { return hook::Thunk<0x463F50>::Call<bool>(this, a1); }
        AGE_API bool NeedGhostBound(const Vector3 *a1, int a2)
                                                            { return hook::Thunk<0x4641A0>::Call<bool>(this, a1, a2); }
        AGE_API bool InitGhostBound(phBound *a1, const Vector3 *a2, int a3)
                                                            { return hook::Thunk<0x464200>::Call<bool>(this, a1, a2, a3); }
        AGE_API int InitGhost(const char *a1, const Matrix34 &a2)
                                                            { return hook::Thunk<0x464330>::Call<int>(this, a1, &a2); }

        AGE_API void PreLoadShader(int a1)                  { hook::Thunk<0x464B00>::Call<void>(this, a1); }
        AGE_API void Optimize(int a1)                       { hook::Thunk<0x464B70>::Call<void>(this, a1); }

        /*
            Custom additions
        */
        bool BeginGeomWithGroup(const char* a1, const char* a2, const char* group, int a3)
        {
            string_buf<256> groupedName("%s_%s", a1, group);

            *(bool*)0x651720 = (a3 & 8) != 0; //set dontPreload
            *(const char**)0x6516DC = groupedName; //set lastName
            auto PackageHash = (HashTable*)0x651728;

            

            int existingGeomSet;
            if (PackageHash->Access(groupedName, &existingGeomSet))
            {
                this->GeomIndex = existingGeomSet;
            }
            else
            {
                auto package = new modPackage();
                *(modPackage**)0x0651740 = package; //set PKG

                if (package->Open("geometry", a1))
                {
                    this->GeomIndex = GetGeomSet(a1, a2, a3);
                    PackageHash->Insert(groupedName, (void*)this->GeomIndex);
                    return true;
                }

                if (package != nullptr)
                {
                    delete package;
                }

                *(modPackage**)0x0651740 = nullptr; //set PKG
                PackageHash->Insert(groupedName, (void*)0);
            }
            return false;
        }

        /*
            lvlInstance virtuals
        */

        virtual AGE_API void Reset()                        { hook::Thunk<0x463280>::Call<void>(this); }
        virtual AGE_API const Vector3 & GetPosition()       PURE;
        virtual AGE_API int IsVisible(const gfxViewport *a1)
                                                            { return hook::Thunk<0x4649F0>::Call<int>(this, a1); }

        virtual AGE_API const Matrix34 & GetMatrix(Matrix34 *a1)
                                                            PURE;
        virtual AGE_API void SetMatrix(const Matrix34 & a1) PURE;
        
        virtual AGE_API void SetVariant(int a1)             { hook::Thunk<0x4643D0>::Call<void>(this, a1); }
        virtual AGE_API const float GetRadius()             { return hook::Thunk<0x4643E0>::Call<float>(this); }
        virtual AGE_API dgPhysEntity * GetEntity()          { return hook::Thunk<0x4643B0>::Call<dgPhysEntity *>(this); }
        virtual AGE_API dgPhysEntity * AttachEntity()       { return hook::Thunk<0x4643C0>::Call<dgPhysEntity *>(this); }
        virtual AGE_API const Vector3 & GetVelocity()       { return hook::Thunk<0x4643A0>::Call<const Vector3 &>(this); }
        virtual AGE_API void Detach()                       { hook::Thunk<0x43FC30>::Call<void>(this); }
        
        virtual AGE_API void Draw(int)                      PURE;
        virtual AGE_API void DrawShadow()                   { hook::Thunk<0x4643F0>::Call<void>(this); }
        virtual AGE_API void DrawShadowMap()                { hook::Thunk<0x464400>::Call<void>(this); }
        virtual AGE_API void DrawGlow()                     { hook::Thunk<0x464410>::Call<void>(this); }
        virtual AGE_API void DrawReflected(float a1)        { hook::Thunk<0x464420>::Call<void>(this, a1); }
        virtual AGE_API void DrawReflectedParts(int a1)     { hook::Thunk<0x4648B0>::Call<void>(this, a1); }
        virtual AGE_API int Init(const char *a1, const Matrix34 &a2, int a3)
                                                            {return hook::Thunk<0x463D90>::Call<int>(this, a1, &a2, a3); }

        virtual AGE_API unsigned int SizeOf(void)           PURE;

        virtual AGE_API bool IsLandmark()                   { return hook::Thunk<0x463180>::Call<bool>(this); }
        virtual AGE_API bool IsCollidable()                 { return hook::Thunk<0x43FC40>::Call<bool>(this); }
        virtual AGE_API bool IsTerrainCollidable()          { return hook::Thunk<0x43FC50>::Call<bool>(this); }
        virtual AGE_API int GetNumLightSources()            { return hook::Thunk<0x4632B0>::Call<int>(this); }
        virtual AGE_API void GetLightInfo(int a1, cltLight *a2)
                                                            { hook::Thunk<0x4630B0>::Call<void>(this); }
        virtual AGE_API int SetupGfxLights(const Matrix34 &a1)
                                                            { return hook::Thunk<0x464670>::Call<int>(this, &a1); }
        virtual AGE_API phBound * GetBound(int a1)          { return hook::Thunk<0x4648C0>::Call<phBound *>(this, a1); };

        static void BindLuaLate(LuaState L) {
            LuaBinding(L).beginClass<lvlInstance>("lvlInstance")
                .addFunction("GetBound", &getBoundLua)
                .addFunction("Cast", &castLua)
            .endClass();
        }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<lvlInstance>("lvlInstance")
                //fields
                .addProperty("Owner", &GetOwner, &SetOwner)
                .addProperty("SubType", &GetSubType, &SetSubType)
                .addProperty("Flags", &GetFlags, &SetFlags)
                .addProperty("GeometrySetIndex", &GetGeomIndex, &SetGeomIndex) //legacy
                .addProperty("GeomIndex", &GetGeomIndex, &SetGeomIndex)
                .addPropertyReadOnly("Previous", &GetPrevious)
                .addPropertyReadOnly("Next", &GetNext)
                .addPropertyReadOnly("CurrentRoom", &GetRoomId)

                //statics
                .addStaticFunction("GetGeomName", &GetGeomName)
                .addStaticFunction("GetGeomTableEntry", &GetGeomTableEntry)
                .addStaticProperty("GeomTableSize", &GetGeomSetCount)

                .addStaticFunction("ResetInstanceHeap", &ResetInstanceHeap)
                .addStaticFunction("ResetAll", &ResetAll)
                .addStaticFunction("SetShadowBillboardMtx", &SetShadowBillboardMtx)
                .addStaticFunction("GetGeomSet", &GetGeomSet)

                //functions
                .addFunction("LoadBoundOnLastEntry", &LoadBoundOnLastEntry)
                .addFunction("GetBoundSphere", &GetBoundSphere)
                .addFunction("BeginGeom", &BeginGeom)
                .addFunction("AddGeom", &AddGeom)
                .addFunction("EndGeom", &EndGeom)
                .addStaticFunction("AddSphere", &AddSphere)
                .addFunction("InitBoundTerrain", &InitBoundTerrain)
                .addFunction("InitBoundTerrrainLocal", &InitBoundTerrainLocal)
                .addFunction("NeedGhostBound", &NeedGhostBound)
                .addFunction("InitGhostBound", &InitGhostBound)
                .addFunction("InitGhost", &InitGhost)
                .addStaticFunction("CreateTempBounds", &CreateTempBounds)
                .addStaticFunction("DeleteTempBounds", &DeleteTempBounds)
                .addFunction("PreLoadShader", &PreLoadShader)
                .addFunction("Optimize", &Optimize)

                //virtuals
                .addFunction("Reset", &Reset)
                .addFunction("IsVisible", &IsVisible)
                .addFunction("GetMatrix", &getMatrixLua)
                .addFunction("GetPosition", &getPositionLua)
                .addFunction("SetMatrix", &SetMatrix)
                .addFunction("SetPosition", &setPositionLua)
                .addFunction("SetVariant", &SetVariant)
                .addFunction("GetRadius", &GetRadius)
                .addFunction("GetEntity", &GetEntity)
                .addFunction("AttachEntity", &AttachEntity)
                .addFunction("GetVelocity", &GetVelocity)
                .addFunction("Detach", &Detach)
                .addFunction("Draw", &Draw)
                .addFunction("DrawShadow", &DrawShadow)
                .addFunction("DrawShadowMap", &DrawShadowMap)
                .addFunction("DrawGlow", &DrawGlow)
                .addFunction("DrawReflected", &DrawReflected)
                .addFunction("DrawReflectedParts", &DrawReflectedParts)
                .addFunction("Init", &Init)
                .addFunction("SizeOf", &SizeOf)
                .addFunction("IsLandmark", &IsLandmark)
                .addFunction("IsCollidable", &IsCollidable)
                .addFunction("IsTerrainCollidable", &IsTerrainCollidable)
                .addFunction("GetNumLightSources", &GetNumLightSources)
                //addFunction("GetLightInfo", [](int light) { cltLight light; lvlInstance::GetLightInfo(light, &light); return light; })
                .addFunction("SetupGfxLights", &SetupGfxLights)
                //.addFunction("GetBound", &GetBound) -> moved to BindLuaLate
            .endClass();
        }
    };
    ASSERT_SIZEOF(lvlInstance, 0x14);

    class LvlInstanceLuaIterator : public CppFunctor
    {
    private:
        lvlInstance* last;
        lvlInstance* end;
    public:
        LvlInstanceLuaIterator(lvlInstance* instance)
        {
            this->last = instance;
        }

        LvlInstanceLuaIterator(lvlInstance* instance, lvlInstance* end)
        {
            this->last = instance;
            this->end = end;
        }

        virtual ~LvlInstanceLuaIterator()
        {
        }

        virtual int run(lua_State* L) override
        {
            if (this->last)
            {
                LuaState(L).push(this->last);
                this->last = this->last->GetNext();
                if (this->last == this->end)
                    this->last = nullptr;
                return 1;
            }
            else
            {
                return 0;
            }
        }
    };

    // Lua initialization
}