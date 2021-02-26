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
        bool isAttached; // still attached to the parent?
        int index; // index into the list of breakables
        Matrix34 matrix;
        lvlInstance::GeomTableEntry* model;
        int bangerDataIndex;
        int geometryIndex;
        lvlInstance* instance;
        vehBreakable *next;
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
        vehBreakable *first; // linked-list
        float maxImpact; // educated guess (default: 10000.0)
        Matrix34 *matrix; // origin?
        int variant; // always zero by default, used in Eject to set Banger variant

        // TODO: figure out what these do

        float p5; // (default: 4.0)
        float p6; // (default: 0.0)
        float p7; // (default: 2.0)
        float p8; // (default: 0.0)
    public:
        inline int getVariant() {
            return this->variant;
        }

        inline void setVariant(int variant) {
            this->variant = variant;
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


        AGE_API void Draw(const Matrix34* a1, modShader* a2, int a3)
        {
            for (auto i = this->first; i; i = i->next)
            {
                if (i->isAttached)
                {
                    auto lodModel = i->model->getLOD(a3);
                    if (lodModel != nullptr)
                    {
                        Matrix34 drawMatrix = Matrix34();
                        drawMatrix.Set(&i->matrix);
                        drawMatrix.Dot(a1);

                        //setup renderer
                        Matrix44::Convert(gfxRenderState::sm_World, &drawMatrix);
                        gfxRenderState::m_Touched = gfxRenderState::m_Touched | 0x88;

                        lodModel->Draw(a2);

                        //reflection
                        if (EnableReflections)
                        {
                            hook::Type<gfxTexture*> g_ReflectionMap = 0x628914;
                            bool isSoftware = *(bool*)0x6830D4;

                            auto state = &MMSTATE;
                            if (a3 == 3 && (g_ReflectionMap != nullptr && !isSoftware && state->EnableReflections))
                            {
                                modShader::BeginEnvMap(g_ReflectionMap, drawMatrix);
                                lodModel->DrawEnvMapped(a2, g_ReflectionMap, 1.f);
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