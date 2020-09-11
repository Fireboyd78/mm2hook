#pragma once
#include "mm2_common.h"
#include "mm2_gfx.h"
#include "mm2_base.h"

namespace MM2
{
    // Forward declarations
    class modModel;
    class modPackage;
    class modShader;
    class modStatic;
    class asMeshSetForm;

    // External declarations
    extern class Stream;
    extern class gfxPacket;
    extern class gfxPacketList;
    extern class gfxTexture;

    class modShader {
    public:
        gfxTexture *Texture;
        gfxMaterial *Material;
    public:
        static hook::Type<gfxMaterial *> sm_Materials;
        static hook::Type<int> sm_StaticMaterialCount;

        static AGE_API void BeginEnvMap(gfxTexture *tex, const Matrix34 &mtx)
                                                                { return hook::StaticThunk<0x4A41B0>::Call<void>(tex, &mtx); }
        static AGE_API void EndEnvMap(void)                     { return hook::StaticThunk<0x4A4420>::Call<void>(); }
        static AGE_API modShader ** LoadShaderSet(Stream *stream, int *a2, int *a3, bool a4)
                                                                { return hook::StaticThunk<0x4A3F60>::Call<modShader **>(stream, a2, a3, a4); }
        static AGE_API gfxMaterial * AddStaticMaterial(const gfxMaterial &mtl)
                                                                { return hook::StaticThunk<0x4A3930>::Call<gfxMaterial *>(&mtl); }
        static AGE_API void KillAll(void)                       { return hook::StaticThunk<0x4A3B20>::Call<void>(); }
        
        AGE_API void Load(Stream *stream, bool a2)              { return hook::Thunk<0x4A3B30>::Call<void>(this, stream, a2); }
        AGE_API void PreLoad(void)                              { return hook::Thunk<0x4A40C0>::Call<void>(this); }
    };

    class modPackage {
    private:
        modPackage *next;
        Stream *stream;
        char file[32];
        uint magic;
        uint fileSize;
    public:
        AGE_API modPackage(void) {
            scoped_vtable x(this);
            hook::Thunk<0x4A46D0>::Call<void>(this);
        }

        AGE_API ~modPackage(void) {
            scoped_vtable x(this);
            hook::Thunk<0x4A46F0>::Call<void>(this);
        }

        AGE_API bool Open(LPCSTR dir, LPCSTR filename)          { return hook::Thunk<0x4A4700>::Call<bool>(this, dir, filename); }
        AGE_API void Close(void)                                { return hook::Thunk<0x4A4790>::Call<void>(this); }
        AGE_API Stream * OpenFile(LPCSTR file)                  { return hook::Thunk<0x4A47B0>::Call<Stream *>(this, file); }
        AGE_API void CloseFile(void)                            { return hook::Thunk<0x4A4800>::Call<void>(this); }
        AGE_API void SkipTo(LPCSTR file)                        { return hook::Thunk<0x4A48D0>::Call<void>(this, file); }
        AGE_API void Skip(void)                                 { return hook::Thunk<0x4A4970>::Call<void>(this); }
    };

    class modStatic {
    public:
        uint8_t PacketCount;
        uint8_t Flags;
        uint16_t FvfFlags;
        uint8_t *ShaderIndices;
        gfxPacket **ppPackets;
        gfxPacketList **ppPacketLists;

    public:
        AGE_API int GetTriCount(void) const	                    { return hook::Thunk<0x4A4DE0>::Call<int>(this); }
        AGE_API int GetAdjunctCount(void) const	                { return hook::Thunk<0x4A4DB0>::Call<int>(this); }
        AGE_API void CopyFrom(const modStatic *mod)	            { return hook::Thunk<0x4A4D60>::Call<void>(this, mod); }
        AGE_API modStatic * Clone(void) const	                { return hook::Thunk<0x4A4CA0>::Call<modStatic *>(this); }
        AGE_API void Optimize(modShader *shader)	            { return hook::Thunk<0x4A49A0>::Call<void>(this, shader); }
        AGE_API void Draw(modShader *shader) const	            { return hook::Thunk<0x4A4550>::Call<void>(this, shader); }
        AGE_API void DrawNoAlpha(modShader *shader) const	    { return hook::Thunk<0x4A4A20>::Call<void>(this, shader); }
        AGE_API void DrawEnvMapped(modShader *shader, gfxTexture *tex, float a3) const
                                                                { return hook::Thunk<0x4A4A50>::Call<void>(this, shader, tex, a3); }
        AGE_API void DrawOrthoMapped(modShader *shader, gfxTexture *tex, float a3, uint a4) const
                                                                { return hook::Thunk<0x4A4B30>::Call<void>(this, shader, tex, a3, a4); }
        AGE_API void DrawWithTexGenAndTexMatrix(void) const	    { return hook::Thunk<0x4A4C50>::Call<void>(this); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<modStatic>("modStatic")
                //functions
                .addFunction("GetTriCount", &GetTriCount)
                .addFunction("GetAdjunctCount", &GetAdjunctCount)
                .endClass();
        }
    };

    class modModel {
    private:
        byte numMaterials; // Shader + Packet count
        byte numMatrices;
        byte flags;
        byte reserved; // unused (padding)

        modShader *shaders;
        gfxPacket **packets;
    public:
        static AGE_API bool ModelAlreadyLoaded(LPCSTR model)    { return hook::StaticThunk<0x597A20>::Call<bool>(model); }
        static AGE_API void DeleteModelHash(LPCSTR model)       { return hook::StaticThunk<0x597A40>::Call<void>(model); }

        AGE_API void GetBoundingBox(Vector3 &a1, Vector3 &a2, Matrix34 *a3) const
                                                                { return hook::Thunk<0x597FB0>::Call<void>(this, &a1, &a2, a3); }
        AGE_API int GetAdjunctCount(void) const                 { return hook::Thunk<0x598190>::Call<int>(this); }
        AGE_API Vector3 & GetPosition(Vector3 &outPos, int a2) const
                                                                { return hook::Thunk<0x598230>::Call<Vector3 &>(this, &outPos, a2); }
        AGE_API void SetPosition(const Vector3 &pos, int a2) const
                                                                { return hook::Thunk<0x598290>::Call<void>(this, &pos, a2); }
        AGE_API void Draw(const Matrix44 *mtx, modShader *shader, uint a3) const
                                                                { return hook::Thunk<0x597D00>::Call<void>(this, mtx, shader, a3); }
        AGE_API void DrawPlain(const Matrix44 *mtx, uint a2) const
                                                                { return hook::Thunk<0x597F00>::Call<void>(this, mtx, a2); }
        AGE_API void DrawWithTexGenCoords(const Matrix44 *mtx, gfxTexture &tex, uint a3) const
                                                                { return hook::Thunk<0x597EA0>::Call<void>(this, mtx, &tex, a3); }
        AGE_API modShader * CopyShaders(void) const             { return hook::Thunk<0x5981C0>::Call<modShader *>(this); }
    };

    class asMeshSetForm : public asNode {
    private:
        byte _buffer[0x60];
    protected:
        hook::Field<0x30, Matrix34> _worldMatrix;
    public:
        inline Matrix34* getWorldMatrix() const {
            return _worldMatrix.ptr(this);
        }
            
        inline void setWorldMatrix(Matrix34* a1) const {
            _worldMatrix.ptr(this)->Set(a1);
        }

        AGE_API asMeshSetForm(void) {
            scoped_vtable x(this);
            hook::Thunk<0x533600>::Call<void>(this);
        }

        AGE_API ~asMeshSetForm(void) {
            scoped_vtable x(this);
            hook::Thunk<0x5339D0>::Call<void>(this);
        }

        /*
            asNode virtuals
        */

        virtual AGE_API void Cull(void)                         { hook::Thunk<0x533810>::Call<void>(this); };
        virtual AGE_API void Update(void)                       { hook::Thunk<0x5337F0>::Call<void>(this); };
    
        //Last arg is never used, so I've set it to nullptr. It's a Vector3 reference, which was meant for offset I guess.
        AGE_API void SetShape(LPCSTR modelName, LPCSTR dirName, bool useLVertex)
                                                                { hook::Thunk<0x533660>::Call<void>(this, modelName, dirName, useLVertex, nullptr); }
        AGE_API void SetZRead(bool a1)                          { hook::Thunk<0x533770>::Call<void>(this, a1); }
        AGE_API void SetZWrite(bool a1)                         { hook::Thunk<0x533790>::Call<void>(this, a1); }
        AGE_API void EnableLighting(bool a1)                    { hook::Thunk<0x5337B0>::Call<void>(this, a1); }
        AGE_API void EnableAlpha(bool a1)                       { hook::Thunk<0x5337D0>::Call<void>(this, a1); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<asMeshSetForm, asNode>("asMeshSetForm")
                .addConstructor(LUA_ARGS())
                .addProperty("WorldMatrix", &getWorldMatrix, &setWorldMatrix)
                .addFunction("SetShape", &SetShape)
                .addFunction("SetZRead", &SetZRead)
                .addFunction("SetZWrite", &SetZWrite)
                .addFunction("EnableLighting", &EnableLighting)
                .addFunction("EnableAlpha", &EnableAlpha)
            .endClass();
        }
    };

    AGE_EXT modStatic * modGetStatic(LPCSTR file, float &a2, bool a3);
    AGE_EXT modStatic * modGetStatic(LPCSTR file, void(*a2)(Vector3 &, void *), void *a3, bool a4);
    AGE_EXT modModel * modGetModel(LPCSTR filename, uint a2, bool a3, bool a4);
    AGE_EXT void modConvertModel(LPCSTR filename, LPCSTR newFilename, bool a3);
    AGE_EXT bool GetPivot(Matrix34 &, LPCSTR basename, LPCSTR file);

    template<>
    void luaAddModule<module_model>(LuaState L) {
        luaBind<modStatic>(L);
        luaBind<asMeshSetForm>(L);
    }
}