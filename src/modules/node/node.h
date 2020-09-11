#pragma once
#include <modules\node.h>

namespace MM2
{
    // Forward declarations
    class asNode;

    // External declarations
    extern class datParser;

    // Class definitions

    class asNode : public asCullable {
    protected:
        asNode *next;
        asNode *child; // not sure on this one
        asNode *parent;
    
        unsigned int flags; // not sure how these work yet

        char *name;
    public:
        AGE_API asNode() {
            scoped_vtable x(this);
            hook::Thunk<0x4A0CE0>::Call<void>(this);
        };

        virtual AGE_API ~asNode() {
            scoped_vtable x(this);
            hook::Thunk<0x4A0D00>::Call<void>(this);
        };

        inline bool isActive(void) const {
            return (flags & 1);
        };

        inline void setActive(bool active) {
            if (isActive() != active)
                flags ^= 1;
        }

        AGE_API char * GetName() const {
            return (name) ? name : "";
        };

        AGE_API void SetName(LPCSTR name)                   { hook::Thunk<0x4A0D70>::Call<void>(this, name); };
        AGE_API int AddChild(asNode *child)                 { return hook::Thunk<0x4A0E60>::Call<int>(this, child); };
        AGE_API int InsertChild(int index, asNode *child)   { return hook::Thunk<0x4A0EE0>::Call<int>(this, index, child); };
        AGE_API int RemoveChild(int index)                  { return hook::Thunk<0x4A0F50>::Call<int>(this, index); };
        AGE_API int RemoveChild(asNode *child)              { return hook::Thunk<0x4A0FD0>::Call<int>(this, child); };
        AGE_API void RemoveAllChildren(void)                { hook::Thunk<0x4A1010>::Call<void>(this); };
        AGE_API asNode * GetChild(int index)                { return hook::Thunk<0x4A1030>::Call<asNode *>(this); };
        AGE_API asNode* GetNext(void)                       { return hook::Thunk<0x4A1060>::Call<asNode *>(this); };
        AGE_API asNode* GetLastChild(void)                  { return hook::Thunk<0x4A1070>::Call<asNode *>(this); };
        AGE_API int NumChildren(void)                       { return hook::Thunk<0x4A10A0>::Call<int>(this); };
        AGE_API void SwitchTo(int index)                    { hook::Thunk<0x4A10C0>::Call<void>(this); };

        /*
            *** DO NOT CHANGE THE ORDER OF THESE ***
            They must EXACTLY match the virtual tables in MM2!
        */

        virtual AGE_API void Update(void)                   { hook::Thunk<0x4A0DB0>::Call<void>(this); };
        virtual AGE_API void Reset(void)                    { hook::Thunk<0x4A0E10>::Call<void>(this); };
        virtual AGE_API void ResChange(int width, int height)
                                                            { hook::Thunk<0x4A0E30>::Call<void>(this, width, height); };
        virtual AGE_API void UpdatePaused(void)             { hook::Thunk<0x4A0DD0>::Call<void>(this); };
        virtual AGE_API void FileIO(datParser &parser)      { hook::Thunk<0x403330>::Call<void>(this, &parser); };
        virtual AGE_API void AfterLoad(void)                { hook::Thunk<0x403340>::Call<void>(this); };
        virtual AGE_API void BeforeSave(void)               { hook::Thunk<0x403350>::Call<void>(this); };
        virtual AGE_API bool Save(void)                     { return hook::Thunk<0x4A11D0>::Call<bool>(this); };
        virtual AGE_API bool Load(void)                     { return hook::Thunk<0x4A1120>::Call<bool>(this); };
        virtual AGE_API char * GetClassName(void)           { return hook::Thunk<0x403360>::Call<char *>(this); };
        virtual AGE_API const char * GetDirName(void)       { return hook::Thunk<0x4A0DA0>::Call<const char *>(this); };

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<asNode, asCullable>("asNode")
            .addProperty("Name", &GetName, &SetName)
            .addProperty("Active", &isActive, &setActive)

            .addFunction("AddChild", &AddChild)
            .addFunction("InsertChild", &InsertChild)
            .addFunction("RemoveChild", static_cast<int(asNode::*)(asNode* child)>(&RemoveChild))
            .addFunction("RemoveAllChildren", &RemoveAllChildren)
            .addFunction("GetChild", &GetChild)
            .addFunction("GetNext", &GetNext)
            .addFunction("GetLastChild", &GetLastChild)
            .addFunction("NumChildren", &NumChildren)
            .addFunction("SwitchTo", &SwitchTo)

            .addFunction("Update", &Update)
            .addFunction("Reset", &Reset)
            .addFunction("ResChange", &ResChange)
            .addFunction("UpdatePaused",&UpdatePaused)
            .addFunction("FileIO", &FileIO)
            .addFunction("AfterLoad",  &AfterLoad)
            .addFunction("BeforeSave", &BeforeSave)
            .addFunction("Save", &Save)
            .addFunction("Load", &Load)
            .addFunction("GetClassName", &GetClassName)
            .addFunction("GetDirName", &GetDirName)
            .endClass();
        }
    };

    ASSERT_SIZEOF(asNode, 0x18);

    // Lua initialization

}