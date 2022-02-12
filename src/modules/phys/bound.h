#pragma once
#include <modules\phys.h>

namespace MM2
{
    // Forward declarations
    class dgBoundBox;
    class dgBoundHotdog;
    class dgBoundSphere;
    class dgBoundGeometry;

    // External declarations
    extern class lvlMaterial;

    // Class definitions
    class dgBoundBox : public phBoundBox
    {
        lvlMaterial* Material;

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<dgBoundBox, phBoundBox>("dgBoundBox")
                .endClass();
        }
    };
    ASSERT_SIZEOF(dgBoundBox, 0x318);

    class dgBoundHotdog : public phBoundHotdog
    {
        lvlMaterial* Material;


        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<dgBoundHotdog, phBoundHotdog>("dgBoundHotdog")
                .endClass();
        }
    };
    ASSERT_SIZEOF(dgBoundHotdog, 0x88);

    class dgBoundSphere : public phBoundSphere
    {
        lvlMaterial* Material;

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<dgBoundSphere, phBoundSphere>("dgBoundSphere")
                .endClass();
        }
    };
    ASSERT_SIZEOF(dgBoundSphere, 0x84);

    class dgBoundGeometry : public phBoundGeometry
    {
        lvlMaterial* Material;

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<dgBoundGeometry, phBoundGeometry>("dgBoundGeometry")
                .endClass();
        }
    };
    ASSERT_SIZEOF(dgBoundGeometry, 0x80);

    // Lua initialization

}