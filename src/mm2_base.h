#pragma once
#include "mm2_common.h"
#include "mm2_lua.h"

namespace MM2
{
    // Forward declarations
    extern class Base;
    extern class asCullable;
    extern class asNode;

    // External declarations
    extern class datParser;

    namespace $
    {
        namespace Base
        {
            HOOK_API AGEHook<0x4C8200>::MemberFunc<void> $$dtor;
        }
        namespace asCullable
        {
            HOOK_API AGEHook<0x460EA0>::MemberFunc<void> $$dtor;
            HOOK_API AGEHook<0x4A3440>::MemberFunc<void> Cull;
        }
        namespace asNode
        {
            HOOK_API AGEHook<0x4A0CE0>::MemberFunc<void> $$ctor;
            HOOK_API AGEHook<0x4A0D00>::MemberFunc<void> $$dtor;

            HOOK_API AGEHook<0x403330>::MemberFunc<void> FileIO;
            HOOK_API AGEHook<0x403340>::MemberFunc<void> AfterLoad;
            HOOK_API AGEHook<0x403350>::MemberFunc<void> BeforeSave;

            HOOK_API AGEHook<0x403360>::MemberFunc<char *> GetClassNameA;

            HOOK_API AGEHook<0x4A0D70>::MemberFunc<void> SetName;

            HOOK_API AGEHook<0x4A0DA0>::MemberFunc<const char *> GetDirName;

            HOOK_API AGEHook<0x4A0DB0>::MemberFunc<void> Update;
            HOOK_API AGEHook<0x4A0DD0>::MemberFunc<void> UpdatePaused;
            HOOK_API AGEHook<0x4A0E10>::MemberFunc<void> Reset;
            HOOK_API AGEHook<0x4A0E30>::MemberFunc<void> ResChange;

            HOOK_API AGEHook<0x4A0E60>::MemberFunc<int> AddChild;
            HOOK_API AGEHook<0x4A0EE0>::MemberFunc<int> InsertChild;
            HOOK_API AGEHook<0x4A0F50>::MemberFunc<int> RemoveChild_$1;
            HOOK_API AGEHook<0x4A0FD0>::MemberFunc<int> RemoveChild_$2;
            HOOK_API AGEHook<0x4A1010>::MemberFunc<void> RemoveAllChildren;
            HOOK_API AGEHook<0x4A1030>::MemberFunc<MM2::asNode *> GetChild;
            HOOK_API AGEHook<0x4A1060>::MemberFunc<MM2::asNode *> GetNext;
            HOOK_API AGEHook<0x4A1070>::MemberFunc<MM2::asNode *> GetLastChild;
            HOOK_API AGEHook<0x4A10A0>::MemberFunc<int> NumChildren;
            HOOK_API AGEHook<0x4A10C0>::MemberFunc<void> SwitchTo;

            HOOK_API AGEHook<0x4A1120>::MemberFunc<bool> Load;
            HOOK_API AGEHook<0x4A11D0>::MemberFunc<bool> Save;
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
            $::asNode::$$ctor(this);
            POP_VTABLE();
        };

        AGE_API virtual ~asNode() {
            PUSH_VTABLE();
            $::asNode::$$dtor(this);
            POP_VTABLE();
        };

        AGE_API char * GetName() const {
            return (name) ? name : "";
        };

        AGE_API void SetName(LPCSTR name) {
            $::asNode::SetName(this, name);
        };

        AGE_API int AddChild(asNode *child) {
            return $::asNode::AddChild(this, child);
        };

        AGE_API int InsertChild(int index, asNode *child) {
            return $::asNode::InsertChild(this, index, child);
        };

        AGE_API int RemoveChild(int index) {
            return $::asNode::RemoveChild_$1(this, index);
        };

        AGE_API int RemoveChild(asNode *child) {
            return $::asNode::RemoveChild_$2(this, child);
        };

        AGE_API void RemoveAllChildren(void) {
            $::asNode::RemoveAllChildren(this);
        };

        AGE_API asNode * GetChild(int index) {
            return $::asNode::GetChild(this, index);
        };

        AGE_API asNode* GetNext(void) {
            return $::asNode::GetNext(this);
        };

        AGE_API asNode* GetLastChild(void) {
            return $::asNode::GetLastChild(this);
        };

        AGE_API int NumChildren(void) {
            return $::asNode::NumChildren(this);
        };

        AGE_API void SwitchTo(int index) {
            $::asNode::SwitchTo(this, index);
        };

        /*
            *** DO NOT CHANGE THE ORDER OF THESE ***
            They must EXACTLY match the virtual tables in MM2!
        */

        AGE_API virtual void Update(void) {
            $::asNode::Update(this);
        };

        AGE_API virtual void Reset(void) {
            $::asNode::Reset(this);
        };

        AGE_API virtual void ResChange(int width, int height) {
            $::asNode::ResChange(this, width, height);
        };

        AGE_API virtual void UpdatePaused(void) {
            $::asNode::UpdatePaused(this);
        };

        AGE_API virtual void FileIO(datParser &parser) {
            $::asNode::FileIO(this, &parser);
        };

        AGE_API virtual void AfterLoad(void) {
            $::asNode::AfterLoad(this);
        };

        AGE_API virtual void BeforeSave(void) {
            $::asNode::BeforeSave(this);
        };

        AGE_API virtual bool Save(void) {
            return $::asNode::Save(this);
        };

        AGE_API virtual bool Load(void) {
            return $::asNode::Load(this);
        };

        AGE_API virtual char * GetClassName(void) {
            return $::asNode::GetClassNameA(this);
        };

        AGE_API virtual const char * GetDirName(void) {
            return $::asNode::GetDirName(this);
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