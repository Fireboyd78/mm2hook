#pragma once
#include "mm2_base.h"
#include "mm2_common.h"
#include "mm2_phys.h"
#include "mm2_bound.h"
#include "mm2_vector.h"
#include "mm2_model.h"
#include "mm2_inst.h"

namespace MM2
{
    // Forward declarations
    extern class pedRagdollMgr;
    class crSkeleton;
    class crSkeletonData;
    
    //External declarations
    extern class dgPhysEntity;
    extern class asNode;
    extern class phBoundBox;
    extern class aiPedestrianInstance;
    
    //TODO
    class dgRagdoll {
    public:
        AGE_API Matrix34* GetMatrix()                       { return ageHook::Thunk<0x5A7D70>::Call<Matrix34*>(this); }
    };

    //TODO
    //also find out why I cant make it a dgPhysEntity
    class pedActive /*: public dgPhysEntity*/ {
    protected:
        ageHook::Field<0x104, crSkeleton*> _skeleton;
        ageHook::Field<0x108, dgRagdoll*> _ragdoll;
    public:
        inline crSkeleton* getSkeleton() const {
            return _skeleton.get(this);
        }

        inline dgRagdoll* getRagdoll() {
            return _ragdoll.get(this);
        }
    };
        
    class crSkeletonData {
        //TODO;
    };

    class crSkeleton {
    public:
        AGE_API void Init(crSkeletonData* a1, Matrix34* a2) { ageHook::Thunk<0x57C5D0>::Call<void>(this, a1, a2); }
        AGE_API void Update()                               { ageHook::Thunk<0x57C630>::Call<void>(this); }
        AGE_API void Attach(Matrix34* a1)                   { ageHook::Thunk<0x57C650>::Call<void>(this, a1); }
        AGE_API void Attach(Matrix44* a1)                   { ageHook::Thunk<0x57C690>::Call<void>(this, a1); }
    };

    class crAnimation {
        //TODO
    };

    struct AnimationSequence {
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

    struct pedAnimation {
        int AnimationCount;
        AnimationSequence *Animations;
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
        ageHook::Field<0x4, pedAnimation*> _animation;
        ageHook::Field<0x8, int> _variant;
    public:
        inline pedAnimation* getAnimation() const {
            return _animation.get(this);
        }

        inline int getVariant() const {
            return _variant.get(this);
        }
    };

    class pedRagdollMgr : public asNode {
    private:
        byte _buffer[0x64]; //guessed size
    protected:
        ageHook::Field<0x30, phBoundBox*> _boundBox;
    public:
        inline phBoundBox* getBoundBox() const {
            return _boundBox.get(this);
        }

        static ageHook::Type<pedRagdollMgr *> Instance;

        AGE_API pedRagdollMgr(void) {
            PUSH_VTABLE();
            ageHook::Thunk<0x57B8B0>::Call<void>(this);
            POP_VTABLE();
        }

        virtual AGE_API ~pedRagdollMgr(void) {
            PUSH_VTABLE();
            ageHook::Thunk<0x57B910>::Call<void>(this);
            POP_VTABLE();
        }

        AGE_API void Init(int skelCount, char** skelNames)  { ageHook::Thunk<0x57B9B0>::Call<void>(this, skelCount, skelNames); }
        AGE_API pedActive* Attach(aiPedestrianInstance* a1) { return ageHook::Thunk<0x57BAF0>::Call<pedActive*>(this, a1); }
        AGE_API void Detach(aiPedestrianInstance* a1)       { ageHook::Thunk<0x57BB80>::Call<void>(this, a1); }
        AGE_API bool UnusedActive()                         { return ageHook::Thunk<0x57BBB0>::Call<bool>(this); }
        
        /*
            asNode virtuals
        */

        AGE_API virtual void Update()                       { ageHook::Thunk<0x57BC10>::Call<void>(this); }
        AGE_API virtual void Reset()                        { ageHook::Thunk<0x57BBD0>::Call<void>(this); }


    };

    template<>
    void luaAddModule<module_creature>(LuaState L) {

    }
}