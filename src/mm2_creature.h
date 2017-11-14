#pragma once
#include "mm2_base.h"
#include "mm2_common.h"

namespace MM2
{
    // Forward declarations
    extern class pedRagdollMgr;

    //External declarations
    extern class asNode;

    namespace $
    {
        namespace  pedRagdollMgr {
            declhook(0x57B8B0, _MemberFunc<void>, $$ctor);
            declhook(0x57B910, _MemberFunc<void>, $$dtor);

            declhook(0x57B9B0, _MemberFunc<void>, Init);
        }
    }
    
    class pedRagdollMgr : public asNode {
    public:
        static ageHook::Type<pedRagdollMgr *> Instance;

        AGE_API pedRagdollMgr(void) {
            PUSH_VTABLE();
            $::pedRagdollMgr::$$ctor(this);
            POP_VTABLE();
        }

        AGE_API virtual ~pedRagdollMgr(void) {
            PUSH_VTABLE();
            $::pedRagdollMgr::$$dtor(this);
            POP_VTABLE();
        }

        //HOW DO I ACCEPT ARGS?
        AGE_API void Init(int numThings, char** things) {
            $::pedRagdollMgr::Init(this, numThings, things);
        }
    };
}