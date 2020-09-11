#pragma once
#include <modules\node.h>

namespace MM2
{
    // Forward declarations
    class asRoot;

    // External declarations
    extern class asNode;

    // Class definitions

    class asRoot : public asNode {
    private:
        Matrix34 matrix; // used for asLinearCS::CurrentMatrix

        bool isPaused;
        bool pauseNextFrame; // pauses the game if set to true, then turns itself off (used in beta1's F12 frame advance function)
        bool useNanSignal;
    public:
        AGE_API asRoot(void) {
            scoped_vtable x(this);
            hook::Thunk<0x4A0B70>::Call<void>(this);
        }

        AGE_API virtual ~asRoot(void) {
            scoped_vtable x(this);
            hook::Thunk<0x4A0C90>::Call<void>(this);
        }

        inline Matrix34 & getMatrix(void) {
            return matrix;
        }

        inline void setMatrix(Matrix34 &mtx) {
            memcpy(&matrix, &mtx, sizeof(Matrix34));
        }

        AGE_API void Init(bool)                             { hook::Thunk<0x4A0BE0>::Call<void>(this);  }
        AGE_API void Quit(void)                             { hook::Thunk<0x4A0C50>::Call<void>(this);  }
        AGE_API bool IsPaused(void)                         { return hook::Thunk<0x4A0C60>::Call<bool>(this); }
        AGE_API void TogglePause(void)                      { hook::Thunk<0x4A0C70>::Call<void>(this); }
        AGE_API void SetPause(bool paused)                  { hook::Thunk<0x4A0C80>::Call<void>(this, paused); }

        /*
            asNode virtuals
        */

        virtual AGE_API void Update(void) override          { hook::Thunk<0x4A0C00>::Call<void>(this); };
        virtual AGE_API void Reset(void) override           { hook::Thunk<0x4A0C40>::Call<void>(this); };
        virtual AGE_API char * GetClassName(void) override  { return hook::Thunk<0x4A0CD0>::Call<char *>(this); };

        //lua
        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<asRoot, asNode>("asRoot")
                .addProperty("Paused", &IsPaused, &SetPause)
                .addVariableRef("PauseNextFrame", &asRoot::pauseNextFrame)
                .addFunction("TogglePause", &TogglePause)
                .endClass();
        }
    };

    declhook(0x661738, _TypeProxy<asRoot>, ROOT);

    // Lua initialization

}