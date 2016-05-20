#pragma once
#include "common.h"
#include "hook.h"

#include "AGE.h"

enum MM2Version
{
    MM2_INVALID = -1,

    MM2_BETA_1  = 0,
    MM2_BETA_2  = 1,
    MM2_RETAIL  = 2,

    MM2_NUM_VERSIONS
};

const AGEGameInfo g_mm2_info[MM2_NUM_VERSIONS] = {
    // TODO: fill in values
    { 0, MM2_BETA_1, 3323, false, "NOT DEFINED" },
    { 0, MM2_BETA_2, 3366, false, "NOT DEFINED" },

    { 0x5C28FC, MM2_RETAIL, 3393, true, "Angel: 3393 / Nov  3 2000 14:34:22" },
};

class CMidtownMadness2 : public CAGEGame {
private:
    MM2Version m_gameVersion;
protected:
    PtrHook<HWND> hwndParent;

    FnHook<void> lpQuitf; // void Quitf(LPCSTR, ...)

    NOINLINE void Initialize() {
        switch (m_gameVersion) {
            case MM2_BETA_1:
            {

            } break;
            case MM2_BETA_2:
            {

            } break;
            case MM2_RETAIL:
            {
                hwndParent = 0x6830B8;

                lpQuitf = 0x4C9810;
            } break;
        }
    };
public:
    CMidtownMadness2(int engineVersion);
    CMidtownMadness2(LPAGEGameInfo gameInfo);

    NOINLINE MM2Version GetGameVersion() const {
        return m_gameVersion;
    };

    NOINLINE static bool GetGameInfo(AGEGameInfo &ppGameInfo) {
        for (auto info : g_mm2_info)
        {
            if (info.offset == 0)
                continue;

            if (strcmp((LPCSTR)info.offset, info.age_string) == 0)
            {
                ppGameInfo = info;
                return true;
            }
        }
        return false;
    };

    NOINLINE HWND GetMainWindowHwnd() const {
        return hwndParent;
    };

    template<typename ...TArgs>
    __declspec(noreturn) void Quitf(LPCSTR str, TArgs... args) {
        this->lpQuitf(str, args...);
    };
};