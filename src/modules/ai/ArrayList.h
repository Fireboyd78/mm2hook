#pragma once
#include <modules\ai.h>

namespace MM2
{
    // Forward delcarations
    class CArrayList;

    // External declarations


    // Class definitions

    class CArrayList {
    private:
        short count;
        int *data;
    public:
        CArrayList(void) {
            count = 0;
            data = nullptr;
        }

        ~CArrayList(void) {
            count = 0;
            data = nullptr;
        }

        inline int Count() const {
            return count;
        }

        inline int Get(int index) {
            return data[index];
        }

        AGE_API void Append(int value)                      { ageHook::Thunk<0x543D50>::Call<void>(this, value); }

        AGE_API void ReadBinary(Stream *stream)             { ageHook::Thunk<0x543DE0>::Call<void>(this, stream); }
        AGE_API void SaveBinary(Stream *stream)             { ageHook::Thunk<0x543DB0>::Call<void>(this, stream); }
    };

    // Lua initialization

}