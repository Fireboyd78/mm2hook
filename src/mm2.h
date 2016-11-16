#pragma once

#include "mm2_common.h"

/*
    Class definitions
*/
#include "mm2_ai.h"
#include "mm2_audio.h"
#include "mm2_base.h"
#include "mm2_city.h"
#include "mm2_data.h"
#include "mm2_game.h"
#include "mm2_gfx.h"
#include "mm2_input.h"
#include "mm2_rgl.h"
#include "mm2_stream.h"
#include "mm2_vehicle.h"

#include "mm2_lua.h"

enum MM2Version
{
    MM2_RETAIL = 0,
    
    // number of supported versions
    MM2_NUM_VERSIONS,

    // unsupported versions
    MM2_BETA_1,
    MM2_BETA_2,
    MM2_BETA_2_PETITE,

    // the first release of MM2
    // don't see a point in supporting this
    MM2_VANILLA,

    MM2_INVALID = -1,
};

static const ageInfoLookup g_mm2_info[] = {
    { 0x5C28FC, true, { MM2_RETAIL, 3393, "Angel: 3393 / Nov  3 2000 14:34:22" }},

    /* unsupported versions */
    { 0x5AB7F8, false, { MM2_BETA_1, 3323, "Angel: 3323 / Jun 29 2000 11:52:28" }},
    { 0x5C18EC, false, { MM2_BETA_2, 3366, "Angel: 3366 / Aug  8 2000 10:08:04" }},

    // PEtite'd Beta 2...
    { 0x6B602D, false, { MM2_BETA_2_PETITE, 3366, "ERROR!" }},

    { 0x5C18FC, false, { MM2_VANILLA, 3390, "Angel: 3390 / Aug 27 2000 16:27:27" }},
    { NULL },
};

class CMidtownMadness2 : public ageGame {
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
};