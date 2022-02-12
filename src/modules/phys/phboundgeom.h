#pragma once
#include <modules\phys.h>

namespace MM2
{
    // Forward declarations
    class phBoundGeometry;

    // External declarations


    // Class definitions
    class phBoundGeometry : public phBoundPolygonal {
    private:
        Vector3* Vertices;
        phMaterial* Materials;
        phHotEdge* HotEdges;
        Vector3* EdgeNormals;
        float* EdgeCosines;
        bool KeepMemory; // if true, dtor doesn't delete materials, hotedges, normals, or cosines
    public:
        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<phBoundGeometry, phBoundPolygonal>("phBoundGeometry")
                .endClass();
        }
    };
    ASSERT_SIZEOF(phBoundGeometry, 0x7C);

    // Lua initialization

}