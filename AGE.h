#pragma once
#include "common.h"

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
    NOINLINE virtual void Initialize() {
        return;
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