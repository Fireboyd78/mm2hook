#pragma once
#include <modules\node.h>

namespace MM2
{
    // Forward declarations
    class asLinearCS;

    // External declarations


    // Class definitions
    class asLinearCS : public asNode {
    public:
        Matrix34 field_18;
        Matrix34 field_48;
        int field_78;
        int field_7c;

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<asLinearCS, asNode>("asLinearCS")
                .endClass();
        }
    };
    ASSERT_SIZEOF(asLinearCS, 0x80);

    // Lua initialization

}