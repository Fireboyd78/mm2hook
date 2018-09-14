#pragma once
#include <modules\banger.h>

#include <mm2_particle.h>

namespace MM2
{
    // Forward delcarations
    class dgBangerData;

    // External declarations
    extern class asNode;
    extern class asBirthRule;

    // Class definitions

    class dgBangerData : public asNode
    {
        uint32_t Index;
        Vector3 vector31C;
        Vector3 vector328;
        int dword34;
        int dword38;
        int dword3C;
        Vector3 *Positions;
        short PositionCount;
        Vector3 dword48;
        int dword54;
        int dword58;
        int dword5C;
        asBirthRule BirthRule;
        int dword114;
        int Flags2;
        short word11C;
        byte byte11E;
        byte byte11F;
        byte Flags;
        byte byte121;
        int dword124;
        short word128;
        int dword12C;
        int dword130;
        int dword134;
        char pad138[28];
    };

    // Lua initialization

}