#pragma once
#include <modules\level.h>

namespace MM2
{
    // Forward declarations
    class lvlInstance;

    // External declarations
    extern class cltLight;
    extern class dgPhysEntity;
    extern class gfxViewport;
    extern class modStatic;
    extern class modShader;
    extern class phBound;
    extern class phBoundGeometry;

    // Class definitions

    class lvlInstance
    {
    private:
        byte byte4;
        byte byte5;

        short room;
        short Flags;

        short GeomSet;

        int dwordC;

        lvlInstance *Next;
    protected:
        static AGE_API int GetGeomSet(char const * a1, char const * a2, int a3)
                                                            { return ageHook::StaticThunk<0x4632C0>::Call<int>(a1, a2, a3); }
        static AGE_API void CreateTempBounds()              { ageHook::StaticThunk<0x464680>::Call<void>(); }
        static AGE_API void DeleteTempBounds()              { ageHook::StaticThunk<0x4647E0>::Call<void>(); }
    public:
        struct GeomTableEntry
        {
            modStatic *VeryLow;
            modStatic *Low;
            modStatic *Medium;
            modStatic *High;
            modShader **pShaders;
            phBoundGeometry *BoundGeom;
            float Radius;
            uint32_t dword1C;

            inline modStatic* getHighestLOD() const {
                if (High != nullptr)
                    return High;
                if (Medium != nullptr)
                    return Medium;
                if (Low != nullptr)
                    return Low;
                if (VeryLow != nullptr)
                    return VeryLow;
                return NULL;
            }

            inline modStatic* getLowestLOD() const {
                if (VeryLow != nullptr)
                    return VeryLow;
                if (Low != nullptr)
                    return Low;
                if (Medium != nullptr)
                    return Medium;
                if (High != nullptr)
                    return High;
                return NULL;
            }

            inline modStatic* getVeryLowLOD() const {
                return VeryLow;
            }

            inline modStatic* getLowLOD() const {
                return Low;
            }

            inline modStatic* getMedLOd() const {
                return Medium;
            }

            inline modStatic* getHighLOD() const {
                return High;
            }

            inline float getRadius() const {
                return Radius;
            }

            static void BindLua(LuaState L) {
                LuaBinding(L).beginClass<GeomTableEntry>("GeomTableEntry")
                    //fields
                    .addPropertyReadOnly("VL", &getVeryLowLOD)
                    .addPropertyReadOnly("L", &getLowLOD)
                    .addPropertyReadOnly("M", &getMedLOd)
                    .addPropertyReadOnly("H", &getHighLOD)
                    .addPropertyReadOnly("Radius", &getRadius)
                    .addFunction("GetHighestLOD", &getHighestLOD)
                    .addFunction("GetLowestLOD", &getLowestLOD)
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
    public:
        
        inline short getRoomId(void) const {
            return room;
        }

        inline short getGeomSetId(void) const {
            return GeomSet;
        }

        static AGE_API void ResetInstanceHeap()             { ageHook::StaticThunk<0x4631A0>::Call<void>(); }
        static AGE_API void ResetAll()                      { ageHook::StaticThunk<0x4631E0>::Call<void>(); }
        static AGE_API void SetShadowBillboardMtx(Matrix44 & a1)
                                                            { ageHook::StaticThunk<0x463290>::Call<void>(a1); }

        static AGE_API int AddSphere(float a1)              { return ageHook::StaticThunk<0x463D50>::Call<int>(a1); }

        AGE_API bool LoadBoundOnLastEntry(char const * a1)  { return ageHook::Thunk<0x463940>::Call<bool>(this, a1); }
        AGE_API Vector4 * GetBoundSphere(Vector4 * a1)      { return ageHook::Thunk<0x463A40>::Call<Vector4 *>(this, a1); }

        AGE_API bool BeginGeom(char const * a1, char const * a2, int a3)
                                                            { return ageHook::Thunk<0x463A80>::Call<bool>(this, a1, a2, a3); }
        AGE_API int AddGeom(char const * a1, char const * a2, int a3)
                                                            { return ageHook::Thunk<0x463BA0>::Call<int>(this, a1, a2, a3); }
        AGE_API void EndGeom()                              { ageHook::Thunk<0x463BC0>::Call<void>(this); }

        AGE_API bool InitBoundTerrain(char const * a1)      { return ageHook::Thunk<0x463DA0>::Call<bool>(this, a1); }
        AGE_API bool InitBoundTerrainLocal(char const * a1) { return ageHook::Thunk<0x463F50>::Call<bool>(this, a1); }
        AGE_API bool NeedGhostBound(Vector3 const * a1, int a2)
                                                            { return ageHook::Thunk<0x4641A0>::Call<bool>(this, a1, a2); }
        AGE_API bool InitGhostBound(phBound * a1, Vector3 const * a2, int a3)
                                                            { return ageHook::Thunk<0x464200>::Call<bool>(this, a1, a2, a3); }
        AGE_API int InitGhost(char const * a1, Matrix34 const & a2)
                                                            { return ageHook::Thunk<0x464330>::Call<int>(this, a1, a2); }

        AGE_API void PreLoadShader(int a1)                  { ageHook::Thunk<0x464B00>::Call<void>(this, a1); }
        AGE_API void Optimize(int a1)                       { ageHook::Thunk<0x464B70>::Call<void>(this, a1); }

        /*
            lvlInstance virtuals
        */

        virtual AGE_API void Reset()                        PURE;
        virtual AGE_API Vector3 const & GetPosition()       PURE;
        virtual AGE_API int IsVisible(gfxViewport const * a1)
                                                            { return ageHook::Thunk<0x4649F0>::Call<int>(this, a1); }

        virtual AGE_API Matrix34 const & GetMatrix(Matrix34* a1)
                                                            PURE;
        virtual AGE_API void SetMatrix(const Matrix34* a1)  PURE;
        
        virtual AGE_API void SetVariant(int a1)             { ageHook::Thunk<0x4643D0>::Call<void>(this, a1); }
        virtual AGE_API float const GetRadius()             { return ageHook::Thunk<0x4643E0>::Call<float>(this); }
        virtual AGE_API dgPhysEntity * GetEntity()          { return ageHook::Thunk<0x4643B0>::Call<dgPhysEntity *>(this); }
        virtual AGE_API dgPhysEntity * AttachEntity()       { return ageHook::Thunk<0x4643C0>::Call<dgPhysEntity *>(this); }
        virtual AGE_API Vector3 const * GetVelocity()       { return ageHook::Thunk<0x4643A0>::Call<Vector3 const *>(this); }
        virtual AGE_API void Detach()                       { ageHook::Thunk<0x43FC30>::Call<void>(this); }
        
        virtual AGE_API void Draw(int)                      PURE;
        virtual AGE_API void DrawShadow()                   { ageHook::Thunk<0x4643F0>::Call<void>(this); }
        virtual AGE_API void DrawShadowMap()                { ageHook::Thunk<0x464400>::Call<void>(this); }
        virtual AGE_API void DrawGlow()                     { ageHook::Thunk<0x464410>::Call<void>(this); }
        virtual AGE_API void DrawReflected(float a1)        { ageHook::Thunk<0x464420>::Call<void>(this, a1); }
        virtual AGE_API void DrawReflectedParts(int a1)     { ageHook::Thunk<0x4648B0>::Call<void>(this, a1); }
        virtual AGE_API int Init(char const *a1, Matrix34 const &a2, int a3)
                                                            {return ageHook::Thunk<0x463D90>::Call<int>(this, a1, a2, a3); }

        virtual AGE_API unsigned int SizeOf(void)           PURE;

        virtual AGE_API bool IsLandmark()                   { return ageHook::Thunk<0x463180>::Call<bool>(this); }
        virtual AGE_API bool IsCollidable()                 { return ageHook::Thunk<0x43FC40>::Call<bool>(this); }
        virtual AGE_API bool IsTerrainCollidable()          { return ageHook::Thunk<0x43FC50>::Call<bool>(this); }
        virtual AGE_API int GetNumLightSources()            { return ageHook::Thunk<0x4632B0>::Call<int>(this); }
        virtual AGE_API void GetLightInfo(int a1, cltLight * a2)
                                                            { ageHook::Thunk<0x4630B0>::Call<void>(this); }
        virtual AGE_API int SetupGfxLights(Matrix34 const & a1)
                                                            { return ageHook::Thunk<0x464670>::Call<int>(this, a1); }
        virtual AGE_API phBound * GetBound(int a1)          { return ageHook::Thunk<0x4648C0>::Call<phBound *>(this, a1); };

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<lvlInstance>("lvlInstance")
                //fields
                .addPropertyReadOnly("CurrentRoom", &getRoomId)
                .addPropertyReadOnly("GeometrySetIndex", &getGeomSetId)

                //statics
                .addStaticFunction("GetGeomName", &GetGeomName)
                .addStaticFunction("GetGeomTableEntry", &GetGeomTableEntry)
                .addStaticProperty("GeomTableSize", &GetGeomSetCount)

                .addStaticFunction("ResetInstanceHeap", &ResetInstanceHeap)
                .addStaticFunction("ResetAll", &ResetAll)
                .addStaticFunction("SetShadowBillboardMtx", &SetShadowBillboardMtx)
                .addStaticFunction("GetGeomSet", &GetGeomSet)

                //functiolns
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
                .addFunction("GetMatrix", &GetMatrix)
                .addFunction("GetPosition", &GetPosition)
                .addFunction("SetMatrix", &SetMatrix)
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
                .addFunction("GetBound", &GetBound)
            .endClass();
        }
    };

    // Lua initialization

}