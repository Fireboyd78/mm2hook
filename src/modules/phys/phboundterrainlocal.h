#pragma once
#include <modules\phys.h>

namespace MM2
{
    // Forward declarations
    class phBoundTerrainLocal;

    // External declarations


    // Class definitions
    class phBoundTerrainLocal : public phBoundTerrain
    {
    public:
        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<phBoundTerrainLocal, phBoundTerrain>("phBoundTerrainLocal")
                .endClass();
        }
    };
    ASSERT_SIZEOF(phBoundTerrainLocal, 0xCC);

    // Lua initialization

}