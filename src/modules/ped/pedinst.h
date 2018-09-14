#pragma once
#include <modules\ped.h>
#include <modules\level.h>

namespace MM2
{
    // Forward delcarations
    class aiPedestrianInstance;

    // External declarations
    extern class dgPhysEntity;
    extern class lvlInstance;

    extern class pedAnimationInstance;

    // Class definitions

    class aiPedestrianInstance : public lvlInstance {
    private:
        byte _buffer[0x14];
    protected:
        ageHook::Field<0x18, pedAnimationInstance> _animationInstance;
    public:
        inline pedAnimationInstance* getAnimationInstance() const {
            return _animationInstance.ptr(this);
        }

        aiPedestrianInstance(void)                          DONOTCALL;
        aiPedestrianInstance(const aiPedestrianInstance &&) DONOTCALL;

        /*
            lvlInstance virtuals
        */

        virtual AGE_API dgPhysEntity * AttachEntity(void)   { return ageHook::Thunk<0x57B730>::Call<dgPhysEntity *>(this); }
        virtual AGE_API bool IsCollidable(void)             { return ageHook::Thunk<0x57B780>::Call<bool>(this); }
    };

    // Lua initialization

}