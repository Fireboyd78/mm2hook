#pragma once
#include <modules\level.h>
#include <handlers\lua_drawable.h>
#include <..\mm2_particle.h>

namespace MM2
{
    // Forward declarations
    class lvlLevel;

    // External declarations
    extern class asParticles;
    extern class gfxViewport;

    // Class definitions

    class lvlLevel : public asCullable {
    private:
        //lua drawables!
        inline void RegisterLuaDrawable(LuaRef self, LuaRef function, int phase = 1) {
            luaDrawableHandler::RegisterCallback(self, function, phase);
        }

    public:
        static hook::Type<lvlLevel*> Singleton;

        AGE_API lvlLevel() {
            scoped_vtable x(this);
            hook::Thunk<0x4653A0>::Call<void>(this);
        };

        virtual AGE_API ~lvlLevel() {
            scoped_vtable x(this);
            hook::Thunk<0x465400>::Call<void>(this);
        };

        /* 
            lvlLevel virtuals
        */

        AGE_API virtual void Load(char const* a1)                   PURE;
        AGE_API virtual void Update()                               { hook::Thunk<0x465460>::Call<void>(this); }
        AGE_API virtual void PreDraw()                              PURE;
        AGE_API virtual void PostDraw()                             PURE;
        AGE_API virtual void Draw(const gfxViewport& a1, uint a2)   PURE;
                                                                    
        AGE_API virtual int FindRoomId(Vector3 const& a1, int a2)   PURE;
        AGE_API virtual int GetNeighborCount(int a1)                PURE;
        AGE_API virtual int GetNeighbors(int* a1, int a2)           PURE;
        AGE_API virtual int GetTouchedNeighbors(int* a1, int a2, int a3, const Vector4& a4)   
                                                                    PURE;
        AGE_API virtual int GetRoomPerimeter(int roomId, Vector3* out, int outSize)
                                                                    PURE;
        AGE_API virtual int GetVisitList(int* a1, int a2, Vector3 const& a3, Vector3 const& a4, int a5, int a6)
                                                                    { return 0; }
        AGE_API virtual bool Collide(int a1, class lvlSegment & a2, class lvlIntersection & a3)
                                                                    { return false; }
        AGE_API virtual bool GetBoundSphere(Vector4& a1, int a2)    { return false; }
        AGE_API virtual const class lvlLevelBound* GetBound()       PURE;
        AGE_API virtual void SetObjectDetail(int a1)                {}
        AGE_API virtual float GetWaterLevel(int a1)                 PURE;
        AGE_API virtual float GetLightingIntensity(Vector3 const& a1)
                                                                    PURE;
        AGE_API virtual void SetPtxHeight(asParticles& a1)          PURE;
        AGE_API virtual bool ClampToWorld(Vector3& a1)              { return false; }
        AGE_API virtual bool LoadInstances(const char* a1, const char* a2)
                                                                    { return hook::Thunk<0x656F0>::Call<bool>(this, a1, a2); }
        AGE_API virtual gfxTexture* GetEnvMap(int a1, Vector3 const& a2, float* a3)
                                                                    { return nullptr; }
                                                                    

        /*
            lvlLevel functions
        */
        AGE_API void MoveToRoom(lvlInstance *instance, int room) {
            hook::Thunk<0x465480>::Call<void>(this, instance, room);
        }

        AGE_API void ResetInstances() {
            hook::Thunk<0x465E50>::Call<void>(this);
        }

        //lua
        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<lvlLevel, asCullable>("lvlLevel")
                //virtual functions
                .addFunction("FindRoomId", &FindRoomId)
                .addFunction("GetNeighborCount", &GetNeighborCount)
                .addFunction("SetObjectDetail", &SetObjectDetail)
                .addFunction("GetWaterLevel", &GetWaterLevel)
                .addFunction("GetLightingIntensity", &GetLightingIntensity)
                .addFunction("SetPtxHeight", &SetPtxHeight)

                //functions
                .addFunction("MoveToRoom", &MoveToRoom)
                .addFunction("ResetInstances", &ResetInstances)

                //drawable
                .addFunction("RegisterDrawable", &RegisterLuaDrawable, 
                    LUA_ARGS(LuaRef, LuaRef, _opt<int>)) //register a LuaCallback

                //singleton
                .addStaticProperty("Singleton", [] { return Singleton.get(); })
                .endClass();
        }
    };

    // Lua initialization

}