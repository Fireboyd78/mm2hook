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

        ushort room;
        ushort Flags;

        short GeomSet;

        int dwordC;

        lvlInstance *Next;
    protected:
        static AGE_API int GetGeomSet(char const * a1, char const * a2, int a3)
                                                            { return hook::StaticThunk<0x4632C0>::Call<int>(a1, a2, a3); }
        static AGE_API void CreateTempBounds()              { hook::StaticThunk<0x464680>::Call<void>(); }
        static AGE_API void DeleteTempBounds()              { hook::StaticThunk<0x4647E0>::Call<void>(); }
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

            inline modStatic * getHighestLOD() const {
                if (High != nullptr)
                    return High;
                if (Medium != nullptr)
                    return Medium;
                if (Low != nullptr)
                    return Low;
                if (VeryLow != nullptr)
                    return VeryLow;
                return nullptr;
            }

            inline modStatic * getLowestLOD() const {
                if (VeryLow != nullptr)
                    return VeryLow;
                if (Low != nullptr)
                    return Low;
                if (Medium != nullptr)
                    return Medium;
                if (High != nullptr)
                    return High;
                return nullptr;
            }

            inline modStatic * getVeryLowLOD() const {
                return VeryLow;
            }

            inline modStatic * getLowLOD() const {
                return Low;
            }

            inline modStatic * getMedLOD() const {
                return Medium;
            }

            inline modStatic * getHighLOD() const {
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
                    .addPropertyReadOnly("M", &getMedLOD)
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
        
        inline ushort getRoomId(void) const {
            return room;
        }

        inline short getGeomSetId(void) const {
            return GeomSet;
        }

        short getFlags() const {
            return this->Flags;
        }

        inline void setFlags(ushort flags) {
            this->Flags = flags;
        }

        inline void setFlag(ushort flag) {
            this->Flags |= flag;
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
            lvlInstance virtuals
        */

        virtual AGE_API void Reset()                        PURE;
        virtual AGE_API const Vector3 & GetPosition()       PURE;
        virtual AGE_API int IsVisible(const gfxViewport *a1)
                                                            { return hook::Thunk<0x4649F0>::Call<int>(this, a1); }

        virtual AGE_API const Matrix34 & GetMatrix(Matrix34 *a1)
                                                            PURE;
        virtual AGE_API void SetMatrix(const Matrix34 *a1)  PURE;
        
        virtual AGE_API void SetVariant(int a1)             { hook::Thunk<0x4643D0>::Call<void>(this, a1); }
        virtual AGE_API const float GetRadius()             { return hook::Thunk<0x4643E0>::Call<float>(this); }
        virtual AGE_API dgPhysEntity * GetEntity()          { return hook::Thunk<0x4643B0>::Call<dgPhysEntity *>(this); }
        virtual AGE_API dgPhysEntity * AttachEntity()       { return hook::Thunk<0x4643C0>::Call<dgPhysEntity *>(this); }
        virtual AGE_API const Vector3 * GetVelocity()       { return hook::Thunk<0x4643A0>::Call<const Vector3 *>(this); }
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