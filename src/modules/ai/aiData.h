#pragma once
#include <modules\ai.h>

namespace MM2
{
    // Forward declarations
    class aiRaceData;

    // External declarations


    // Class definitions

    class aiRaceData {
    private:
        hook::Field<0x98, float> _copChaseDistance;
        byte _buffer[0xC0];
    public:
        aiRaceData(void)                                    DONOTCALL;
        aiRaceData(const aiRaceData &&)                     DONOTCALL;

        virtual ~aiRaceData(void) DONOTCALL;

        inline float getCopChaseDistance()
        {
            return _copChaseDistance.get(this);
        }
    };

    ASSERT_SIZEOF(aiRaceData, 0xC8);

    // Lua initialization

}