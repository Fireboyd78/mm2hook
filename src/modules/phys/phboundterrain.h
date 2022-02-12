#pragma once
#include <modules\phys.h>

namespace MM2
{
    // Forward declarations
    class phBoundTerrain;

    // External declarations


    // Class definitions
    class phBoundTerrain : public phBoundGeometry
    {
    private:
        bool UseHotEdges;
        Vector3 Size;
        int WidthSectionCount;
        int HeightSectionCount;
        int DepthSectionCount;
        int TotalSectionCount;
        int TotalReferencedFacesCount;
        ushort* SectionIndexOffsets;
        ushort* SectionIndexCounts;
        ushort* SectionIndices;
        int dword_ac;
        int dword_b0;
        Vector3 SectionSizeFactors;
        int dword_c0;
        int dword_c4;
        int dword_c8;
    public:
        inline int getWidthSectionCount()
        {
            return this->WidthSectionCount;
        }

        inline int getHeightSectionCount()
        {
            return this->HeightSectionCount;
        }

        inline int getDepthSectionCount()
        {
            return this->DepthSectionCount;
        }

        inline int getIndexCount(int w, int h, int d) 
        {
            int section = w * h * d;
            if (section < 0 || section >= this->TotalSectionCount)
                return -1;
            return this->SectionIndexCounts[section];
        }

        inline int getIndex(int w, int h, int d, int index)
        {
            int section = w * h * d;
            if (section < 0 || section >= this->TotalSectionCount)
                return -1;
            if (index < 0 || index >= this->SectionIndexCounts[section])
                return -1;
            return this->SectionIndices[this->SectionIndexOffsets[section] + index];
        }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<phBoundTerrain, phBoundGeometry>("phBoundTerrain")
                .addPropertyReadOnly("NumWidthSections", &getWidthSectionCount)
                .addPropertyReadOnly("NumHeightSections", &getHeightSectionCount)
                .addPropertyReadOnly("NumDepthSections", &getDepthSectionCount)

                .addFunction("GetIndexCount", &getIndexCount)
                .addFunction("GetIndex", &getIndex)
                .endClass();
        }
    };
    ASSERT_SIZEOF(phBoundTerrain, 0xCC);

    // Lua initialization

}