#pragma once
#include "mm2_common.h"

namespace MM2
{
    //External declarations
    extern class datParser;

    namespace $
    {
        namespace gfxLight {
            declhook(0x4B1C00, _MemberFunc<void>, Reset);
        }
    }

    class gfxViewport
    {
    private:  
        Matrix44 m_Projection;
        Matrix44 m_ViewModel;
        Matrix44 Camera;
        Matrix44 World;
        int field_100;
        int field_104;
        int field_108;
        int field_10C;
        int field_110;
        int field_114;
        int field_118;
        int field_11C;
        int Flags;
        float DefaultAspect;
        byte field_128;
        float Near;
        float Far;
        float field_134;
        float field_138;
        float field_13C;
        float float140;
        float float144;
        float float148;
        float float14C;
        float float150;
        float float154;
        float float158;
        D3DVIEWPORT7 m_Viewport;
        float Fov;
        float Aspect;
    public:
        inline byte getField_128(void) {
            return this->field_128;
        }

        inline float getNear(void) {
            return this->Near;
        }

        inline float getFar(void) {
            return this->Far;
        }

        inline float getFov(void) {
            return this->Fov;
        }

        inline float getAspect(void) {
            return this->Aspect;
        }

        inline D3DVIEWPORT7* getViewport() {
            return &this->m_Viewport;
        }

        void Ortho(float a1, float a2, float a3, float a4, float a5, float a6) { hook::Thunk<0x4B1800>::Call<void>(this, a1, a2, a3, a4, a5, a6); }
        void Perspective(float Fov, float Aspect, float Near, float Far) { hook::Thunk<0x4B1640>::Call<void>(this, Fov, Aspect, Near, Far); }
    };
    ASSERT_SIZEOF(gfxViewport, 0x17C);

    class cltLight
    {

    };

    // what kind of primitive to draw
    // same values as D3DPRIMITIVETYPE
    enum gfxDrawMode {
        DRAWMODE_POINTLIST      = 1,
        DRAWMODE_LINELIST       = 2,
        DRAWMODE_LINESTRIP      = 3,
        DRAWMODE_TRIANGLELIST   = 4,
        DRAWMODE_TRIANGLESTRIP  = 5,
        DRAWMODE_TRIANGLEFAN    = 6,
    };

    /*
        gfxInterface
    */
    enum gfxDeviceType {
        Software        = 0,    // Software (No 3D Video Card)
        Hardware        = 1,    // Hardware (3D Video Card)
        HardwareWithTnL = 2     // Hardware (3D Video Card With T&L)
    };

    enum gfxDepthFlags {
        Depth16 = 0x400,
        Depth24 = 0x200,
        Depth32 = 0x100
    };

    struct gfxResData {
        unsigned short ScreenWidth;
        unsigned short ScreenHeight;
        unsigned short ColorDepth;
        unsigned short Flags; // = ((ColorDepth == 16) + 6)
    };

    struct gfxInterface {
        /*0x00*/ GUID GUID;
        /*0x10*/ char Name[64];

        /*0x50*/ unsigned int DeviceCaps;

        /*0x54*/ gfxDeviceType DeviceType;

        /*0x58*/ unsigned int ResolutionCount;   // Max of 64 resolutions
        /*0x5C*/ unsigned int ResolutionChoice;

        /*0x60*/ gfxDepthFlags AcceptableDepths;  // Used to check if mmResolution::Depth is allowed

        /*0x64*/ unsigned int AvailableMemory;
        /*0x68*/ unsigned int VendorID;
        /*0x6C*/ unsigned int DeviceID;

        /*0x70*/ gfxResData Resolutions[64];
    };

    /* Basically a 1:1 copy of D3DLIGHT */
    struct gfxLight {
    public:
        D3DLIGHTTYPE Type;

        Vector4 Diffuse;
        Vector4 Specular;
        Vector4 Ambient;

        Vector3 Position;
        Vector3 Direction;

        float Range;
        float Falloff;

        float Attenuation0;
        float Attenuation1;
        float Attenuation2;

        float Theta;
        float Phi;

        void Reset(void) {
            $::gfxLight::Reset(this);
        };
    };

    struct gfxMaterial {
        Vector4 Diffuse;
        Vector4 Ambient;
        Vector4 Specular;
        Vector4 Emissive;

        float Shininess;

        ColorARGB Color; // 32-bit diffuse color

        AGE_API void Reset(void) {
            hook::Thunk<0x4B1BB0>::Call<void>(this);
        };
    };

    class gfxImage {
    public:
        enum gfxImageFormat {
            NONE = 0,
            ARGB_8888 = 1,
            RGB_0888  = 2,
            ARGB_1555 = 3,
            RGB_0555  = 4,
            Palette8  = 5,
            Palette4  = 6,
        };


        uint16_t Width;
        uint16_t Height;
        uint16_t Size;
        uint8_t Type;
        uint8_t PaletteType;
        uint32_t TexEnv;
        void *pImageData;
        void *pPaletteData;
        uint32_t RefCount;
        gfxImage *Next;

        void Scale(int a1, int a2)                          { hook::Thunk<0x4AEDC0>::Call<void>(this, a1, a2); }

        static gfxImage * Create(int width, int height, gfxImageFormat format, gfxImageFormat paletteType, int mipCount)
                                                            { return hook::StaticThunk<0x4AE920>::Call<gfxImage*>(width, height, format, paletteType, mipCount); }
    };
    
    class gfxTexture;

    class gfxTextureCacheEntry {
    public:
        gfxTexture* pTexture;
        IDirectDrawSurface7* pSurface;
        uint32_t LastAccessTime;
        gfxTextureCacheEntry* Next;
    public:
        inline gfxTexture * getTexture() {
            return this->pTexture;
        }

        inline gfxTextureCacheEntry * getNext() {
            return this->Next;
        }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<gfxTextureCacheEntry>("gfxTextureCacheEntry")
                .addPropertyReadOnly("NextEntry", &getNext)
                .addPropertyReadOnly("Texture", &getTexture)

                .endClass();
        }
    };

    class gfxTextureCachePool {
    public:
        uint16_t Width;
        uint16_t Height;
        uint16_t MipMapCount;
        uint16_t TextureCount;
        uint16_t EntryCount;
        uint16_t HasNoSurface;
        gfxTextureCacheEntry* First;
        gfxTextureCachePool* Next;
        DDPIXELFORMAT PixelFormat;
    public:
        inline int getMipMapCount() {
            return this->MipMapCount;
        }

        inline int getWidth() {
            return this->Width;
        }

        inline int getHeight() {
            return this->Height;
        }

        inline int getTextureCount() {
            return this->TextureCount;
        }

        inline int getEntryCount() {
            return this->EntryCount;
        }

        inline gfxTextureCachePool * getNext() {
            return this->Next;
        }

        inline gfxTextureCacheEntry* getFirst() {
            return this->First;
        }

        static hook::Type<gfxTextureCachePool*> sm_FirstPool;

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<gfxTextureCachePool>("gfxTextureCachePool")
                .addStaticProperty("FirstPool", [] { return sm_FirstPool.get(); })

                .addPropertyReadOnly("FirstEntry", &getFirst)
                .addPropertyReadOnly("NextPool", &getNext)

                .addPropertyReadOnly("Width", &getWidth)
                .addPropertyReadOnly("Height", &getHeight)
                .addPropertyReadOnly("MipMapCount", &getMipMapCount)
                .addPropertyReadOnly("EntryCount", &getEntryCount)
                .addPropertyReadOnly("TextureCount", &getTextureCount)

                .endClass();
        }
    };

    class gfxTexture {
    public:
        /*0x00*/uint BindIndex;

        /*0x04*/const char *Name;

        /*0x08*/ushort Width;
        /*0x0A*/ushort Height;

        /*0x0C*/uint TexEnv;
        /*0x10*/uint DominantColor;

        /*0x14*/IDirectDrawSurface7 *DirectDrawSurface;
        /*0x18*/IDirectDrawPalette *DirectDrawPalette;

        /*0x1C*/gfxTextureCacheEntry *CacheEntry;
        /*0x20*/gfxTextureCachePool *CachePool;

        /*0x24*/uint RefCount;

        /*0x28*/gfxTexture *Next;
        /*0x2C*/gfxTexture *NextLOD;

        /*0x30*/byte LOD;
        /*0x31*/byte MaxLOD;

        static hook::Type<bool> sm_EnableSetLOD;
        static hook::Type<bool> sm_Allow32;
        static hook::Type<gfxTexture*> sm_First;
        static hook::Type<bool> sm_UseInternalCache;
    public:
        inline const char * getName() {
            return this->Name;
        }

        inline int getWidth() {
            return this->Width;
        }

        inline int getHeight() {
            return this->Height;
        }

        inline gfxTextureCachePool * getPool() {
            return this->CachePool;
        }

        inline gfxTextureCacheEntry * getCacheEntry() {
            return this->CacheEntry;
        }

        inline gfxTexture * getNext() {
            return this->Next;
        }

        inline gfxTexture * getNextLOD() {
            return this->NextLOD;
        }
    public:
        AGE_API gfxTexture()
        {
            hook::Thunk<0x4AC8E0>::Call<void>(this);
        }

        AGE_API ~gfxTexture()
        {
            hook::Thunk<0x4AC910>::Call<void>(this);
        }

        AGE_API gfxTexture * Clone()
        {
            return hook::Thunk<0x4AE250>::Call<gfxTexture*>(this);
        }

        AGE_API void Blit(int destX, int destY, gfxTexture *src, int srcX, int srcY, int width, int height)
        {
            return hook::Thunk<0x4AE1E0>::Call<void>(this, destX, destY, src, srcX, srcY, width, height);
        }

        AGE_API static gfxTexture * Create(gfxImage* img, bool lastMip) 
        {
            return hook::StaticThunk<0x4AD090>::Call<gfxTexture*>(img, lastMip);
        }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<gfxTexture>("gfxTexture")
                .addStaticProperty("First", [] { return sm_First.get(); })
                .addStaticProperty("UseInternalCache", [] { return sm_UseInternalCache.get(); })

                .addPropertyReadOnly("Name", &getName)
                .addPropertyReadOnly("Next", &getNext)
                .addPropertyReadOnly("NextLOD", &getNextLOD)

                .addPropertyReadOnly("Width", &getWidth)
                .addPropertyReadOnly("Height", &getHeight)
                .addPropertyReadOnly("CacheEntry", &getCacheEntry)
                .addPropertyReadOnly("CachePool", &getPool)

                .endClass();
        }
    };

    class gfxPacket {
    public:
        gfxPacket *Next;
        IDirect3DVertexBuffer7 *VertexBuffer;
        int *Positions;
        void *Vertices;
        ushort VertexTypeDesc;
        short StartVertex;
        ushort AdjunctCount;
        ushort TriCount;
        ushort *Indices;
        byte* unk_1C;
        byte* unk_20;
        byte unk_24;
        uint unk_28;

        void DoLock()
        {
            hook::Thunk<0x4B4720>::Call<void>(this);
        }

        void DoUnlock()
        {
            hook::Thunk<0x4B4740>::Call<void>(this);
        }

        void GetPosition(Vector3* out, int index)
        {
            hook::Thunk<0x4B3C10>::Call<void>(this, out, index);
        }

        void GetNormal(Vector3* out, int index) {
            hook::Thunk<0x4B3CF0>::Call<void>(this, out, index);
        }

        void GetTexCoord(Vector2* out, int index)
        {
            hook::Thunk<0x4B3E20>::Call<void>(this, out, index);
        }

        void GetTri(int* out, int index)
        {
            out[0] = Indices[(3 * index) + 0];
            out[1] = Indices[(3 * index) + 1];
            out[2] = Indices[(3 * index) + 2];
        }

        void SetNormal(const Vector3& normal, int index) {
            hook::Thunk<0x4B3D90>::Call<void>(this, &normal, index);
        }

        void SetPosition(const Vector3& position, int index) {
            hook::Thunk<0x4B3C70>::Call<void>(this, &position, index);
        }
    };

    class gfxBitmap {
    public:
        const char* Name;
        uint16_t Width;
        uint16_t Height;
        IDirectDrawSurface7 *pSurface;
        uint32_t RefCount;
        gfxBitmap *Next;
    };

    class gfxPipeline {
    public:
        static gfxViewport * CreateViewport() {
            return hook::StaticThunk<0x4A90B0>::Call<gfxViewport*>();
        }

        static void ForceSetViewport(gfxViewport* viewport) {
            hook::StaticThunk<0x4B2EE0>::Call<void>(viewport);
        }

        static void CopyBitmap(int destX, int destY, gfxBitmap* bitmap, int srcX, int srcY, int width, int height, bool srcColorKey) {
            hook::StaticThunk<0x4AB4C0>::Call<void>(destX, destY, bitmap, srcX, srcY, width, height, srcColorKey);
        }

        static void StartFade(uint color, float time) {
            hook::StaticThunk<0x4B2CE0>::Call<void>(color, time);
        }

        static void SetFade(uint color) {
            hook::StaticThunk<0x4B2D20>::Call<void>(color);
        }

        static void ClearRect(int a1, int a2, int a3, int a4, uint a5) {
            hook::StaticThunk<0x4AB520>::Call<void>(a1, a2, a3, a4, a5);
        }
    };

    // yes, this is actually how it is in MM2
    // seems like Angel moved everything to gfxPacket

    class gfxPacketList : public gfxPacket {};

    class gfxRenderStateData {
        /*
            Don't edit this unless you know what you're doing!
        */
    public:
        bool Clipping;
        bool Lighting;

        byte SrcBlend;              /* D3DBLEND */
        byte DestBlend;             /* D3DBLEND */
        byte VertexBlend;           /* D3DVERTEXBLENDFLAGS */

        int TextureArg;             /* D3DTA_* member */
        int BlendSet;               /* gfxBlendSet */

        float FogStart;             /* D3DRENDERSTATE_FOGSTART */
        float FogEnd;               /* D3DRENDERSTATE_FOGEND */

        byte ZEnable;               /* D3DZBUFFERTYPE */
        byte ShadeMode;             /* D3DSHADEMODE */
        bool ZWriteEnable;
        byte CullMode;              /* D3DCULL */
        byte ZFunc;                 /* D3DCMPFUNC */
        byte AlphaRef;              /* 0x0 - 0xFF */
        byte AlphaFunc;             /* D3DCMPFUNC */
        bool DitherEnable;
        bool AlphaEnable;           /* ALPHABLENDENABLE and ALPHATESTENABLE */
        bool TexturePerspective;
        byte FillMode;              /* D3DSHADEMODE */
        byte ZBias;                 /* 0 - 16 */
        byte AntiAlias;             /* D3DANTIALIASMODE */
        byte FogVertexMode;         /* D3DFOGMODE */

        byte AddrU[2];              /* D3DTSS_ADDRESSU States */
        byte AddrV[2];              /* D3DTSS_ADDRESSV States */
        
        int unk_2C;                 /* unused */
        
        ColorARGB Ambient;

        ColorARGB FogColor;
        float FogDensity;

        byte ColorOp1;
        byte ColorOp2;              /* educated guess -- unused */

        bool SpecularEnable;
        bool FogEnable;
        bool NormalizeNormals;
        bool RangeFogEnable;

        gfxTexture *Texture[2];
        gfxMaterial *Material;
    };

    class gfxRenderState {
    public:
        gfxRenderStateData Data;
        gfxMaterial Material;

        static hook::Type<int> m_Touched;
        static hook::Type<int> m_TouchedMask;
        static hook::Type<int[2]> m_TouchedMasks;

        static hook::TypeProxy<Matrix44> sm_Camera;
        static hook::TypeProxy<Matrix44> sm_World;

        static hook::TypeProxy<Matrix44> sm_Composite;
        static hook::TypeProxy<Matrix44> sm_FullComposite;

        static hook::TypeProxy<Matrix44> sm_View;
        static hook::TypeProxy<Matrix44> sm_Modelview;

        // educated guess -- applied to view?
        static hook::TypeProxy<Matrix44> sm_Transform;
    public:
        static void SetCamera(Matrix44 * mtx)   { hook::StaticThunk<0x4B2A20>::Call<void>(mtx); }
        void SetBlendSet(int a1, UCHAR a2)      { hook::Thunk<0x4B2350>::Call<void>(this, a1, a2); }
    };

    class ltLight {
    public:
        int Type;
        Vector3 Position;
        Vector3 Direction;
        Vector3 Color;
        float Intensity;
        int unk_2C;
        float SpotExponent;
        gfxTexture* HighlightTexture;
        int unk_38;
        float ProjectionSize;
        bool EnableProjection;
        byte unk_41;
        byte unk_42;
        byte unk_43;
        int ShadowMode;
        int unk_48;

        ANGEL_ALLOCATOR 

        AGE_API ltLight() { hook::Thunk<0x59AB80>::Call<void>(this); }
        AGE_API ~ltLight() { hook::Thunk<0x59ABC0>::Call<void>(this); }

        //static funcs
        static AGE_API void ShutdownLights() { hook::StaticThunk<0x59ABD0>::Call<void>(); }
        static AGE_API void DrawGlowBegin() { hook::StaticThunk<0x59AE30>::Call<void>(); }
        static AGE_API void DrawGlowEnd() { hook::StaticThunk<0x59AEF0>::Call<void>(); }
        static AGE_API void SetUpGfxLightBegin(const Vector3& a1)
        {
            hook::StaticThunk<0x59B390>::Call<void>(&a1);
        }
        static AGE_API void SetUpGfxLightEnd() { hook::StaticThunk<0x59B460>::Call<void>(); }
        static AGE_API int GetNumPointLights() { return hook::StaticThunk<0x59B3E0>::Call<int>(); }
        static AGE_API ltLight* GetPointLight(int a1) { return hook::StaticThunk<0x59B3F0>::Call<ltLight*>(a1); }
        static AGE_API ltLight* GetClosestLight() { return hook::StaticThunk<0x59B410>::Call<ltLight*>(); }


        //member funcs
        AGE_API void Default() { hook::Thunk<0x59ABF0>::Call<void>(this); }
        AGE_API void Random() { hook::Thunk<0x59AC40>::Call<void>(this); }
        AGE_API void Draw(float scale) { hook::Thunk<0x59ACB0>::Call<void>(this, scale); }
        AGE_API void DrawGlow(const Vector3& position) { hook::Thunk<0x59AD90>::Call<void>(this, &position); }
        AGE_API void DrawHighlight(const Vector3& a1, const Matrix34& a2)
        {
            hook::Thunk<0x59AFB0>::Call<void>(this, &a1, &a2);
        }
        AGE_API void SetUpGfxLight() { hook::Thunk<0x59B5B0>::Call<void>(this); }
        AGE_API bool SetGfxLight(gfxLight* a1, const Vector3& a2)
        {
            return hook::Thunk<0x59B740>::Call<bool>(this, a1, &a2);
        }
        AGE_API void Illuminate(Vector3& outColor, Vector3& a2, Vector3& a3)
        {
            hook::Thunk<0x59B990>::Call<void>(this, &outColor, &a2, &a3);
        }
        AGE_API float ComputeIntensity(const Vector3& a1, float a2)
        {
            return hook::Thunk<0x59BA50>::Call<float>(this, &a1, a2);
        }
        AGE_API float ComputeDistance(const Vector3& a1) { return hook::Thunk<0x59BB70>::Call<float>(this, &a1); }

        //TODO
        /*AGE_API void SetUpProjection(ltProjection& a1)  { hook::Thunk<0x59BBB0 >::Call<void>(this, &a1); }*/

        AGE_API void FileIO(datParser* a1) { hook::Thunk<0x59BCA0>::Call<void>(this, a1); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<ltLight>("ltLight")
                //ctor
                .addConstructor(LUA_ARGS())
                //variables
                .addVariableRef("Type", &ltLight::Type)
                .addVariableRef("Position", &ltLight::Position)
                .addVariableRef("Direction", &ltLight::Direction)
                .addVariableRef("Color", &ltLight::Color)
                .addVariableRef("Intensity", &ltLight::Intensity)
                .addVariableRef("SpotExponent", &ltLight::SpotExponent)
                .addVariableRef("ProjectionSize", &ltLight::ProjectionSize)
                .addVariableRef("EnableProjection", &ltLight::EnableProjection)
                .addVariableRef("ShadowMode", &ltLight::ShadowMode)

                //statics
                .addStaticFunction("ShutdownLights", &ShutdownLights)
                .addStaticFunction("DrawGlowBegin", &DrawGlowBegin)
                .addStaticFunction("DrawGlowEnd", &DrawGlowEnd)
                .addStaticFunction("SetUpGfxLightBegin", &SetUpGfxLightBegin)
                .addStaticFunction("SetUpGfxLightEnd", &SetUpGfxLightEnd)
                .addStaticFunction("GetNumPointLights", &GetNumPointLights)
                .addStaticFunction("GetClosestLight", &GetClosestLight)
                .addStaticFunction("GetPointLight", &GetPointLight)

                //members
                .addFunction("Default", &Default)
                .addFunction("Random", &Random)
                .addFunction("Draw", &Draw)
                .addFunction("DrawGlow", &DrawGlow)
                .addFunction("DrawHighlight", &DrawHighlight)
                .addFunction("ComputeDistance", &ComputeDistance)
                .addFunction("ComputeIntensity", &ComputeIntensity)
                .addFunction("Illuminate", &Illuminate)
                .addFunction("SetUpGfxLight", &SetUpGfxLight)
                .addFunction("SetGfxLight", &SetGfxLight)
                .addFunction("FileIO", &FileIO)

                .endClass();
        }
    };
    ASSERT_SIZEOF(ltLight, 0x4C);

    class ltLensFlare {
    public:
        byte _buffer[0x20];

        ANGEL_ALLOCATOR

        AGE_API ltLensFlare(int a1) { hook::Thunk<0x59BE80>::Call<void>(this, a1); }
        AGE_API ~ltLensFlare() { hook::Thunk<0x59BF80>::Call<void>(this); }

        //member funcs
        AGE_API void DrawBegin() { hook::Thunk<0x59BFA0>::Call<void>(this); }
        AGE_API void DrawEnd() { hook::Thunk<0x59C0C0>::Call<void>(this); }
        AGE_API void Draw(Vector3 &position, Vector3 &color, float intensity) { hook::Thunk<0x59C1C0>::Call<void>(this, &position, &color, intensity); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<ltLensFlare>("ltLensFlare")
                //members
                .addFunction("DrawBegin", &DrawBegin)
                .addFunction("DrawEnd", &DrawEnd)
                .addFunction("Draw", &Draw)
                .endClass();
        }
    };
    ASSERT_SIZEOF(ltLensFlare, 0x20);

    // Statically available functions
    static gfxTexture * gfxGetTexture(const char *a1, bool a2 = true) {
        return hook::StaticThunk<0x4B30F0>::Call<gfxTexture*>(a1, a2);
    }

    static gfxBitmap * gfxGetBitmap(const char *a1, bool a2 = true, bool a3 = true) {
        return hook::StaticThunk<0x4B3210>::Call<gfxBitmap*>(a1, a2, a3);
    }

    //
    declhook(0x4ABE00, _Func<bool>, $gfxAutoDetect);
    declhook(0x4A8CE0, _Func<void>, $gfxPipeline_SetRes);

    declhook(0x4A88F0, _Func<WNDPROC>, $gfxPipeline_gfxWindowProc);

    declhook(0x4AC3D0, _Func<LPD3DENUMDEVICESCALLBACK7>, $DeviceCallback);
    declhook(0x4AC6F0, _Func<LPDDENUMMODESCALLBACK2>, $ResCallback);

    declhook(0x682FA0, _Type<HWND>, hWndParent);
    declhook(0x6830B8, _Type<HWND>, hWndMain);

    declhook(0x68311C, _Type<LPCSTR>, lpWindowTitle);

    declhook(0x6830F0, _Type<ATOM>, ATOM_Class);
    declhook(0x683108, _Type<LPCSTR>, IconID);

    declhook(0x5CA3EC, _Type<bool>, pageFlip);
    declhook(0x5CA3ED, _Type<bool>, hasBorder);
    declhook(0x5CA3EE, _Type<bool>, useMultiTexture);
    declhook(0x5CA664, _Type<bool>, enableHWTnL);

    declhook(0x68451D, _Type<bool>, novblank);

    declhook(0x6830D0, _Type<bool>, inWindow);
    declhook(0x6830D1, _Type<bool>, isMaximized);
    declhook(0x6830D2, _Type<bool>, tripleBuffer);
    declhook(0x6830D3, _Type<bool>, useReference);
    declhook(0x6830D4, _Type<bool>, useSoftware);
    declhook(0x6830D5, _Type<bool>, useAgeSoftware);
    declhook(0x6830D6, _Type<bool>, useBlade);
    declhook(0x6830D7, _Type<bool>, useSysMem);

    declhook(0x6830D8, _Type<int>, useInterface);

    declhook(0x684518, _Type<LPDIRECTDRAWCREATEEX>, lpDirectDrawCreateEx);

    declhook(0x6830A8, _Type<IDirectDraw7 *>, lpDD);
    declhook(0x6830AC, _Type<IDirect3D7 *>, lpD3D);
    declhook(0x6830C8, _Type<IDirect3DDevice7 *>, lpD3DDev);
    declhook(0x6830CC, _Type<IDirectDrawSurface7 *>, lpdsRend);

    declhook(0x683130, _Type<gfxInterface>, gfxInterfaces);
    declhook(0x6844C0, _Type<unsigned int>, gfxInterfaceCount);

    declhook(0x6844C8, _Type<int>, gfxInterfaceChoice);

    declhook(0x6844B0, _Type<int>, gfxMinScreenWidth);
    declhook(0x6844CC, _Type<int>, gfxMinScreenHeight);

    declhook(0x6844FC, _Type<int>, gfxMaxScreenWidth);
    declhook(0x6844D8, _Type<int>, gfxMaxScreenHeight);

    declhook(0x6B165C, _Type<int>, gfxTexQuality);
    declhook(0x6857D0, _Type<int>, gfxTexReduceSize);

    declhook(0x5CD38C, _Type<gfxImage * (*)(const char *, bool)>, gfxLoadImage);
    declhook(0x5CD680, _Type<gfxImage * (*)(gfxImage*, const char *, bool)>, gfxPrepareImage);

    declhook(0x6B0454, _Type<uint>, mmCpuSpeed);
    
    declhook(0x683124, _Type<gfxViewport*>, gfxCurrentViewport);

    declhook(0x6830F4, _Type<float>, window_fWidth);
    declhook(0x683120, _Type<float>, window_fHeight);

    declhook(0x683128, _Type<int>, window_iWidth);
    declhook(0x683100, _Type<int>, window_iHeight);

    declhook(0x6830E4, _Type<int>, window_ZDepth);
    declhook(0x6830F8, _Type<int>, window_ColorDepth);

    declhook(0x6830EC, _Type<int>, window_X);
    declhook(0x683110, _Type<int>, window_Y);

    declhook(0x6854A0, _TypeProxy<gfxRenderStateData>, LASTRSTATE);
    declhook(0x6856A0, _TypeProxy<gfxRenderState>, RSTATE);

    template<>
    void luaAddModule<module_gfx>(LuaState L) {
        luaBind<ltLight>(L);
        luaBind<ltLensFlare>(L);
        luaBind<gfxTexture>(L);
        luaBind<gfxTextureCacheEntry>(L);
        luaBind<gfxTextureCachePool>(L);

        LuaBinding(L).addFunction("gfxGetTexture", &gfxGetTexture);
    }

    ASSERT_SIZEOF(gfxRenderStateData, 0x50);
    ASSERT_SIZEOF(gfxRenderState, 0x98);
}
