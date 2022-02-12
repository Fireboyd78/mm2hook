#pragma once
#include <modules\phys.h>

namespace MM2
{
    // Forward declarations
    struct phPolygon;

    // External declarations


    // Class definitions
    struct phPolygon
    {
        Vector3 Normal;
        float Radius;
        Vector4 EdgeNormalCross[4];
        ushort Indices[4];
        ushort Indices2[4];

        /*
            Lua Helpers
        */
        short getIndex(int i)
        {
            if (i < 0 || i >= 4)
                return -1;
            return (short)this->Indices[i];
        }

        void setIndex(int i, ushort index)
        {
            if (i >= 0 && i < 4)
                this->Indices[i] = index;
        }

        inline byte getMaterialIndex()
        {
            return *(byte*)&Radius;
        }

        inline void setMaterialIndex(byte index)
        {
            *(byte*)&Radius = index;
        }

        inline bool isQuad()
        {
            return this->Indices[3] != 0;
        }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<phPolygon>("phPolygon")
                .addPropertyReadOnly("IsQuad", &isQuad)
                .addVariableRef("Radius", &phPolygon::Radius)
                .addVariableRef("Normal", &phPolygon::Normal)
                .addProperty("MaterialIndex", &getMaterialIndex, &setMaterialIndex)
                .addFunction("GetIndex", &getIndex)
                .addFunction("SetIndex", &setIndex)
                .endClass();
        }
    };


    // Lua initialization

}