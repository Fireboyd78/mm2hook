#pragma once
#include "common.h"
#include "hook.h"

#include "AGE.h"

/*
    Shared resources
*/
#include "mm2_utils.h"
#include "mm2_vector.h"

/*
    Class definitions
*/
#include "mm2_ai.h"
#include "mm2_city.h"
#include "mm2_data.h"
#include "mm2_game.h"
#include "mm2_input.h"
#include "mm2_stream.h"
#include "mm2_vehicle.h"

static const AGEGameInfo g_mm2_info[] = {
    { 0x5AB7F8, MM2_BETA_1, 3323, true, "Angel: 3323 / Jun 29 2000 11:52:28" },
    { 0x5C18EC, MM2_BETA_2, 3366, true, "Angel: 3366 / Aug  8 2000 10:08:04" },

    { 0x5C28FC, MM2_RETAIL, 3393, true, "Angel: 3393 / Nov  3 2000 14:34:22" },

    // PEtite'd Beta 2...
    { 0x6B602D, MM2_BETA_2_PETITE, 3366, false, "ERROR!" },
};

namespace MM2 {
    void Printf(LPCSTR str, ...);
    void Messagef(LPCSTR str, ...);
    void Displayf(LPCSTR str, ...);
    void Warningf(LPCSTR str, ...);
    void Errorf(LPCSTR str, ...);
    void Quitf(LPCSTR str, ...);
    void Abortf(LPCSTR str, ...);

    char * AngelReadString(UINT stringId);

    extern MM2PtrHook<char> szCityName;
    extern MM2PtrHook<char> szCityName2;
};

class CMidtownMadness2 : public CAGEGame {
private:
    MM2Version m_gameVersion;
protected:
    MM2PtrHook<HWND> hwndParent = { 0x64993C, 0x681B00, 0x6830B8 };
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