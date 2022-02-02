#pragma once
#include <modules\banger.h>
#include <mm2_particle.h>

namespace MM2
{
    // Forward declarations
    class dgBangerData;
    class dgBangerDataManager;

    // External declarations
    extern class asNode;
    extern class asBirthRule;

    // Class definitions

    class dgBangerData : public asNode
    {
    private:
        uint Index;
        Vector3 CG;
        Vector3 Size;
        int dword34;
        int dword38;
        int dword3C;
        Vector3* GlowOffsets;
        short NumGlows;
        short word46;
        float Mass;
        float Elasticity;
        float Friction;
        float ImpulseLimit2;
        float YRadius;
        int ColliderId;
        asBirthRule BirthRule;
        int TexNumber;
        int BillFlags;
        short SpinAxis;
        short Flash;
        byte CollisionType;
        byte CollisionPrim;
        short unknown290;
        phBound* Bound;
        short NumParts;
        short unknown298;
        int unknown300;
        int AudioId;
        int GeomSet;
        char pad138[28];
    public:
        inline Vector3 getGlowOffset(int num)
        {
            if (num >= this->NumGlows || num < 0)
                return Vector3();
            return Vector3(this->GlowOffsets[num]);
        }

        void setGlowOffset(int num, Vector3 offset)
        {
            if (num >= this->NumGlows || num < 0)
                return;
            (&this->GlowOffsets[num])->Set(offset);
        }

        inline Vector3 getCG()
        {
            return this->CG;
        }

        inline int getBillFlags()
        {
            return this->BillFlags;
        }
    public:
        //lua
        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<dgBangerData, asNode>("dgBangerData")
                .addVariableRef("Index", &dgBangerData::Index, false)
                .addVariableRef("NumGlows", &dgBangerData::NumGlows, false)
                .addVariableRef("NumParts", &dgBangerData::NumParts, false)
                .addVariableRef("GeometrySetIndex", &dgBangerData::GeomSet, false)
                .addVariableRef("BirthRule", &dgBangerData::BirthRule, false)

                .addVariableRef("CG", &dgBangerData::CG)
                .addVariableRef("Size", &dgBangerData::Size)
                .addVariableRef("Mass", &dgBangerData::Mass)
                .addVariableRef("Elasticity", &dgBangerData::Elasticity)
                .addVariableRef("Friction", &dgBangerData::Friction)
                .addVariableRef("ImpulseLimit2", &dgBangerData::ImpulseLimit2)
                .addVariableRef("YRadius", &dgBangerData::YRadius)
                .addVariableRef("ColliderId", &dgBangerData::ColliderId)
                .addVariableRef("TexNumber", &dgBangerData::TexNumber)
                .addVariableRef("BillFlags", &dgBangerData::BillFlags)
                .addVariableRef("SpinAxis", &dgBangerData::SpinAxis)
                .addVariableRef("Flash", &dgBangerData::Flash)
                .addVariableRef("CollisionType", &dgBangerData::CollisionType)
                .addVariableRef("CollisionPrim", &dgBangerData::CollisionPrim)
                .addVariableRef("AudioId", &dgBangerData::AudioId)
                .addVariableRef("Bound", &dgBangerData::Bound)

                .addFunction("GetGlowOffset", &getGlowOffset)
                .addFunction("SetGlowOffset", &setGlowOffset)
                .endClass();
        }
    };
    ASSERT_SIZEOF(dgBangerData, 0x154);

    class dgBangerDataManager : public asNode
    {
    private:
        int dataCount;
        dgBangerData datas[512];
        gfxTexture* ParticleSheetTextures[20];
        byte buffer[0x58];
    public:
        inline int getDataCount()
        {
            return dataCount;
        }

        inline dgBangerData* getData(int id)
        {
            if (id >= dataCount || id < 0)
                return nullptr;
            return &datas[id];
        }
    protected:
        static hook::Type<dgBangerDataManager*> Instance;
    public:
        inline static dgBangerDataManager* GetInstance()
        {
            return Instance.get();
        }

        /*
            asNode virtuals
        */
        AGE_API char* GetClassName() override               { return hook::Thunk<0x4415B0>::Call<char*>(this); }

        /*
            dgBangerDataManager
        */
        int AddBangerDataEntry(char const* name, char const* partName)
                                                            { return hook::Thunk<0x440940>::Call<int>(this, name, partName); }
        void ChangeData()                                   { hook::Thunk<0x440A60>::Call<void>(this); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<dgBangerDataManager, asNode>("dgBangerDataManager")
                .addStaticProperty("Instance", &dgBangerDataManager::GetInstance)
                .addFunction("AddBangerDataEntry", &AddBangerDataEntry, LUA_ARGS(LPCSTR, _opt<LPCSTR>))
                .endClass();
        }
    };
    ASSERT_SIZEOF(dgBangerDataManager, 0x2A8C4);

    // Lua initialization

}