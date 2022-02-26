#pragma once
#include <modules\level\inst.h>

namespace MM2
{
    // Forward declarations
    class lvlFixedAny;
    class lvlFixedRotY;
    class lvlFixedMatrix;

    // External declarations
    extern class gfxViewport;

    // Class definitions
    class lvlFixedAny : public lvlInstance
    {
    public:
        AGE_API lvlFixedAny(void) {
            scoped_vtable x(this);
            hook::Thunk<0x467300>::Call<void>(this);
        }

        /*
            lvlInstance virtuals
        */

        virtual AGE_API void SetVariant(int a1) override    { hook::Thunk<0x467540>::Call<void>(this, a1); }
        
        virtual AGE_API void Draw(int a1) override          { hook::Thunk<0x467420>::Call<void>(this, a1); }
        virtual AGE_API void DrawShadow() override          { hook::Thunk<0x4677E0>::Call<void>(this); }
        virtual AGE_API void DrawShadowMap() override       { hook::Thunk<0x467740>::Call<void>(this); }
        virtual AGE_API void DrawReflectedParts(int a1) override
                                                            { hook::Thunk<0x467320>::Call<void>(this, a1); }
        virtual AGE_API int Init(const char* a1, const Matrix34& a2, int a3) override
                                                            { return hook::Thunk<0x467580>::Call<int>(this, a1, &a2, a3); }


        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<lvlFixedAny, lvlInstance>("lvlFixedAny")
                .endClass();
        }
    };
    ASSERT_SIZEOF(lvlFixedAny, 0x14);

    class lvlFixedRotY : public lvlFixedAny
    {
    private:
        Vector3 Position;
        Vector2 XAxis;
    public:
        AGE_API lvlFixedRotY(void) {
            scoped_vtable x(this);
            hook::Thunk<0x467910>::Call<void>(this);
        }

        /*
            lvlInstance virtuals
        */

        virtual AGE_API const Vector3 & GetPosition() override
                                                            { return hook::Thunk<0x467940>::Call<const Vector3 &>(this); }
        virtual AGE_API int IsVisible(const gfxViewport & a1)
                                                            { return hook::Thunk<0x467950>::Call<int>(this, &a1); }
        virtual AGE_API const Matrix34 & GetMatrix(Matrix34 *a1)
                                                            { return hook::Thunk<0x4679A0>::Call<const Matrix34 &>(this); }
        virtual AGE_API void SetMatrix(const Matrix34 & a1) { hook::Thunk<0x4679F0>::Call<void>(this, a1); }
        
        virtual AGE_API unsigned int SizeOf(void) override { return sizeof(this); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<lvlFixedRotY, lvlFixedAny>("lvlFixedRotY")
                .endClass();
        }
    };
    ASSERT_SIZEOF(lvlFixedRotY, 0x28);

    class lvlFixedMatrix : public lvlFixedAny
    {
    private:
        Matrix34 Matrix;
    public:
        AGE_API lvlFixedMatrix(void) {
            scoped_vtable x(this);
            hook::Thunk<0x467A20>::Call<void>(this);
        }

        /*
            lvlInstance virtuals
        */

        virtual AGE_API const Vector3 & GetPosition() override
                                                            { return hook::Thunk<0x467A50>::Call<const Vector3 &>(this); }
        virtual AGE_API int IsVisible(const gfxViewport *a1)
                                                            { return hook::Thunk<0x467A60>::Call<int>(this, a1); }
        virtual AGE_API const Matrix34 & GetMatrix(Matrix34 *a1)
                                                            { return hook::Thunk<0x467AB0>::Call<const Matrix34 &>(this); }
        virtual AGE_API void SetMatrix(const Matrix34 & a1) { hook::Thunk<0x467AC0>::Call<void>(this, a1); }
        
        virtual AGE_API unsigned int SizeOf(void) override { return sizeof(this); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<lvlFixedMatrix, lvlFixedAny>("lvlFixedMatrix")
                .endClass();
        }
    };
    ASSERT_SIZEOF(lvlFixedMatrix, 0x44);

    // Lua initialization

}