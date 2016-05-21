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

typedef struct MM2InitData {
    IHookPtr *lpHook;
    DWORD addresses[MM2_NUM_VERSIONS];
} *LPMM2InitData;

static const AGEGameInfo g_mm2_info[MM2_NUM_VERSIONS] = {
    // TODO: fill in values
    { 0, MM2_BETA_1, 3323, false, "NOT DEFINED" },
    { 0, MM2_BETA_2, 3366, false, "NOT DEFINED" },

    { 0x5C28FC, MM2_RETAIL, 3393, true, "Angel: 3393 / Nov  3 2000 14:34:22" },
};

namespace MM2 {
    void Printf(LPCSTR str, ...);
    void Messagef(LPCSTR str, ...);
    void Displayf(LPCSTR str, ...);
    void Warningf(LPCSTR str, ...);
    void Errorf(LPCSTR str, ...);
    void Quitf(LPCSTR str, ...);
    void Abortf(LPCSTR str, ...);
};

class CMidtownMadness2 : public CAGEGame {
private:
    MM2Version m_gameVersion;
protected:
    PtrHook<HWND> hwndParent;

    int Initialize() {
        switch (m_gameVersion) {
            case MM2_BETA_1:
            {

            } return HOOK_INIT_UNSUPPORTED;
            case MM2_BETA_2:
            {

            } return HOOK_INIT_UNSUPPORTED;
            case MM2_RETAIL:
            {
                hwndParent = 0x6830B8;
            } return HOOK_INIT_OK;
        }
        return HOOK_INIT_FAILED;
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
};