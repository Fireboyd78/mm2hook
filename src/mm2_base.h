#pragma once
#include "mm2_common.h"
#include "mm2_lua.h"

namespace MM2
{
    // Forward declarations
    class Base;
    class asCullable;
    class asNode;

    // External declarations
    extern class datParser;

    class Base {
    public:
        virtual AGE_API ~Base() {
            PUSH_VTABLE();
            ageHook::Thunk<0x4C8200>::Call<void>(this);
            POP_VTABLE();
        };
    };

    class asCullable : public Base {
    public:
        virtual AGE_API ~asCullable() {
            PUSH_VTABLE();
            ageHook::Thunk<0x460EA0>::Call<void>(this);
            POP_VTABLE();
        };

        virtual AGE_API void Cull(void) {
            ageHook::Thunk<0x4A3440>::Call<void>(this);
        };
    };

    class asNode : public asCullable {
        asNode *next;
        asNode *child; // not sure on this one
        asNode *parent;

        unsigned int flags; // not sure how these work yet

        char *name;
    public:
        AGE_API asNode() {
            PUSH_VTABLE();
            ageHook::Thunk<0x4A0CE0>::Call<void>(this);
            POP_VTABLE();
        };

        virtual AGE_API ~asNode() {
            PUSH_VTABLE();
            ageHook::Thunk<0x4A0D00>::Call<void>(this);
            POP_VTABLE();
        };

        AGE_API char * GetName() const {
            return (name) ? name : "";
        };

        AGE_API void SetName(LPCSTR name)                   { ageHook::Thunk<0x4A0D70>::Call<void>(this, name); };
        AGE_API int AddChild(asNode *child)                 { return ageHook::Thunk<0x4A0E60>::Call<int>(this, child); };
        AGE_API int InsertChild(int index, asNode *child)   { return ageHook::Thunk<0x4A0EE0>::Call<int>(this, index, child); };
        AGE_API int RemoveChild(int index)                  { return ageHook::Thunk<0x4A0F50>::Call<int>(this, index); };
        AGE_API int RemoveChild(asNode *child)              { return ageHook::Thunk<0x4A0FD0>::Call<int>(this, child); };
        AGE_API void RemoveAllChildren(void)                { ageHook::Thunk<0x4A1010>::Call<void>(this); };
        AGE_API asNode * GetChild(int index)                { return ageHook::Thunk<0x4A1030>::Call<asNode *>(this); };
        AGE_API asNode* GetNext(void)                       { return ageHook::Thunk<0x4A1060>::Call<asNode *>(this); };
        AGE_API asNode* GetLastChild(void)                  { return ageHook::Thunk<0x4A1070>::Call<asNode *>(this); };
        AGE_API int NumChildren(void)                       { return ageHook::Thunk<0x4A10A0>::Call<int>(this); };
        AGE_API void SwitchTo(int index)                    { ageHook::Thunk<0x4A10C0>::Call<void>(this); };

        /*
            *** DO NOT CHANGE THE ORDER OF THESE ***
            They must EXACTLY match the virtual tables in MM2!
        */

        virtual AGE_API void Update(void)                   { ageHook::Thunk<0x4A0DB0>::Call<void>(this); };
        virtual AGE_API void Reset(void)                    { ageHook::Thunk<0x4A0E10>::Call<void>(this); };
        virtual AGE_API void ResChange(int width, int height)
                                                            { ageHook::Thunk<0x4A0E30>::Call<void>(this, width, height); };
        virtual AGE_API void UpdatePaused(void)             { ageHook::Thunk<0x4A0DD0>::Call<void>(this); };
        virtual AGE_API void FileIO(datParser &parser)      { ageHook::Thunk<0x403330>::Call<void>(this, &parser); };
        virtual AGE_API void AfterLoad(void)                { ageHook::Thunk<0x403340>::Call<void>(this); };
        virtual AGE_API void BeforeSave(void)               { ageHook::Thunk<0x403350>::Call<void>(this); };
        virtual AGE_API bool Save(void)                     { return ageHook::Thunk<0x4A11D0>::Call<bool>(this); };
        virtual AGE_API bool Load(void)                     { return ageHook::Thunk<0x4A1120>::Call<bool>(this); };
        virtual AGE_API char * GetClassName(void)           { return ageHook::Thunk<0x403360>::Call<char *>(this); };
        virtual AGE_API const char * GetDirName(void)       { return ageHook::Thunk<0x4A0DA0>::Call<const char *>(this); };

        template <class TNode>
        static void RegisterLua(luaClassBinder<TNode> *lc) {
            lc->addFunction("AddChild", &AddChild);
            lc->addFunction("InsertChild", &InsertChild);
            lc->addFunction("RemoveChild", static_cast<int (TNode::*)(int)>(&RemoveChild));
            lc->addFunction("RemoveAllChildren", &RemoveAllChildren);
            lc->addFunction("GetChild", &GetChild);
            lc->addFunction("GetNext", &GetNext);
            lc->addFunction("GetLastChild", &GetLastChild);
            lc->addFunction("NumChildren", &NumChildren);
            lc->addFunction("SwitchTo", &SwitchTo);

            lc->addProperty("name", &GetName, &SetName);
        };
    };
}