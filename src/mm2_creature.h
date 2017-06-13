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
            HOOK_EXPORT(0x0057B8B0, _MemberFunc<void>, $$ctor);
            HOOK_EXPORT(0x0057B910, _MemberFunc<void>, $$dtor);

            HOOK_EXPORT(0x006B4740, _Type<MM2::pedRagdollMgr *>, Instance);
            HOOK_EXPORT(0x0057B9B0, _MemberFunc<void>, Init);
        }
    }
    
    class pedRagdollMgr : public asNode {
    public:
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

        static pedRagdollMgr * Instance(void) {
            return $::pedRagdollMgr::Instance;
        };
    };


}