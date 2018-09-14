#pragma once
#include <modules\ped.h>

namespace MM2
{
    // Forward declarations
    class pedRagdollMgr;

    // External declarations
    extern class asNode;
    extern class aiPedestrianInstance;
    extern class phBoundBox;

    // Class definitions

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

    // Lua initialization

}