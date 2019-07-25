#pragma once
#include <modules\node.h>
//#include <mm2_game.h>
//#include <modules\vehicle.h>

namespace MM2
{
    // Forward declarations
    extern class camBaseCS;
    extern class camAppCS;
    extern class camCarCS;
    extern class camAICS;

    // External declarations
    extern class asNode;
    extern class vehCar;

    // Class definitions
    class camBaseCS : public asNode {
    private:
        byte buffer[128]; //unknown data size for now
    public:
        AGE_API camBaseCS(void) {
            PUSH_VTABLE();
            ageHook::Thunk<0x521D60>::Call<void>(this);
            POP_VTABLE();
        };

        virtual AGE_API ~camBaseCS(void) {
            PUSH_VTABLE();
            ageHook::Thunk<0x521DF0>::Call<void>(this);
            POP_VTABLE();
        };

        //fields
        inline Vector3* getPosition(void) const {
            return getPtr<Vector3>(this, 0x40);
        };

        inline Matrix34* getMatrix(void) const {
            return getPtr<Matrix34>(this, 0x1C);
        };

        inline float getFOV(void) {
             return *getPtr<float>(this, 0x88);
        }

        inline void setFOV(float fov) {
            auto fovPtr = getPtr<float>(this, 0x88);
            *fovPtr = fov;
        }

        //virtuals
        virtual AGE_API void MakeActive()                         { ageHook::Thunk<0x521520>::Call<void>(this); };
        virtual AGE_API void UpdateInput()                        { ageHook::Thunk<0x520410>::Call<void>(this); };
        virtual AGE_API void ForceMatrixDelta(const Vector3& a1)  { ageHook::Thunk<0x521E60>::Call<void>(this, a1); };
        virtual AGE_API void SetST(float* a1)                     { ageHook::Thunk<0x51D750>::Call<void>(this, a1); };

        //members
        AGE_API void UpdateView()                                 { ageHook::Thunk<0x521E30>::Call<void>(this); };

        //lua
        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<camBaseCS, asNode>("camBaseCS")
                //properties
                .addProperty("FOV", &getFOV, &setFOV)
                .addPropertyReadOnly("Position", &getPosition)
                .addPropertyReadOnly("Matrix", &getMatrix)

                //members
                .addFunction("UpdateView", &UpdateView)
                .endClass();
        }
    };

    class camAppCS : public camBaseCS
    {
    public:
        AGE_API camAppCS(void) {
            PUSH_VTABLE();
            ageHook::Thunk<0x522B50>::Call<void>(this);
            POP_VTABLE();
        };

        virtual AGE_API ~camAppCS(void) {
            PUSH_VTABLE();
            ageHook::Thunk<0x522050>::Call<void>(this);
            POP_VTABLE();
        };

        //overrides
        virtual AGE_API void FileIO(datParser& a1) override       { ageHook::Thunk<0x5229D0>::Call<void>(this, a1); };
    
        //lua
        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<camAppCS, camBaseCS>("camAppCS")
                .endClass();
        }

    protected:
        //members
        AGE_API void ApproachIt()                                 { ageHook::Thunk<0x522060>::Call<void>(this); };

    };

    class camCarCS : public camAppCS
    {
    public:
        AGE_API camCarCS(void) {
            PUSH_VTABLE();
            ageHook::Thunk<0x520A30>::Call<void>(this);
            POP_VTABLE();
        };

        virtual AGE_API ~camCarCS(void) {
            PUSH_VTABLE();
            ageHook::Thunk<0x406810>::Call<void>(this);
            POP_VTABLE();
        };

        //overrides
        virtual AGE_API void FileIO(datParser& a1) override  { ageHook::Thunk<0x5214E0>::Call<void>(this, a1); };

        //members
        AGE_API void Init(MM2::vehCar* car, char* name)      { ageHook::Thunk<0x5214A0>::Call<void>(this); };

        //lua
        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<camCarCS, camAppCS>("camCarCS")
                .endClass();
        }
    };

    class camPointCS : public camCarCS {
    public:
        AGE_API camPointCS(void) {
            PUSH_VTABLE();
            ageHook::Thunk<0x520610>::Call<void>(this);
            POP_VTABLE();
        };

        virtual AGE_API ~camPointCS(void) {
            PUSH_VTABLE();
            ageHook::Thunk<0x406820>::Call<void>(this);
            POP_VTABLE();
        };

        //asNode overrides
        AGE_API char* GetClassName() override           { return ageHook::Thunk<0x520A20>::Call<char*>(this); }
        AGE_API void Reset() override                   { ageHook::Thunk<0x520690>::Call<void>(this); }
        AGE_API void Update() override                  { ageHook::Thunk<0x520790>::Call<void>(this); }

        //camBaseCS overrides
        AGE_API void MakeActive() override              { ageHook::Thunk<0x5206A0>::Call<void>(this); }

        //members
        AGE_API void SetAppRate(float a1)               { ageHook::Thunk<0x520730>::Call<void>(this, a1); }
        AGE_API void SetMaxDist(float a1)               { ageHook::Thunk<0x5206F0>::Call<void>(this, a1); }
        AGE_API void SetMinDist(float a1)               { ageHook::Thunk<0x520710>::Call<void>(this, a1); }
        AGE_API void SetPos(Vector3* a1)                { ageHook::Thunk<0x5206B0>::Call<void>(this, a1); }
        AGE_API void SetVel(Vector3* a1)                { ageHook::Thunk<0x520740>::Call<void>(this, a1); }
        
        //lua
        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<camPointCS, camCarCS>("camPointCS")
                //properties
                .addProperty("Position", &getPosition, &SetPos)
                .addFunction("SetVel", &SetVel)
                .addFunction("SetAppRate", &SetAppRate)
                .addFunction("SetMaxDist", &SetMaxDist)
                .addFunction("SetMinDist", &SetMinDist)
                .endClass();
        }
    };

    class camPovCS : public camCarCS {
    private:
        //members
        AGE_API void UpdatePOV()                        { ageHook::Thunk<0x51D5A0>::Call<void>(this); }
    public:
        AGE_API camPovCS(void) {
            PUSH_VTABLE();
            ageHook::Thunk<0x51D410>::Call<void>(this);
            POP_VTABLE();
        };

        virtual AGE_API ~camPovCS(void) {
            PUSH_VTABLE();
            ageHook::Thunk<0x51D520>::Call<void>(this);
            POP_VTABLE();
        };

        //asNode overrides
        AGE_API void AfterLoad() override               { ageHook::Thunk<0x51D710>::Call<void>(this); }
        AGE_API void FileIO(datParser& a1) override     { ageHook::Thunk<0x51D690>::Call<void>(this, a1); }
        AGE_API char* GetClassName() override           { return ageHook::Thunk<0x51D760>::Call<char*>(this); }
        AGE_API void Reset() override                   { ageHook::Thunk<0x51D540>::Call<void>(this); }
        AGE_API void Update() override                  { ageHook::Thunk<0x51D570>::Call<void>(this); }

        //camBaseCS overrides
        AGE_API void MakeActive() override              { ageHook::Thunk<0x51D530>::Call<void>(this); }
        AGE_API void UpdateInput() override             { ageHook::Thunk<0x51D590>::Call<void>(this); }

        //lua
        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<camPovCS, camCarCS>("camPovCS")
                //properties
                .endClass();
        }
    };

    class camTrackCS : public camCarCS {
    private:
        //members
        AGE_API void Collide(Vector3 a1)                { ageHook::Thunk<0x51EED0>::Call<void>(this, a1); }
        AGE_API void Front(float a1)                    { ageHook::Thunk<0x51F980>::Call<void>(this, a1); }
        AGE_API void MinMax(Matrix34 a1)                { ageHook::Thunk<0x51ECC0>::Call<void>(this, a1); }
        AGE_API void PreApproach()                      { ageHook::Thunk<0x51EB40>::Call<void>(this); }
        AGE_API void Rear(float a1)                     { ageHook::Thunk<0x51FA00>::Call<void>(this, a1); }

        AGE_API void UpdateCar()                        { ageHook::Thunk<0x51DC70>::Call<void>(this); }
        AGE_API void UpdateHill()                       { ageHook::Thunk<0x51DEE0>::Call<void>(this); }
        AGE_API void UpdateTrack()                      { ageHook::Thunk<0x51E400>::Call<void>(this); }
    public:
        AGE_API camTrackCS(void) {
            PUSH_VTABLE();
            ageHook::Thunk<0x51D798>::Call<void>(this);
            POP_VTABLE();
        };

        virtual AGE_API ~camTrackCS(void) {
            PUSH_VTABLE();
            ageHook::Thunk<0x51DA80>::Call<void>(this);
            POP_VTABLE();
        };

        //asNode overrides
        AGE_API void AfterLoad() override               { ageHook::Thunk<0x51DAF0>::Call<void>(this); }
        AGE_API void FileIO(datParser& a1) override     { ageHook::Thunk<0x51FA80>::Call<void>(this, a1); }
        AGE_API char* GetClassName() override           { return ageHook::Thunk<0x51FD60>::Call<char*>(this); }
        AGE_API void Reset() override                   { ageHook::Thunk<0x51DB00>::Call<void>(this); }
        AGE_API void Update() override                  { ageHook::Thunk<0x51DB50>::Call<void>(this); }

        //camBaseCS overrides
        AGE_API void MakeActive() override              { ageHook::Thunk<0x51DAE0>::Call<void>(this); }
        AGE_API void UpdateInput() override             { ageHook::Thunk<0x51DC60>::Call<void>(this); }

        //members
        AGE_API void SwingToRear()                      { ageHook::Thunk<0x51F920>::Call<void>(this); }
        
        //lua
        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<camTrackCS, camCarCS>("camTrackCS")
                //properties
                .addFunction("SwingToRear", &SwingToRear)
                .endClass();
        }
    };

    class camAICS : public camCarCS {
    public:
        AGE_API camAICS(void) {
            PUSH_VTABLE();
            ageHook::Thunk<0x520A30>::Call<void>(this);
            POP_VTABLE();
        };

        virtual AGE_API ~camAICS(void) {
            PUSH_VTABLE();
            ageHook::Thunk<0x406810>::Call<void>(this);
            POP_VTABLE();
        };

        //fields
        static inline float getHeight(void) {
            return *(float*)0x5D4340;
        }

        static inline void setHeight(float height) {
            auto heightPtr = (float*)0x5D4340;
            *heightPtr = height;
        }

        //overrides
        AGE_API void Reset() override { ageHook::Thunk<0x520AB0>::Call<void>(this); }

        //members
        AGE_API void SetPosition(Vector3* a1)                     { ageHook::Thunk<0x520AC0>::Call<void>(this, a1); };
        AGE_API void SetMatrix(Matrix34* a1)                      { ageHook::Thunk<0x520AE1>::Call<void>(this, a1); };

        //lua
        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<camAICS, camCarCS>("camAICS")
                //properties
                .addStaticProperty("Height", &getHeight, &setHeight)
                .addProperty("Position", &getPosition, &SetPosition)
                .addProperty("Matrix", &getMatrix, &SetMatrix)
                .endClass();
        }
    };

    class camViewCS : public asNode {
    private:
        byte buffer[0x4C]; //matches operator new @0x51FE4C
    public:
        AGE_API camViewCS(void) {
            PUSH_VTABLE();
            ageHook::Thunk<0x51FD70>::Call<void>(this);
            POP_VTABLE();
        };

        virtual AGE_API ~camViewCS(void) {
            PUSH_VTABLE();
            ageHook::Thunk<0x51FE20>::Call<void>(this);
            POP_VTABLE();
        };

        //overrides
        AGE_API void Update() override                                             { ageHook::Thunk<0x51FFC0>::Call<void>(this); }
        AGE_API void Reset() override                                              { ageHook::Thunk<0x520010>::Call<void>(this); }
        AGE_API void FileIO(datParser& a1) override                                { ageHook::Thunk<0x5200D0>::Call<void>(this, a1); }

        //members
        AGE_API static camViewCS* Instance(vehCar* a1)                             { return ageHook::StaticThunk<0x51FE30>::Call<camViewCS*>(a1); }

        AGE_API void SetCam(camCarCS* newCamera)                                   { ageHook::Thunk<0x51FE90>::Call<void>(this, newCamera); }
        AGE_API void OneShot()                                                     { ageHook::Thunk<0x5200C0>::Call<void>(this); }
        AGE_API bool NewCam(camCarCS* newCamera, int a2, float a3, datCallback a4) { return ageHook::Thunk<0x51FF30>::Call<bool>(this, newCamera, a2, a3, a4); }
        AGE_API void Init(vehCar* a1)                                              { ageHook::Thunk<0x51FFF0>::Call<void>(this, a1); }

        AGE_API void ForceMatrixDelta(Vector3 const* a1)                           { ageHook::Thunk<0x5200A0>::Call<void>(this, a1); }
        AGE_API void ForceMatrixDelta(Matrix34 const* a1)                          { ageHook::Thunk<0x520080>::Call<void>(this, a1); }
        
        //helpers for lua calls
        bool NewCam(camCarCS* newCamera, int a2, float a3) {
            return NewCam(newCamera, a2, a3, datCallback::NullCallback);
        }

        //will try and retrieve the instance from the games current player
        //todo : move mmGameManager to something that can be include'd without
        //breaking the entire thing!
        /*static camViewCS* Instance() {
            auto gameInstance = mmGameManager::Instance.get()->getGame();
            if (gameInstance == nullptr)
                return nullptr;

            auto playerInstance = gameInstance->getPlayer();
            if (playerInstance == nullptr)
                return nullptr;

            auto playerCar = playerInstance->getCar();
            if (playerCar == nullptr) {
                return nullptr;
            }else{
                return Instance(playerCar);
            }
        }*/

        //lua
        //.addFunction("RemoveChild", static_cast<int(asNode::*)(asNode* child)>(&RemoveChild))

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<camViewCS, asNode>("camViewCS")
                //properties
                //.addStaticFunction("Instance", &Instance)
                .addFunction("SetCam", &SetCam)
                .addFunction("NewCam", static_cast<bool(camViewCS::*)(camCarCS*, int, float)>(&NewCam))
                .addFunction("Init", &Init)
                .addFunction("ForceMatrixDelta", static_cast<void(camViewCS::*)(Matrix34 const*)>(&ForceMatrixDelta))
                .endClass();
        }
    };

    // Lua initialization
    template<>
    void luaAddModule<module_camera>(LuaState L) {
        //cam base classes
        luaBind<camBaseCS>(L);
        luaBind<camAppCS>(L);
        luaBind<camCarCS>(L);

        //camera types
        luaBind<camPointCS>(L);
        luaBind<camPovCS>(L);
        luaBind<camAICS>(L);
        luaBind<camTrackCS>(L);

        //view
        luaBind<camViewCS>(L);
    }

}