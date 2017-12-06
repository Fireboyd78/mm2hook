#pragma once
#include "mm2_common.h"
#include "mm2_data.h"
#include "mm2_game.h"

namespace MM2
{
    // Forward declarations
    union eqEvent;
    class uiWidget;
    class UIButton;
    class UIBMButton;
    class UIIcon;
    class UILabel;
    class UIMenu;
    class mmPopup;

    //External declarations
    extern class mmGame;

    namespace $
    {
        namespace UIMenu {
            declhook(0x4E0980, _MemberFunc<void>, AssignBackground);
            declhook(0x4E1A90, _MemberFunc<MM2::UIButton *>, AddButton);
            declhook(0x4E2340, _MemberFunc<MM2::UIBMButton *>, AddBMButton);
            declhook(0x4E2340, _MemberFunc<MM2::UIIcon *>, AddIcon);
            declhook(0x4E1BB0, _MemberFunc<MM2::UILabel *>, AddLabel);
        }
    }

    union eqEvent {
        struct data {
            int p0;
            int p1;
            int p2;
            int p3;
            int p4;
            int p5;
            int p6;
            int p7;
            int p8;
        } _s;
        char buffer[36];
    };
    
    class uiWidget : public asNode {
        virtual AGE_API void Disable(void)                  { ageHook::Thunk<0x4E7330>::Call<void>(this); }
        virtual AGE_API void Enable(void)                   { ageHook::Thunk<0x4E7340>::Call<void>(this); }
        virtual AGE_API void TurnOn(void)                   { ageHook::Thunk<0x4E7350>::Call<void>(this); }
        virtual AGE_API void TurnOff(void)                  { ageHook::Thunk<0x4E7370>::Call<void>(this); }
        virtual AGE_API void SetReadOnly(BOOL readonly)     { ageHook::Thunk<0x4E7380>::Call<void>(this, readonly); }
        virtual AGE_API void Action(eqEvent event)          { ageHook::Thunk<0x4E73A0>::Call<void>(this, event); }
        virtual AGE_API void CaptureAction(eqEvent event)   { ageHook::Thunk<0x4E73B0>::Call<void>(this, event); }
        virtual AGE_API void Switch(int a1)                 { ageHook::Thunk<0x4E7480>::Call<void>(this, a1); }
        virtual AGE_API void EvalMouseX(float x)            { ageHook::Thunk<0x4E73C0>::Call<void>(this, x); }
        virtual AGE_API char * ReturnDescription(void)      { return ageHook::Thunk<0x4E73D0>::Call<char *>(this); }
        virtual AGE_API void SetPosition(float x, float y)
                                                            { ageHook::Thunk<0x4E73E0>::Call<void>(this, x, y); }
        virtual AGE_API float GetScreenHeight(void)         { return ageHook::Thunk<0x4E73F0>::Call<float>(this); }
    };

    class UIButton : public uiWidget {
    public:
        AGE_API void SetType(int type)                      { ageHook::Thunk<0x4ED140>::Call<void>(this, type); }

        /*
            asNode virtuals
        */

        virtual AGE_API void Update(void)                   { ageHook::Thunk<0x4A0DB0>::Call<void>(this); };

        /*
            uiWidget virtuals
        */

        virtual AGE_API void Disable(void)                  { ageHook::Thunk<0x4ED1F0>::Call<void>(this); }
        virtual AGE_API void Enable(void)                   { ageHook::Thunk<0x4ED1D0>::Call<void>(this); }
        virtual AGE_API void SetReadOnly(BOOL readOnly)     { ageHook::Thunk<0x4ED090>::Call<void>(this, readOnly); }
        virtual AGE_API void Action(eqEvent event)          { ageHook::Thunk<0x4ED040>::Call<void>(this, event); }
        virtual AGE_API void Switch(int a1)                 { ageHook::Thunk<0x4ED0D0>::Call<void>(this, a1); }
    };


    class UIBMButton : public uiWidget {

    };

    class UIIcon : public uiWidget {

    };

    class UILabel : public uiWidget {
    public:
        AGE_API void SetText(char* text) {
            ageHook::Thunk<0x4ED3A0>::Call<void>(this, text);
        }

        AGE_API void SetBlink(bool blink) {
            ageHook::Thunk<0x4ED400>::Call<void>(this, blink);
        }
    };

    class UIMenu : public asNode {
    public:
        AGE_API UIButton * AddButton(int unk, LocString *name, float f1, float f2, float f3, float f4, int i1, int i2, MM2::datCallback callback, int i3) {
            return $::UIMenu::AddButton(this, unk, name, f1, f2, f3, f4, i1, i2, callback, i3);
        };
    };

    class mmPopup : public asNode {
        // PUMain: 0x1C (size: 0xC8)
        // PUQuit: 0x20 (size: 0xC4)
        // PUExit: 0x24 (size: 0xBC)
        // TODO...
    private:
        byte _buffer[0x60];
    protected:
        ageHook::Field<0x18, mmGame *> _game;
    public:
        inline mmGame* getGame(void) const {
            return _game.get(this);
        };

        AGE_API int IsEnabled(void)                         { return ageHook::Thunk<0x42A280>::Call<int>(this); };
        AGE_API void Lock(void)                             { ageHook::Thunk<0x42B4F0>::Call<void>(this); };
        AGE_API void Unlock(void)                           { ageHook::Thunk<0x42B500>::Call<void>(this); };
        AGE_API void ProcessChat(void)                      { ageHook::Thunk<0x42A400>::Call<void>(this); };

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<mmPopup>("mmPopup")
                .addFunction("IsEnabled", &IsEnabled)
                .addFunction("Lock", &Lock)
                .addFunction("Unlock", &Unlock)
                .addFunction("ProcessChat", &ProcessChat)
            .endClass();
        }
    };

    template<>
    void luaAddModule<module_ui>(LuaState L) {
        luaBind<mmPopup>(L);
    }
}