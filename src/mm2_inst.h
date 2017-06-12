#pragma once
#include "mm2_common.h"
#include "mm2_phys.h"

namespace MM2
{
    // Forward declarations
    extern class lvlInstance;
    extern class aiPedestrianInstance;
    extern class dgUnhitBangerInstance;

    namespace $
    {
        namespace lvlInstance {
            HOOK_API AGEHook<0x00463D90>::MemberFunc<void> Init;
            HOOK_API AGEHook<0x00463940>::MemberFunc<void> LoadBoundOnLastEntry;
            HOOK_API AGEHook<0x004643A0>::MemberFunc<MM2::Vector3&> GetVelocity;
        }
        namespace dgUnhitBangerInstance {
            HOOK_API AGEHook<0x00441C30>::MemberFunc<void> Init;
            HOOK_API AGEHook<0x00441D80>::Func<MM2::dgUnhitBangerInstance *> RequestBanger;
        }
        namespace aiPedestrianInstance
        {
            HOOK_API AGEHook<0x0057B780>::MemberFunc<bool> IsCollidable;
            HOOK_API AGEHook<0x0057B730>::MemberFunc<MM2::dgPhysEntity *> AttachEntity;
        }
    }
    
    class lvlInstance {
    public:
        AGE_API virtual void Init(const char *name, Matrix34 const &matrix, int unknown) PURE {
            $::lvlInstance::Init(this, name, &matrix, unknown);
        }

        AGE_API void LoadBoundOnLastEntry(char* name) {
            $::lvlInstance::LoadBoundOnLastEntry(this, name);
        }

        AGE_API virtual Vector3& GetVelocity() {
            return $::lvlInstance::GetVelocity(this);
        }
    };

    class dgUnhitBangerInstance : public lvlInstance {
    public:
        AGE_API virtual void Init(char const *name, Matrix34 const &matrix, int unknown) PURE {
                    $::dgUnhitBangerInstance::Init(this, name, matrix, unknown);
        }

        AGE_API static dgUnhitBangerInstance* RequestBanger(char const *name, int unknown) {
            return $::dgUnhitBangerInstance::RequestBanger(name, unknown);
        }
    };

    class aiPedestrianInstance : public lvlInstance {
    public:
        AGE_API virtual bool IsCollidable() {
            return $::aiPedestrianInstance::IsCollidable(this);
        }

        AGE_API virtual dgPhysEntity * AttachEntity() {
            return $::aiPedestrianInstance::AttachEntity(this);
        }
    };

}