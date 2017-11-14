#pragma once
#include "mm2_common.h"
#include "mm2_data.h"
#include "mm2_game.h"

namespace MM2
{
    // Forward declarations
    class UIMenu;
    class UIBMButton;
    class UIIcon;
    class UIButton;
    class UILabel;
    class mmPopup;

    //External declarations
    extern class mmGame;

    namespace $
    {
        namespace UIMenu {
            declhook(0x004E0980, _MemberFunc<void>, AssignBackground);
            declhook(0x004E1A90, _MemberFunc<MM2::UIButton *>, AddButton);
            declhook(0x004E2340, _MemberFunc<MM2::UIBMButton *>, AddBMButton);
            declhook(0x004E2340, _MemberFunc<MM2::UIIcon *>, AddIcon);
            declhook(0x004E1BB0, _MemberFunc<MM2::UILabel *>, AddLabel);
        }
    }
    
    class UILabel {
    public:
        void SetText(char* text) {
            ageHook::Thunk<0x4ED3A0>::Call<void>(this, text);
        }

        void SetBlink(bool blink) {
            ageHook::Thunk<0x4ED400>::Call<void>(this, blink);
        }
    };

    class UIIcon {

    };

    class UIWidget {

    };
    class UIButton {
    public:
        virtual void SetReadOnly(bool readOnly) {
            ageHook::Thunk<0x4ED090>::Call<void>(this, readOnly);
        }
        virtual void Enable() {
            ageHook::Thunk<0x4ED1D0>::Call<void>(this);
        }
        virtual void Disable() {
            ageHook::Thunk<0x4ED1F0>::Call<void>(this);
        }
        void SetType(int type) {
            ageHook::Thunk<0x4ED140>::Call<void>(this, type);
        }
    };


    class UIBMButton {

    };

    class UIMenu {
    public:
        AGE_API void AddButton(int unk, char* name, float f1, float f2, float f3, float f4, int i1, int i2, MM2::datCallback callback , int i3) {
            $::UIMenu::AddButton(this, unk, name, f1, f2, f3, f4, i1, i2, callback, i3);
        };
    };

    class mmPopup {
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

        AGE_API int IsEnabled(void) {
            return ageHook::Thunk<0x42A280>::Call<int>(this);
        };

        AGE_API void Lock(void) {
            ageHook::Thunk<0x42B4F0>::Call<void>(this);
        };

        AGE_API void Unlock(void) {
            ageHook::Thunk<0x42B500>::Call<void>(this);
        };

        AGE_API void ProcessChat(void) {
            ageHook::Thunk<0x42A400>::Call<void>(this);
        };
    };

}