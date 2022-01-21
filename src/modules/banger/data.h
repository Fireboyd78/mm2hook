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
        int unknown272;
        int TexNumber;
        int BillFlags;
        short SpinAxis;
        short Flash;
        byte CollisionType;
        byte CollisionPrim;
        short unknown290;
        int BoundPtr;
        short NumParts;
        short unknown298;
        int unknown300;
        int AudioId;
        int GeomSet;
        char pad138[24];
    public:
        inline int getBillFlags()
        {
            return this->BillFlags;
        }

        inline Vector3 getCG()
        {
            return this->CG;
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
        static hook::Type<dgBangerDataManager*> Instance;

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
    public:
    };

    ASSERT_SIZEOF(dgBangerDataManager, 0x2A8C4);

    // Lua initialization

}