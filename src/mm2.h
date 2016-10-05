#pragma once
#include "mm2_common.h"

/*
    Class definitions
*/
#include "mm2_ai.h"
#include "mm2_base.h"
#include "mm2_city.h"
#include "mm2_data.h"
#include "mm2_game.h"
#include "mm2_gfx.h"
#include "mm2_input.h"
#include "mm2_stream.h"
#include "mm2_vehicle.h"

static const ageInfoLookup g_mm2_info[] = {
    { 0x5AB7F8, true, { MM2_BETA_1, 3323, "Angel: 3323 / Jun 29 2000 11:52:28" }},
    { 0x5C18EC, true, { MM2_BETA_2, 3366, "Angel: 3366 / Aug  8 2000 10:08:04" }},
    { 0x5C28FC, true, { MM2_RETAIL, 3393, "Angel: 3393 / Nov  3 2000 14:34:22" }},

    // PEtite'd Beta 2...
    { 0x6B602D, false, { MM2_BETA_2_PETITE, 3366, "ERROR!" }},
    { NULL },
};

class CMidtownMadness2 : public ageGame {
protected:
    static MM2PtrHook<HWND> $hwndMain;
public:
    CMidtownMadness2(const ageInfo &gameInfo)
        : ageGame(gameInfo) {};
    CMidtownMadness2(short gameVersion, short engineVersion)
        : ageGame(gameVersion, engineVersion) {};
    CMidtownMadness2(short gameVersion, short engineVersion, LPCSTR versionString)
        : ageGame(gameVersion, engineVersion, versionString) {};

    static bool GetGameInfo(ageInfoLookup &outInfo) {
        return GetAGEInfo(g_mm2_info, outInfo);
    };

    MM2Version GetVersion() const {
        return (MM2Version)m_info.gameVersion;
    };

    virtual HWND GetMainWindowHwnd() const {
        return $hwndMain;
    };

    virtual void Initialize() {
        MM2HookMgr::Initialize(GetVersion());
    };
};