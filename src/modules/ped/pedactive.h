#pragma once
#include <modules\ped.h>

namespace MM2
{
    // Forward declarations
    class pedActive;
    class pedActiveData;

    // External declarations
    extern class crSkeleton;
    extern class dgPhysEntity;
    extern class dgRagdoll;

    // Class definitions

    //TODO
    class pedActiveData {};

    class pedActive : public dgPhysEntity {
    protected:
        hook::Field<0x104, crSkeleton *> _skeleton;
        hook::Field<0x108, dgRagdoll *> _ragdoll;
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