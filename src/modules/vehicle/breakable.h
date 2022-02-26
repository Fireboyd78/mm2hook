#pragma once
#include <modules\vehicle.h>

namespace MM2
{
    // Forward declarations
    class vehBreakable;
    class vehBreakableMgr;

    // External declarations
    extern class modStatic;
    
    class vehBreakable {
    public:
        bool IsAttached; // still attached to the parent?
        int Index; // index into the list of breakables
        Matrix34 Matrix;
        lvlInstance::GeomTableEntry* Model;
        int BangerDataIndex;
        int GeometryIndex;
        lvlInstance* Instance;
        vehBreakable *Next;
    public:
        AGE_API vehBreakable(modStatic **a1, Matrix34 *a2, int a3, int a4, int a5)
                                                            { hook::Thunk<0x4D8530>::Call<void>(this); }
        
        AGE_API void Add(vehBreakable *breakable)           { hook::Thunk<0x4D8BE0>::Call<void>(this, breakable); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<vehBreakable>("vehBreakable")
                .addFunction("Add", &Add)
            .endClass();
        }
    };

    class vehBreakableMgr {
    public:
        static bool EnableReflections;
    private:
        vehBreakable *First; // linked-list
        float ImpactThreshold; // educated guess (default: 10000.0)
        Matrix34 *Matrix; // origin?
        int Variant; // always zero by default, used in Eject to set Banger variant

        // TODO: figure out what these do

        float field_10; // (default: 4.0)
        float field_14; // (default: 0.0)
        float field_18; // (default: 2.0)
        float field_1C; // (default: 0.0)
    public:
        inline int getVariant() {
            return this->Variant;
        }

        inline void setVariant(int variant) {
            this->Variant = variant;
        }

        inline float getImpactThreshold() {
            return this->ImpactThreshold;
        }

        inline void setImpactThreshold(float threshold) {
            this->ImpactThreshold = threshold;
        }

        inline float getField_10() {
            return this->field_10;
        }

        inline void setField_10(float a1) {
            this->field_10 = a1;
        }

        inline float getField_18() {
            return this->field_18;
        }

        inline void setField_18(float a1) {
            this->field_18 = a1;
        }

    public:
        ANGEL_ALLOCATOR

        AGE_API vehBreakableMgr()                           { hook::Thunk<0x4D8580>::Call<void>(this); }
        AGE_API ~vehBreakableMgr()                          { hook::Thunk<0x4D85B0>::Call<void>(this); }

        AGE_API bool Init(Matrix34* a1)                     { return hook::Thunk<0x4D85E0>::Call<bool>(this, a1); }
        AGE_API void Add(LPCSTR a1, LPCSTR a2, modStatic **a3, int a4, int a5)     
                                                            { hook::Thunk<0x4D86A0 >::Call<void>(this, a1, a2, a3, a4, a5); }
        AGE_API void Create(LPCSTR a1, LPCSTR a2, modStatic **a3, int a4, int a5)
                                                            { hook::Thunk<0x4D8600 >::Call<void>(this, a1, a2, a3, a4, a5); }
        AGE_API void Eject(vehBreakable *a1, int room)      { hook::Thunk<0x4D8940>::Call<void>(this, a1, room); }
        AGE_API void EjectAll(int room)                     { hook::Thunk<0x4D8BB0>::Call<void>(this, room); }
        AGE_API void Reset()                                { hook::Thunk<0x4D8710>::Call<void>(this); }

        AGE_API void Draw(const Matrix34& matrix, modShader* shaders, int lod)
        {
            for (auto i = this->First; i; i = i->Next)
            {
                if (i->IsAttached)
                {
                    auto model = i->Model->GetLOD(lod);
                    if (model != nullptr)
                    {
                        Matrix34 drawMatrix = Matrix34();
                        drawMatrix.Set(i->Matrix);
                        drawMatrix.Dot(matrix);

                        //setup renderer
                        Matrix44::Convert(gfxRenderState::sm_World, drawMatrix);
                        gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x88;

                        model->Draw(shaders);

                        //reflection
                        if (EnableReflections)
                        {
                            hook::Type<gfxTexture*> g_ReflectionMap = 0x628914;
                            bool isSoftware = *(bool*)0x6830D4;

                            auto level = *lvlLevel::Singleton;
                            Vector3 carPosition = Vector3(matrix.m30, matrix.m31, matrix.m32);

                            auto state = &MMSTATE;
                            if (lod == 3 && (g_ReflectionMap != nullptr && !isSoftware && state->EnableReflections) &&
                                !(level->GetRoomInfo(level->FindRoomId(carPosition, 0))->Flags & static_cast<int>(RoomFlags::Subterranean)))
                            {
                                modShader::BeginEnvMap(g_ReflectionMap, drawMatrix);
                                model->DrawEnvMapped(shaders, g_ReflectionMap, 1.f);
                                modShader::EndEnvMap();
                            }
                        }
                    }
                }
            }
        }

        AGE_API vehBreakable * Get(int index)               { return hook::Thunk<0x4D86F0>::Call<vehBreakable *>(this, index); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<vehBreakableMgr>("vehBreakableMgr")
                .addConstructor(LUA_ARGS())
                .addFunction("Add", &Add)
                .addFunction("Create", &Create)
                .addFunction("Eject", &Eject)
                .addFunction("EjectAll", &EjectAll)
                .addFunction("Get", &Get)
            .endClass();
        }
    };

    ASSERT_SIZEOF(vehBreakable, 0x4C);
    ASSERT_SIZEOF(vehBreakableMgr, 0x20);

    // Lua initialization

}