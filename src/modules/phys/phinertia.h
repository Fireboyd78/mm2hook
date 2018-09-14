#pragma once
#include <modules\phys.h>

namespace MM2
{
    // Forward delcarations
    class phInertialCS;

    // External declarations


    // Class definitions

    class phInertialCS {
    protected:
        ageHook::Field<0x54, Matrix34> _matrix;
    public:
        inline Matrix34 * getMatrix(void) const {
            return _matrix.ptr(this);
        }
    };


    // Lua initialization

}