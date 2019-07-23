#pragma once
#include <modules\node.h>
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

        //members
        AGE_API void Init(MM2::vehCar* car, char* name)                { ageHook::Thunk<0x5214A0>::Call<void>(this); };

        //overrides
        virtual AGE_API void FileIO(datParser& a1) override       { ageHook::Thunk<0x5214E0>::Call<void>(this, a1); };
    
        //lua
        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<camCarCS, camAppCS>("camCarCS")
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

        //members
        AGE_API void SetPosition(Vector3* a1)                     { ageHook::Thunk<0x520AC0>::Call<void>(this, a1); };
        AGE_API void SetMatrix(Matrix34* a1)                      { ageHook::Thunk<0x520AE1>::Call<void>(this, a1); };


        //overrides
        AGE_API void Reset() override                             { ageHook::Thunk<0x520AB0>::Call<void>(this); }

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

    // Lua initialization
    template<>
    void luaAddModule<module_camera>(LuaState L) {
        luaBind<camBaseCS>(L);
        luaBind<camAppCS>(L);
        luaBind<camCarCS>(L);
        luaBind<camAICS>(L);
    }

}