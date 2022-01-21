#pragma once
#include <modules\ped.h>

namespace MM2
{
    // Forward declarations
    class pedAnimation;
    class pedAnimationInstance;

    // External declarations
    extern class crAnimation;
    extern class crSkeleton;
    extern class crSkeletonData;
    extern class modModel;
    extern class modShader;

    // Class definitions
    
    class pedAnimation {
    public:
        struct Sequence {
            const char *Name;
            uint32_t dword4;
            float float8;
            float floatC;
            uint32_t dword10;
            crAnimation *pCrAnimation;
            uint32_t dword18;
            const char *NextName;
            uint32_t NextIndex;
        };

        int AnimationCount;
        Sequence *Animations;
        Matrix34 matrix348;
        modModel *pModel;
        modShader **ppShaders;
        crSkeleton *pSkeleton;
        crSkeletonData SkeletonData;
        float *pvoid4C;
        Vector2 *pvoid50;
        uint8_t *pvoid54;
        uint8_t *pvoid58;
        uint8_t **dword5C;
        uint32_t *pColors;
        uint32_t SphereIndex;
        int nColorCount;
        uint16_t HashTableIndex;
    };

    class pedAnimationInstance {
    protected:
        hook::Field<0x4, pedAnimation *> _animation;
        hook::Field<0x8, int> _variant;
    public:
        inline pedAnimation * getAnimation() const {
            return _animation.get(this);
        }

        inline int getVariant() const {
            return _variant.get(this);
        }

        AGE_API void Draw(bool a1)                          { hook::Thunk<0x57B370>::Call<void>(this, a1); }
    };

    // Lua initialization

}