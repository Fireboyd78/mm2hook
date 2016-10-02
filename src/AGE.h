#pragma once
#include "common.h"

//
// A.G.E. (Angel Game Engine) common includes
//

struct ageInfo
{
    short gameVersion;
    short engineVersion;
    LPCSTR versionString;
};

struct ageInfoLookup {
    DWORD offset;
    bool isSupported;

    ageInfo info;
};

class ageGame {
protected:
    ageInfo m_info;
public:
    ageGame(const ageInfo &gameInfo) : m_info(gameInfo) {};

    ageGame(short gameVersion, short engineVersion)
        : m_info { gameVersion, engineVersion } {};
    ageGame(short gameVersion, short engineVersion, LPCSTR versionString)
        : m_info { gameVersion, engineVersion, versionString } {};

    static bool GetAGEInfo(const ageInfoLookup *infoLookup, ageInfoLookup &outInfo) {
        const ageInfoLookup *l = NULL;
        for (l = infoLookup; l->offset != 0; l++) {
            if (strcmp((LPCSTR)l->offset, l->info.versionString) == 0)
            {
                outInfo = *l;
                return true;
            }
        }
        return false;
    };

    virtual int GetGameVersion() const {
        return m_info.gameVersion;
    };

    virtual int GetEngineVersion() const {
        return m_info.engineVersion;
    };

    virtual HWND GetMainWindowHwnd() const PURE;

    virtual void Initialize() {};
};