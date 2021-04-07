#pragma once
#include <modules\node.h>
//#include <mm2_game.h>
//#include <modules\vehicle.h>

namespace MM2
{
    // Forward declarations
    class camBaseCS;
    class camAppCS;
    class camCarCS;
    class camPointCS;
    class camPovCS;
    class camTrackCS;
    class camAICS;
    class camViewCS;
    class asCamera;

    // External declarations
    extern class asNode;
    extern class vehCar;

    // Class definitions
    class asCamera : public asNode {
    private:
        byte _buffer[0x158];
    public:
        AGE_API asCamera(void) {
            scoped_vtable x(this);
            hook::Thunk<0x4A2340>::Call<void>(this);
        };

        virtual AGE_API ~asCamera(void) {
            scoped_vtable x(this);
            hook::Thunk<0x4A22E0>::Call<void>(this);
        };
    };
    
    ASSERT_SIZEOF(asCamera, 0x170);

    class camBaseCS : public asNode {
    protected:
        uint16_t unk_18; // flags?
        uint16_t unk_1A; // unused/padding ?

        Matrix34 matrix_1;
        Matrix34 matrix_2;

        camViewCS *view; // never assigned to? used by IsViewCSInTransition, but result is discarded (see: camPostCS::Update)

        float BlendTime;
        float BlendGoal;
        float CameraFOV;
        float CameraNear;
    public:
        AGE_API camBaseCS(void) {
            scoped_vtable x(this);
            hook::Thunk<0x521D60>::Call<void>(this);
        }

        virtual AGE_API ~camBaseCS(void) {
            scoped_vtable x(this);
            hook::Thunk<0x521DF0>::Call<void>(this);
        }

        //fields
        inline Matrix34 * getMatrix(void)                   { return &matrix_1; }
        inline Vector3 * getPosition(void)                  { return reinterpret_cast<Vector3 *>(&matrix_1.m30); }

        inline float getFOV(void)                           { return CameraFOV; }
        inline void setFOV(float fov)                       { CameraFOV = fov; }

        //virtuals
        virtual AGE_API void MakeActive()                   { hook::Thunk<0x521520>::Call<void>(this); }
        virtual AGE_API void UpdateInput()                  { hook::Thunk<0x520410>::Call<void>(this); }
        virtual AGE_API void ForceMatrixDelta(const Vector3 &a1)
                                                            { hook::Thunk<0x521E60>::Call<void>(this, &a1); }
        virtual AGE_API void SetST(float *a1)               { hook::Thunk<0x51D750>::Call<void>(this, a1); }

        //members
        AGE_API void UpdateView()                           { hook::Thunk<0x521E30>::Call<void>(this); }

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

    ASSERT_SIZEOF(camBaseCS, 0x90);

    class camAppCS : public camBaseCS {
    protected:
        Matrix34 *unk_90; // parent?

        Vector3 TrackTo;

        BOOL ApproachOn;
        BOOL AppAppOn;

        float AppRot;
        float AppXRot;
        float AppYPos;
        float AppXZPos;
        float AppApp;
        float AppRotMin;
        float AppPosMin;
        float LookAbove;

        void *unk_C8;

        float MaxDist;
        float MinDist;
        float LookAt;

        // approach stuff?
        Matrix34 unk_D8;
    protected:
        AGE_API void ApproachIt()                           { hook::Thunk<0x522060>::Call<void>(this); }

    public:
        AGE_API camAppCS(void) {
            scoped_vtable x(this);
            hook::Thunk<0x521F70>::Call<void>(this);
        }

        virtual AGE_API ~camAppCS(void) {
            scoped_vtable x(this);
            hook::Thunk<0x522050>::Call<void>(this);
        }

        //overrides
        AGE_API void FileIO(datParser &parser) override     { hook::Thunk<0x5229D0>::Call<void>(this, &parser); }
    
        //lua
        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<camAppCS, camBaseCS>("camAppCS")
                // TODO
            .endClass();
        }
    };

    ASSERT_SIZEOF(camAppCS, 0x108);

    class camCarCS : public camAppCS {
    protected:
        vehCar *car;
        int unk_10C; // one of: -1, 0, 1?
    public:
        inline vehCar * getCar(void) const {
            return this->car;
        }

        AGE_API camCarCS(void) {
            scoped_vtable x(this);
            hook::Thunk<0x521470>::Call<void>(this);
        }

        virtual AGE_API ~camCarCS(void) {
            scoped_vtable x(this);
            hook::Thunk<0x521490>::Call<void>(this);
        }

        AGE_API void Init(vehCar *car, char *name)          { hook::Thunk<0x5214A0>::Call<void>(this, car, name); }

        //overrides
        AGE_API void FileIO(datParser &parser) override     { hook::Thunk<0x5214E0>::Call<void>(this, &parser); }

        //lua
        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<camCarCS, camAppCS>("camCarCS")
                // TODO
            .endClass();
        }
    };

    ASSERT_SIZEOF(camCarCS, 0x110);

    class camPointCS : public camCarCS {
    protected:
        Vector3 position;
        Vector3 velocity;

        float unk_128; // maxDist * 2?
        float maxDist;
        float appRate;
        float minDist;
        float unk_138; // minDist * 2?
    public:
        AGE_API camPointCS(void) {
            scoped_vtable x(this);
            hook::Thunk<0x520610>::Call<void>(this);
        }

        virtual AGE_API ~camPointCS(void) {
            scoped_vtable x(this);
            hook::Thunk<0x406820>::Call<void>(this);
        }

        AGE_API void SetAppRate(float rate)                 { hook::Thunk<0x520730>::Call<void>(this, rate); }
        AGE_API void SetMaxDist(float dist)                 { hook::Thunk<0x5206F0>::Call<void>(this, dist); }
        AGE_API void SetMinDist(float dist)                 { hook::Thunk<0x520710>::Call<void>(this, dist); }
        AGE_API void SetPos(Vector3 *pos)                   { hook::Thunk<0x5206B0>::Call<void>(this, pos); }
        AGE_API void SetVel(Vector3 *vel)                   { hook::Thunk<0x520740>::Call<void>(this, vel); }

        //camBaseCS overrides
        AGE_API void MakeActive() override                  { hook::Thunk<0x5206A0>::Call<void>(this); }

        //asNode overrides
        AGE_API void Reset() override                       { hook::Thunk<0x520690>::Call<void>(this); }
        AGE_API void Update() override                      { hook::Thunk<0x520790>::Call<void>(this); }
        AGE_API char * GetClassName() override              { return hook::Thunk<0x520A20>::Call<char*>(this); }

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

    ASSERT_SIZEOF(camPointCS, 0x13C);

    class camPovCS : public camCarCS {
    protected:
        int unk_110;

        Vector3 Offset;
        Vector3 ReverseOffset;
        float POVJitterAmp;
        
        Vector3 unk_130;

        float unk_13C; // related to camCarCS::unk_10C: if -1, use PI; otherwise, use this value (see: camPOVCS::Update @ 0x51D62A)
        float Pitch;
        float unk_144; // yaw? (roates matrix if != 0)
    private:
        AGE_API void UpdatePOV()                            { hook::Thunk<0x51D5A0>::Call<void>(this); }
    public:
        AGE_API camPovCS(void) {
            scoped_vtable x(this);
            hook::Thunk<0x51D410>::Call<void>(this);
        }

        virtual AGE_API ~camPovCS(void) {
            scoped_vtable x(this);
            hook::Thunk<0x51D520>::Call<void>(this);
        }

        //camBaseCS overrides
        AGE_API void MakeActive() override                  { hook::Thunk<0x51D530>::Call<void>(this); }
        AGE_API void UpdateInput() override                 { hook::Thunk<0x51D590>::Call<void>(this); }

        //asNode overrides
        AGE_API void Reset() override                       { hook::Thunk<0x51D540>::Call<void>(this); }
        AGE_API void Update() override                      { hook::Thunk<0x51D570>::Call<void>(this); }
        AGE_API void FileIO(datParser &parser) override     { hook::Thunk<0x51D690>::Call<void>(this, &parser); }
        AGE_API void AfterLoad() override                   { hook::Thunk<0x51D710>::Call<void>(this); }
        AGE_API char * GetClassName() override              { return hook::Thunk<0x51D760>::Call<char*>(this); }

        //lua
        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<camPovCS, camCarCS>("camPovCS")
                //properties
            .endClass();
        }
    };

    ASSERT_SIZEOF(camPovCS, 0x148);

    class camTrackCS : public camCarCS {
        AGE_API void Collide(Vector3 a1)                    { hook::Thunk<0x51EED0>::Call<void>(this, a1); }
        AGE_API void Front(float a1)                        { hook::Thunk<0x51F980>::Call<void>(this, a1); }
        AGE_API void MinMax(Matrix34 a1)                    { hook::Thunk<0x51ECC0>::Call<void>(this, a1); }
        AGE_API void PreApproach()                          { hook::Thunk<0x51EB40>::Call<void>(this); }
        AGE_API void Rear(float a1)                         { hook::Thunk<0x51FA00>::Call<void>(this, a1); }

        AGE_API void UpdateCar()                            { hook::Thunk<0x51DC70>::Call<void>(this); }
        AGE_API void UpdateHill()                           { hook::Thunk<0x51DEE0>::Call<void>(this); }
        AGE_API void UpdateTrack()                          { hook::Thunk<0x51E400>::Call<void>(this); }
    public:
        AGE_API camTrackCS(void) {
            scoped_vtable x(this);
            hook::Thunk<0x51D770>::Call<void>(this);
        }

        virtual AGE_API ~camTrackCS(void) {
            scoped_vtable x(this);
            hook::Thunk<0x51DA80>::Call<void>(this);
        }

        AGE_API void SwingToRear()                          { hook::Thunk<0x51F920>::Call<void>(this); }

        //asNode overrides
        AGE_API void AfterLoad() override                   { hook::Thunk<0x51DAF0>::Call<void>(this); }
        AGE_API void FileIO(datParser &parser) override     { hook::Thunk<0x51FA80>::Call<void>(this, &parser); }
        AGE_API char * GetClassName() override              { return hook::Thunk<0x51FD60>::Call<char*>(this); }
        AGE_API void Reset() override                       { hook::Thunk<0x51DB00>::Call<void>(this); }
        AGE_API void Update() override                      { hook::Thunk<0x51DB50>::Call<void>(this); }

        //camBaseCS overrides
        AGE_API void MakeActive() override                  { hook::Thunk<0x51DAE0>::Call<void>(this); }
        AGE_API void UpdateInput() override                 { hook::Thunk<0x51DC60>::Call<void>(this); }

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
            scoped_vtable x(this);
            hook::Thunk<0x520A30>::Call<void>(this);
        }

        virtual AGE_API ~camAICS(void) {
            scoped_vtable x(this);
            hook::Thunk<0x406810>::Call<void>(this);
        }

        static inline float getHeight(void) {
            return *(float*)0x5D4340;
        }

        static inline void setHeight(float height) {
            auto heightPtr = (float*)0x5D4340;
            *heightPtr = height;
        }

        AGE_API void SetPosition(Vector3 *a1)               { hook::Thunk<0x520AC0>::Call<void>(this, a1); }
        AGE_API void SetMatrix(Matrix34 *a1)                { hook::Thunk<0x520AE0>::Call<void>(this, a1); }

        // asNode overrides
        AGE_API void Reset() override                       { hook::Thunk<0x520AB0>::Call<void>(this); }

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
    protected:
        int unk_18;
        int unk_1C;
        int unk_20;

        datCallback callback;

        camCarCS *carCS;
        void *unk_34;
        void *transitionCS; // type: camTransitionCS *

        // overrides?
        bool unk_3C; // use values below?

        float unk_40; // camera far? (4th param in gfxViewport::Perspective)
        float unk_44; // camera near? (3rd param in gfxViewport::Perspective)
        int unk_48; // no idea; completely unused?
    public:
        AGE_API camViewCS(void) {
            scoped_vtable x(this);
            hook::Thunk<0x51FD70>::Call<void>(this);
        }

        virtual AGE_API ~camViewCS(void) {
            scoped_vtable x(this);
            hook::Thunk<0x51FE20>::Call<void>(this);
        }

        inline camCarCS * getCurrentCamera(void) const      { return carCS; }
        bool isCurrentCamera(camCarCS *cam)                 { return carCS == cam; }

        AGE_API static camViewCS * Instance(vehCar *a1)     { return hook::StaticThunk<0x51FE30>::Call<camViewCS *>(a1); }

        AGE_API void SetCam(camCarCS *cam)                  { hook::Thunk<0x51FE90>::Call<void>(this, cam); }
        AGE_API void OneShot()                              { hook::Thunk<0x5200C0>::Call<void>(this); }

        AGE_API bool NewCam(camCarCS *cam, int a2, float a3, datCallback a4)
                                                            { return hook::Thunk<0x51FF30>::Call<bool>(this, cam, a2, a3, a4); }

        AGE_API void Init(vehCar *car)                      { hook::Thunk<0x51FFF0>::Call<void>(this, car); }

        AGE_API void ForceMatrixDelta(const Vector3 *a1)    { hook::Thunk<0x5200A0>::Call<void>(this, a1); }
        AGE_API void ForceMatrixDelta(const Matrix34 *a1)   { hook::Thunk<0x520080>::Call<void>(this, a1); }

        //overrides
        AGE_API void Update() override                      { hook::Thunk<0x51FFC0>::Call<void>(this); }
        AGE_API void Reset() override                       { hook::Thunk<0x520010>::Call<void>(this); }
        AGE_API void FileIO(datParser &parser) override     { hook::Thunk<0x5200D0>::Call<void>(this, &parser); }

        //helpers for lua calls
        bool NewCam(camCarCS *cam, int a2, float a3) {
            return NewCam(cam, a2, a3, datCallback::NullCallback);
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
                .addStaticFunction("Instance", &Instance)
                .addPropertyReadOnly("CurrentCamera", &getCurrentCamera)
                .addFunction("IsCurrentCamera", &isCurrentCamera)
                .addFunction("SetCam", &SetCam)
                .addFunction("NewCam", static_cast<bool(camViewCS::*)(camCarCS *, int, float)>(&NewCam))
                .addFunction("Init", &Init)
                .addFunction("ForceMatrixDelta", static_cast<void(camViewCS::*)(const Matrix34 *)>(&ForceMatrixDelta))
            .endClass();
        }
    };

    ASSERT_SIZEOF(camViewCS, 0x4C); //matches operator new @0x51FE4C

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