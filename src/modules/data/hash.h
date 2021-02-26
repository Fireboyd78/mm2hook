#pragma once
#include <modules\data.h>

namespace MM2
{
    // Forward declarations
    class HashTable;

    // External declarations


    // Class definitions
    class HashTable 
    {
    public:
        AGE_API void* Access(char const* a1)                            { return hook::Thunk<0x4C6CA0>::Call<void*>(this, a1); }
        AGE_API bool Access(char const* a1, void* out)                  { return hook::Thunk<0x4C6D10>::Call<bool>(this, a1, out); }                        
        AGE_API bool Insert(char const* a1, void* data)                 { return hook::Thunk<0x4C69F0>::Call<bool>(this, a1, data); }
    };

    // Lua initialization

}