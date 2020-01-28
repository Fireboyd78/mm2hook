#pragma once
#include <modules\node.h>

namespace MM2
{
    // Forward declarations
    class asCullManager;

    // External declarations
    extern class asCullable;
    extern class Matrix34;
    extern class gfxBitmap;

    // Class definitions
    class asCullManager : public asNode {
        void *unk_18; // unused

        // 0x1C - 0x5C
        int NumCameras;
        struct asCamera *Cameras[16];

        void *unk_60; // unused

        // 0x64 - 0x78
        int NumCullables,       MaxCullables;
        int NumCullables2D,     MaxCullables2D;
        int NumCullables2DFG,   MaxCullables2DFG;

        // 0x7C - 0x84
        asCullable **Cullables;
        asCullable **Cullables2D;
        asCullable **Cullables2DFG;

        Matrix34 **CullTransforms;  // for each Cullable, store the state of asLinearCS::CurrentMatrix;
                                    // restore the state prior to calling Cull method

        unsigned int ClearColor;

        //
        // unused -- debugging stuff?
        //

        void *unk_90, *unk_94;
        Timer unk_98, unk_9c;

        void *unk_a0;

        bool ResetOnUpdate; // resets all camera/cullable counts to zero upon next call to Update (one-time)
    private:
        //lua helper
        static asCullManager* getInstance() {
            return asCullManager::Instance;
        }
    public:
        static ageHook::Type<asCullManager *> Instance;

        asCullManager(void)                                 DONOTCALL;
        asCullManager(const asCullManager &&)               DONOTCALL;

        virtual ~asCullManager(void) {
            ageHook::Thunk<0x4A1370>::Call<void>(this);
        }
        
        /*
            asNode virtuals
        */
        void Reset() override                               { ageHook::Thunk<0x4A13F0>::Call<void>(this); }
        void Update() override                              { ageHook::Thunk<0x4A1450>::Call<void>(this); }

        /*
            asCullManager virtuals
        */
        virtual AGE_API void DeclareCamera(struct asCamera *camera)
                                                            { ageHook::Thunk<0x4A1580>::Call<void>(this, camera); }

        virtual AGE_API void DeclareCullable(asCullable *cullable)
                                                            { ageHook::Thunk<0x4A15B0>::Call<void>(this, cullable); }
        virtual AGE_API void DeclareCullable2D(asCullable *cullable)
                                                            { ageHook::Thunk<0x4A1600>::Call<void>(this, cullable); }
        virtual AGE_API void DeclareCullable2DFG(asCullable *cullable)
                                                            { ageHook::Thunk<0x4A1640>::Call<void>(this, cullable); }

        /*
            asCullManager
        */
        AGE_API void DeclareBitmap(asCullable *cullable, gfxBitmap *bitmap)
                                                            { ageHook::Thunk<0x4A1680>::Call<void>(this, cullable, bitmap); }

        //lua
        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<asCullManager, asNode>("asCullManager")
                .addStaticProperty("Instance", &getInstance)

                .addFunction("DeclareCullable", &DeclareCullable)
                .addFunction("DeclareCullable2D", &DeclareCullable2D)
                .addFunction("DeclareCullable2DFG", &DeclareCullable2DFG)
                .endClass();
        }
    };

    ASSERT_SIZEOF(asCullManager, 0xA8);

    // Lua initialization

}