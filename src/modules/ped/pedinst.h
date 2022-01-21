#pragma once
#include <modules\ped.h>
#include <modules\level.h>

namespace MM2
{
    // Forward declarations
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
        hook::Field<0x18, pedAnimationInstance> _animationInstance;
    public:
        inline pedAnimationInstance* getAnimationInstance() const {
            return _animationInstance.ptr(this);
        }

        aiPedestrianInstance(void)                          DONOTCALL;
        aiPedestrianInstance(const aiPedestrianInstance &&) DONOTCALL;

        /*
            lvlInstance virtuals
        */

        virtual AGE_API dgPhysEntity * AttachEntity(void)   { return hook::Thunk<0x57B730>::Call<dgPhysEntity *>(this); }
        virtual AGE_API bool IsCollidable(void)             { return hook::Thunk<0x57B780>::Call<bool>(this); }
        virtual AGE_API const Matrix34 & GetMatrix(Matrix34* a1)
                                                            { return hook::Thunk<0x57B5A0>::Call<Matrix34 const&>(this, a1); }
        virtual AGE_API const Vector3 & GetPosition()       { return hook::Thunk<0x57B5C0>::Call<Vector3 const&>(this); }
    };

    // Lua initialization

}