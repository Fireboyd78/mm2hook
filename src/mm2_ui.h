#pragma once
#include "mm2_common.h"
#include "mm2_game.h";
#include "mm2_data.h";

namespace MM2
{
    // Forward declarations
    extern class UIMenu;
	extern class  UIBMButton;
	extern class UIIcon;
	extern class UIButton;

    namespace $
    {
		namespace UIButton {
			HOOK_API AGEHook<0x004ED140>::MemberFunc<void>SetType;
			HOOK_API AGEHook<0x004ED1D0>::MemberFunc<void>Enable;
			HOOK_API AGEHook<0x004ED1F0>::MemberFunc<void>Disable;
			HOOK_API AGEHook<0x004ED090>::MemberFunc<void>SetReadOnly;
 	    }
		namespace UIBMButton {

		}
		namespace UIMenu {
			HOOK_API AGEHook<0x004E0980>::MemberFunc<void> AssignBackground;
			HOOK_API AGEHook<0x004E1A90>::MemberFunc<MM2::UIButton *> AddButton;
			HOOK_API AGEHook<0x004E2340>::MemberFunc<MM2::UIBMButton *> AddBMButton;
			HOOK_API AGEHook<0x004E2340>::MemberFunc<MM2::UIIcon *> AddIcon;
		}
    }

	class UIIcon {

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

}