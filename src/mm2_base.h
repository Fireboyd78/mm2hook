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

    namespace $
    {
        namespace Base
        {
            HOOK_EXPORT(0x4C8200, _MemberFunc<void>, $$dtor);
        }
        namespace asCullable
        {
            HOOK_EXPORT(0x460EA0, _MemberFunc<void>, $$dtor);
            HOOK_EXPORT(0x4A3440, _MemberFunc<void>, Cull);
        }
    }

    class Base {
    public:
        AGE_API virtual ~Base() {
            PUSH_VTABLE();
            $::Base::$$dtor(this);
            POP_VTABLE();
        };
    };

    class asCullable : public Base {
    public:
        AGE_API virtual ~asCullable() {
            PUSH_VTABLE();
            $::asCullable::$$dtor(this);
            POP_VTABLE();
        };

        AGE_API virtual void Cull(void) {
            $::asCullable::Cull(this);
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

        AGE_API virtual ~asNode() {
            PUSH_VTABLE();
            ageHook::Thunk<0x4A0D00>::Call<void>(this);
            POP_VTABLE();
        };

        AGE_API char * GetName() const {
            return (name) ? name : "";
        };

        AGE_API void SetName(LPCSTR name) {
            ageHook::Thunk<0x4A0D70>::Call<void>(this, name);
        };

        AGE_API int AddChild(asNode *child) {
            return ageHook::Thunk<0x4A0E60>::Call<int>(this, child);
        };

        AGE_API int InsertChild(int index, asNode *child) {
            return ageHook::Thunk<0x4A0EE0>::Call<int>(this, index, child);
        };

        AGE_API int RemoveChild(int index) {
            return ageHook::Thunk<0x4A0F50>::Call<int>(this, index);
        };

        AGE_API int RemoveChild(asNode *child) {
            return ageHook::Thunk<0x4A0FD0>::Call<int>(this, child);
        };

        AGE_API void RemoveAllChildren(void) {
            ageHook::Thunk<0x4A1010>::Call<void>(this);
        };

        AGE_API asNode * GetChild(int index) {
            return ageHook::Thunk<0x4A1030>::Call<asNode *>(this);
        };

        AGE_API asNode* GetNext(void) {
            return ageHook::Thunk<0x4A1060>::Call<asNode *>(this);
        };

        AGE_API asNode* GetLastChild(void) {
            return ageHook::Thunk<0x4A1070>::Call<asNode *>(this);
        };

        AGE_API int NumChildren(void) {
            return ageHook::Thunk<0x4A10A0>::Call<int>(this);
        };

        AGE_API void SwitchTo(int index) {
            ageHook::Thunk<0x4A10C0>::Call<void>(this);
        };

        /*
            *** DO NOT CHANGE THE ORDER OF THESE ***
            They must EXACTLY match the virtual tables in MM2!
        */

        AGE_API virtual void Update(void) {
            ageHook::Thunk<0x4A0DB0>::Call<void>(this);
        };

        AGE_API virtual void Reset(void) {
            ageHook::Thunk<0x4A0E10>::Call<void>(this);
        };

        AGE_API virtual void ResChange(int width, int height) {
            ageHook::Thunk<0x4A0E30>::Call<void>(this, width, height);
        };

        AGE_API virtual void UpdatePaused(void) {
            ageHook::Thunk<0x4A0DD0>::Call<void>(this);
        };

        AGE_API virtual void FileIO(datParser &parser) {
            ageHook::Thunk<0x403330>::Call<void>(this, &parser);
        };

        AGE_API virtual void AfterLoad(void) {
            ageHook::Thunk<0x403340>::Call<void>(this);
        };

        AGE_API virtual void BeforeSave(void) {
            ageHook::Thunk<0x403350>::Call<void>(this);
        };

        AGE_API virtual bool Save(void) {
            return ageHook::Thunk<0x4A11D0>::Call<bool>(this);
        };

        AGE_API virtual bool Load(void) {
            return ageHook::Thunk<0x4A1120>::Call<bool>(this);
        };

        AGE_API virtual char * GetClassName(void) {
            return ageHook::Thunk<0x403360>::Call<char *>(this);
        };

        AGE_API virtual const char * GetDirName(void) {
            return ageHook::Thunk<0x4A0DA0>::Call<const char *>(this);
        };

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