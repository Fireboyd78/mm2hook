#include "mm2.h"

namespace {
    MM2Version EngineVersionToGameVersion(int engineVersion) {
        switch (engineVersion) {
            case 3323:
                return MM2_BETA_1;
            case 3366:
                return MM2_BETA_2;
            case 3393:
                return MM2_RETAIL;
        }
        return MM2_INVALID;
    }
}

CMidtownMadness2::CMidtownMadness2(int engineVersion)
    : CAGEGame(engineVersion) {
    this->m_gameVersion = EngineVersionToGameVersion(engineVersion);
    this->Initialize();
}

CMidtownMadness2::CMidtownMadness2(LPAGEGameInfo gameEntry)
    : CMidtownMadness2(gameEntry->age_version) {
}
