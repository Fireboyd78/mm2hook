#pragma once
#include <modules\data.h>

namespace MM2
{
    // Forward declarations
    class datRefCount;

    // External declarations


    // Class definitions
    class datRefCount {
        int refCount;
    public:
        datRefCount()                               { scoped_vtable x(this); hook::Thunk<0x59AAD0>::Call<void>(this); }
        virtual ~datRefCount()                      { scoped_vtable x(this); hook::Thunk<0x59AAE0>::Call<void>(this); }

        int IncRef()                                { return hook::Thunk<0x59AAF0>::Call<int>(this); }
        int DecRef()                                { return hook::Thunk<0x59AB00>::Call<int>(this); }
    };
    
    ASSERT_SIZEOF(datRefCount, 0x8);

    // Lua initialization

}