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
        hook::Field<0x30, phBoundBox *> _boundBox;
    public:
        AGE_API pedRagdollMgr(void) {
            scoped_vtable x(this);
            hook::Thunk<0x57B8B0>::Call<void>(this);
        }

        virtual AGE_API ~pedRagdollMgr(void) {
            scoped_vtable x(this);
            hook::Thunk<0x57B910>::Call<void>(this);
        }

        static hook::Type<pedRagdollMgr *> Instance;

        inline phBoundBox* getBoundBox() const {
            return _boundBox.get(this);
        }

        AGE_API void Init(int skelCount, char** skelNames)  { hook::Thunk<0x57B9B0>::Call<void>(this, skelCount, skelNames); }
        AGE_API pedActive* Attach(aiPedestrianInstance* a1) { return hook::Thunk<0x57BAF0>::Call<pedActive*>(this, a1); }
        AGE_API void Detach(aiPedestrianInstance* a1)       { hook::Thunk<0x57BB80>::Call<void>(this, a1); }
        AGE_API bool UnusedActive()                         { return hook::Thunk<0x57BBB0>::Call<bool>(this); }

        /*
            asNode virtuals
        */

        virtual AGE_API void Update()                       { hook::Thunk<0x57BC10>::Call<void>(this); }
        virtual AGE_API void Reset()                        { hook::Thunk<0x57BBD0>::Call<void>(this); }


    };

    // Lua initialization

}