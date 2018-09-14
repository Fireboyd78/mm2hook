#pragma once
#include <modules\ped.h>

namespace MM2
{
    // Forward delcarations
    class pedActive;
    class pedActiveData;

    // External declarations
    extern class crSkeleton;
    extern class dgPhysEntity;
    extern class dgRagdoll;

    // Class definitions

    //TODO
    class pedActiveData {};

    //TODO
    //also find out why I cant make it a dgPhysEntity
    class pedActive : public dgPhysEntity {
    protected:
        ageHook::Field<0x104, crSkeleton *> _skeleton;
        ageHook::Field<0x108, dgRagdoll *> _ragdoll;
    public:
        inline crSkeleton * getSkeleton() const {
            return _skeleton.get(this);
        }

        inline dgRagdoll * getRagdoll() {
            return _ragdoll.get(this);
        }
    };

    // Lua initialization

}