#pragma once
#include <modules\city.h>
#include <modules\level\sky.h>
#include <modules\level\inst.h>

namespace MM2
{
    // Forward declarations


    // External declarations
    extern class lvlInstance;

    // Class definitions
    class cityLevel : public lvlLevel {
    private:
        int unk28;
    protected:
        //helpers
    public:
        static hook::Type<lvlSky> Sky;
        static hook::Type<int> timeOfDay;

        AGE_API cityLevel() {
            scoped_vtable x(this);
            hook::Thunk<0x443860>::Call<void>(this);
        };

        virtual AGE_API ~cityLevel() {
            scoped_vtable x(this);
            hook::Thunk<0x443910>::Call<void>(this);
        };

        /*
            lvlLevel virtuals
        */

        AGE_API virtual void Load(char const* a1) override          { hook::Thunk<0x443F30>::Call<void>(this, a1); }
        AGE_API virtual void Update() override                      { hook::Thunk<0x4452B0>::Call<void>(this); }
        AGE_API virtual void PreDraw() override                     { hook::Thunk<0x4452E0>::Call<void>(this); }
        AGE_API virtual void PostDraw() override                    { hook::Thunk<0x445300>::Call<void>(this); }
        AGE_API virtual void Draw(const gfxViewport& a1, uint a2) override
                                                                    { hook::Thunk<0x445400>::Call<void>(this, a1, a2); }

        AGE_API virtual int FindRoomId(Vector3 const& a1, int a2) override
                                                                    { return hook::Thunk<0x446A60>::Call<int>(this, a1, a2); }
        AGE_API virtual int GetNeighborCount(int a1) override       { return hook::Thunk<0x446C20>::Call<int>(this, a1); }
        AGE_API virtual int GetNeighbors(int* a1, int a2) override  { return hook::Thunk<0x446C40>::Call<int>(this, a1, a2); }
        AGE_API virtual int GetTouchedNeighbors(int* a1, int a2, int a3, const Vector4& a4) override
                                                                    { return hook::Thunk<0x446CD0>::Call<int>(this, a1, a2, a3, a4); }
        AGE_API virtual int GetRoomPerimeter(int roomId, Vector3* out, int outSize) override
                                                                    { return hook::Thunk<0x446FA0>::Call<int>(this, roomId, out, outSize); }
        AGE_API virtual int GetVisitList(int* a1, int a2, Vector3 const& a3, Vector3 const& a4, int a5, int a6) override
                                                                    { return hook::Thunk<0x447030>::Call<int>(this, a1, a2, a3, a4, a5, a6); }
        AGE_API virtual bool Collide(int a1, class lvlSegment& a2, class lvlIntersection& a3) override
                                                                    { return false; }
        AGE_API virtual bool GetBoundSphere(Vector4& a1, int a2) override
                                                                    { return hook::Thunk<0x445310>::Call<bool>(this, a1, a2); }
        AGE_API virtual const class lvlLevelBound* GetBound() override      
                                                                    { return hook::Thunk<0x443930>::Call<class lvlLevelBound*>(this); }
        AGE_API virtual void SetObjectDetail(int a1) override       { hook::Thunk<0x443E50>::Call<void>(this, a1); }
        AGE_API virtual float GetWaterLevel(int a1)  override       { return hook::Thunk<0x445280>::Call<float>(this, a1); }
        AGE_API virtual float GetLightingIntensity(Vector3 const& a1) override
                                                                    { return hook::Thunk<0x445290>::Call<float>(this, a1); }
        AGE_API virtual void SetPtxHeight(asParticles& a1) override { hook::Thunk<0x4452A0>::Call<void>(this, a1); }

        AGE_API virtual gfxTexture* GetEnvMap(int a1, Vector3 const& a2, float* a3) override
                                                                    { return hook::Thunk<0x443940>::Call<gfxTexture*>(this, a1, a2, a3); }


        /*
            cityLevel functions
        */
        AGE_API void EnablePVS(bool enable) {
            hook::Thunk<0x4472A0>::Call<void>(this, enable);
        }

        AGE_API void EnableSky(bool enable) {
            hook::Thunk<0x447290>::Call<void>(this, enable);
        }

        AGE_API static void LoadPath(const char* a1, const Matrix34& a2, bool a3) {
            hook::StaticThunk<0x4451E0>::Call<void>(a1, a2, a3);
        }

        AGE_API static void LoadPathSet(const char* a1, const char* a2) {
            hook::StaticThunk<0x445140>::Call<void>(a1, a2);
        }

        AGE_API static void LoadProp(int a1, const char* a2, const Matrix34& a3) {
            hook::StaticThunk<0x4450E0>::Call<void>(a1, a2, a3);
        }

        //should be protected but we use this in the hook
        AGE_API void DrawRooms(const gfxViewport* a1, uint a2, LPVOID a3, int a4) {
            hook::Thunk<0x445820>::Call<void>(this, a1, a2, a3, a4);
        }

        //lua
        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<cityLevel, lvlLevel>("cityLevel")
                //functions
                .addFunction("EnablePVS", &EnablePVS)
                .addFunction("EnableSky", &EnableSky)
                
                .addStaticFunction("LoadPath", &LoadPath)
                .addStaticFunction("LoadPathSet", &LoadPathSet)
                .addStaticFunction("LoadProp", &LoadProp)
                
                //singleton
                .addStaticProperty("Singleton", [] 
                { 
                    //MM2 only uses cityLevel so this works
                    auto level = lvlLevel::Singleton.get();
                    return reinterpret_cast<cityLevel*>(level);
                })

                //sky singleton
                .addStaticProperty("Sky", [] { return &Sky; })
                .endClass();
        }
    };

    // Lua initialization

}