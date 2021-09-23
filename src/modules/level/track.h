#pragma once
#include <modules\level.h>

namespace MM2
{
    // Forward declarations
    class lvlTrackManager;

    // External declarations


    // Class definitions

    class lvlTrackManager {
    private:
        int MaxNumTracks;
        int field_04;
        int field_08;
        float HalfWheelWidth;
        float OneDivWheelWidth;
        int TracksData;
        int field_18;
        int field_1c;
        int field_20;
        int field_24;
        int field_28;
        int field_2c;
        int field_30;
        int field_34;
        int field_38;
        int field_3c;
        int field_40;
        int field_44;
        int field_48;
        int field_4c;
        Matrix34* WheelMatrixPtr;
        int field_54;
    public:
        ANGEL_ALLOCATOR

        AGE_API lvlTrackManager() {
            scoped_vtable x(this);
            hook::Thunk<0x465EC0>::Call<void>(this);
        }

        AGE_API ~lvlTrackManager() {
            scoped_vtable x(this);
            hook::Thunk<0x465ED0>::Call<void>(this);
        }

        AGE_API void Init(Matrix34 const &a1, float a2, int a3)                          { hook::Thunk<0x465EE0>::Call<void>(this, &a1, a2, a3); }
        AGE_API void Update(const Vector3 *a1, const Vector3 *a2, gfxTexture *a3)        { hook::Thunk<0x465F60>::Call<void>(this, a1, a2, a3); }
        AGE_API void Reset()                                                             { hook::Thunk<0x465F50>::Call<void>(this); }
        AGE_API void Draw()                                                              { hook::Thunk<0x466320>::Call<void>(this); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<lvlTrackManager>("lvlTrackManager")
                .addFunction("Init", &Init)
                .addFunction("Update", &Update)
                .addFunction("Reset", &Reset)
                .addFunction("Draw", &Draw)
                .endClass();
        }
    };
    ASSERT_SIZEOF(lvlTrackManager, 0x58);

    // Lua initialization

}