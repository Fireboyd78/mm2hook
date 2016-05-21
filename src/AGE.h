#pragma once
#include "common.h"

//
// A.G.E. (Angel Game Engine) common includes
//

typedef struct AGEGameInfo
{
    DWORD offset;
    int version;
    short age_version;
    bool isSupported;
    LPCSTR age_string;
} *LPAGEGameInfo;

class CAGEGame {
private:
    int m_engineVersion;
protected:
    virtual int Initialize() {
        return HOOK_INIT_OK;
    };
public:
    CAGEGame(int engineVersion) {
        m_engineVersion = engineVersion;
    };

    CAGEGame(LPAGEGameInfo gameInfo) 
        : CAGEGame(gameInfo->age_version) {
    };

    NOINLINE int GetEngineVersion() const {
        return m_engineVersion;
    };
};