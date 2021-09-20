#pragma once
#include <modules\node.h>

namespace MM2
{
    // Forward declarations
    class asFileIO;

    // External declarations


    // Class definitions

    class asFileIO : public Base
    {
    private:
        char* name;
    public:
        AGE_API asFileIO() {
            scoped_vtable x(this);
            hook::Thunk<0x597790>::Call<void>(this);
        };

        virtual AGE_API ~asFileIO() {
            scoped_vtable x(this);
            hook::Thunk<0x5977A0>::Call<void>(this);
        };

        AGE_API char * GetName() const {
            return (name) ? name : "";
        };

        AGE_API void SetName(LPCSTR name)                   { hook::Thunk<0x597940>::Call<void>(this, name); };

        /*
            *** DO NOT CHANGE THE ORDER OF THESE ***
            They must EXACTLY match the virtual tables in MM2!
        */

        virtual AGE_API void FileIO(datParser& parser)      { hook::Thunk<0x5979B0>::Call<void>(this, &parser); };
        virtual AGE_API void AfterLoad(void)                { hook::Thunk<0x595AE0>::Call<void>(this); };
        virtual AGE_API void BeforeSave(void)               { hook::Thunk<0x595AF0>::Call<void>(this); };
        virtual AGE_API bool Save(void)                     { return hook::Thunk<0x597800>::Call<bool>(this); };
        virtual AGE_API bool Load(void)                     { return hook::Thunk<0x597890>::Call<bool>(this); };
        virtual AGE_API const char * GetDirName(void)       { return hook::Thunk<0x597970>::Call<const char*>(this); };
        virtual AGE_API char * GetClassName(void)           { return hook::Thunk<0x5979C0>::Call<char*>(this); };

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<asFileIO, Base>("asFileIO")
                .endClass();
        }
    };

    // Lua initialization

}