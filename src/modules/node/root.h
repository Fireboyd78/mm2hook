#pragma once
#include <modules\node.h>

namespace MM2
{
    // Forward delcarations
    class asRoot;

    // External declarations
    extern class asNode;

    // Class definitions

    class asRoot : public asNode {
    private:
        Matrix34 matrix; // used for asLinearCS::CurrentMatrix

        bool isPaused;
        bool unk_49; // pauses the game if set to true, then turns itself off (unused -- never set by anything)
        bool useNanSignal;
    public:
        AGE_API asRoot(void) {
            PUSH_VTABLE();
            ageHook::Thunk<0x4A0B70>::Call<void>(this);
            POP_VTABLE();
        }

        AGE_API virtual ~asRoot(void) {
            PUSH_VTABLE();
            ageHook::Thunk<0x4A0C90>::Call<void>(this);
            POP_VTABLE();
        }

        inline Matrix34 & getMatrix(void) {
            return matrix;
        }

        inline void setMatrix(Matrix34 &mtx) {
            memcpy(&matrix, &mtx, sizeof(Matrix34));
        }

        AGE_API void Init(bool)                             { ageHook::Thunk<0x4A0BE0>::Call<void>(this);  }
        AGE_API void Quit(void)                             { ageHook::Thunk<0x4A0C50>::Call<void>(this);  }
        AGE_API bool IsPaused(void)                         { return ageHook::Thunk<0x4A0C60>::Call<bool>(this); }
        AGE_API void TogglePause(void)                      { ageHook::Thunk<0x4A0C70>::Call<void>(this); }
        AGE_API void SetPause(bool paused)                  { ageHook::Thunk<0x4A0C80>::Call<void>(this, paused); }

        /*
            asNode virtuals
        */

        virtual AGE_API void Update(void) override          { ageHook::Thunk<0x4A0C00>::Call<void>(this); };
        virtual AGE_API void Reset(void) override           { ageHook::Thunk<0x4A0C40>::Call<void>(this); };
        virtual AGE_API char * GetClassName(void) override  { return ageHook::Thunk<0x4A0CD0>::Call<char *>(this); };
    };

    declhook(0x661738, _TypeProxy<asRoot>, ROOT);

    // Lua initialization

}