#pragma once
#include "mm2_common.h"
#include "mm2_data.h"
#include "mm2_game.h"

namespace MM2
{
    // Forward declarations
    extern class UIMenu;
    extern class UIBMButton;
    extern class UIIcon;
    extern class UIButton;
    extern class UILabel;
    extern class mmPopup;

    //External declarations
    extern class mmGame;

    namespace $
    {
        namespace mmPopup
        {
            HOOK_EXPORT(0x42A280, _MemberFunc<int>,  IsEnabled);
            HOOK_EXPORT(0x42B4F0, _MemberFunc<void>, Lock);
            HOOK_EXPORT(0x42B500, _MemberFunc<void>, Unlock);
            HOOK_EXPORT(0x42A400, _MemberFunc<void>, ProcessChat);
        }
        namespace UIButton {
            HOOK_EXPORT(0x004ED140, _MemberFunc<void>, SetType);
            HOOK_EXPORT(0x004ED1D0, _MemberFunc<void>, Enable);
            HOOK_EXPORT(0x004ED1F0, _MemberFunc<void>, Disable);
            HOOK_EXPORT(0x004ED090, _MemberFunc<void>, SetReadOnly);
         }
        namespace UILabel {
            HOOK_EXPORT(0x004ED3A0, _MemberFunc<void>, SetText);
            HOOK_EXPORT(0x004ED400, _MemberFunc<void>, SetBlink);
        }
        namespace UIBMButton {

        }
        namespace UIMenu {
            HOOK_EXPORT(0x004E0980, _MemberFunc<void>, AssignBackground);
            HOOK_EXPORT(0x004E1A90, _MemberFunc<MM2::UIButton *>, AddButton);
            HOOK_EXPORT(0x004E2340, _MemberFunc<MM2::UIBMButton *>, AddBMButton);
            HOOK_EXPORT(0x004E2340, _MemberFunc<MM2::UIIcon *>, AddIcon);
            HOOK_EXPORT(0x004E1BB0, _MemberFunc<MM2::UILabel *>, AddLabel);
        }
    }
    
    class UILabel {
    public:
        void SetText(char* text) {
            $::UILabel::SetText(this, text);
        }

        void SetBlink(bool blink) {
            $::UILabel::SetBlink(this, blink);
        }
    };

    class UIIcon {

    };

    class UIWidget {

    };
    class UIButton {
    public:
        virtual void SetReadOnly(bool readOnly) {
            $::UIButton::SetReadOnly(this, readOnly);
        }
        virtual void Enable() {
            $::UIButton::Enable(this);
        }
        virtual void Disable() {
            $::UIButton::Disable(this);
        }
        void SetType(int type) {
            $::UIButton::SetType(this, type);
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
    public:
        inline mmGame* getGame(void) const {
            return *getPtr<mmGame*>(this, 0x18);
        };

        AGE_API int IsEnabled(void) {
            return $::mmPopup::IsEnabled(this);
        };

        AGE_API void Lock(void) {
            $::mmPopup::Lock(this);
        };

        AGE_API void Unlock(void) {
            $::mmPopup::Unlock(this);
        };

        AGE_API void ProcessChat(void) {
            $::mmPopup::ProcessChat(this);
        };
    };

}