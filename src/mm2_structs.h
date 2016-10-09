#pragma once

/*
    Basically, just place any structures here. Finished or not, it's better to have them somewhere
*/


namespace MM2
{
    struct ArgEntry
    {
        DWORD   WordCount;
        LPCSTR* WordArray;
    };

    struct HashEntry
    {
        LPCSTR      Name;
        LPVOID      Entry;
        HashEntry*  NextEntry;
    };

    struct HashPosition
    {
        DWORD   Index;
        LPCSTR  Name;
        LPVOID  Entry;
    };

    struct HashTable
    {
        WORD        word0;
        bool        FixedSize;
        bool        bool3;
        DWORD       lpUnk4;
        DWORD       MaxSlots;
        DWORD       UsedSlots;
        HashEntry** FirstEntry;
        HashTable*  PreviousHashTable;
    };

    struct asCullManager
    {
        void** lpVTable;
        DWORD dword4;
        DWORD dword8;
        DWORD dwordC;
        _DWORD SomeFlags;
        DWORD dword14;
        _DWORD dword18;
        _DWORD CurrentCameras;
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
        WORD Width;
        WORD Height;
        WORD Size;
        WORD Type;
        DWORD TexEnv;
        void *lpImage;
        DWORD *ColorData;
        DWORD RefCount;
        gfxImage *PreviousImage;
        DWORD dword20;
        DWORD dword24;
    };

    struct gfxBitmap
    {
        void *lpAlloc;
        WORD Width;
        WORD Height;
        IDirectDrawSurface7 *DDrawSurface;
        DWORD RefCount;
        gfxBitmap *PreviousBitmap;
    };
}