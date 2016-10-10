#pragma once

/*
    Basically, just place any structures here. Finished or not, it's better to have them somewhere
*/

#include "mm2_gfx.h"
//#include <cstdint>

namespace MM2
{
    struct ArgEntry
    {
        std::uint32_t   WordCount;
        const char**    WordArray;
    };

    struct HashPosition
    {
        std::uint32_t   Index;
        const char*     Name;
        void*           Entry;
    };

    struct HashTable
    {
        std::uint16_t        word0;
        bool        FixedSize;
        bool        bool3;
        std::uint32_t       lpUnk4;
        std::uint32_t       MaxSlots;
        std::uint32_t       UsedSlots;

        struct HashEntry
        {
            const char*     Name;
            void*           Entry;
            HashEntry*      NextEntry;
        }** FirstEntry;

        HashTable*  PreviousHashTable;
    };

    struct asCullManager
    {
        void** lpVTable;
        std::uint32_t dword4;
        std::uint32_t dword8;
        std::uint32_t dwordC;
        std::uint32_t SomeFlags;
        std::uint32_t dword14;
        std::uint32_t dword18;
        std::uint32_t CurrentCameras;
        struct asCamera *CameraArray[16];
        int field_60;
        int CurrentCullables;
        int MaxCullables;
        int CurrentCullables2D;
        int MaxCullables2D;
        int CurrentCullables2DFG;
        int MaxCullables2DFG;
        struct asCullable **CullableArray;
        int *Cullables2D;
        struct asCullable **Cullables2DFG;
        int *CullablesMatrixes;
        int SomeOtherFlags;
        int field_90;
        int field_94;
        int Timer1;
        int Timer2;
        int field_A0;
        char ShouldReset;
        char field_A5;
        char field_A6;
        char field_A7;
    };

    struct gfxTexture
    {
        unsigned int DirectDrawSomething;
        const char *Name;
        unsigned __int16 Width;
        unsigned __int16 Height;
        unsigned int TexEnv;
        unsigned int DominantColor;
        IDirectDrawSurface7 *DirectDrawSurface;
        IDirectDrawPalette *DirectDrawPalette;
        unsigned int CacheEntry;
        unsigned int dword20;
        unsigned int RefCount;
        gfxTexture *PreviousTexture;
        gfxTexture *NextLOD;
        unsigned __int8 MaxLODCount;
        unsigned __int8 LODCount;
        unsigned __int8 EndPadding[2];
    };

    struct gfxImage
    {
        std::uint16_t Width;
        std::uint16_t Height;
        std::uint16_t Size;
        std::uint16_t Type;
        std::uint32_t TexEnv;
        void *lpImage;
        std::uint32_t *ColorData;
        std::uint32_t RefCount;
        gfxImage *PreviousImage;
        std::uint32_t dword20;
        std::uint32_t dword24;
    };

    struct gfxBitmap
    {
        void *lpAlloc;
        std::uint16_t Width;
        std::uint16_t Height;
        IDirectDrawSurface7 *DDrawSurface;
        std::uint16_t RefCount;
        gfxBitmap *PreviousBitmap;
    };
}