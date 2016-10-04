#pragma once
#include "mm2_common.h"

namespace MM2
{
    // Forward declarations
    extern class Base;
    extern class asCullable;
    extern class asNode;

    // External declarations
    extern class datParser;

    class Base {
    protected:
        static MM2FnHook<void> $$dtor;
    public:
        virtual ~Base() {
            PUSH_VTABLE();
            $$dtor(this);
            POP_VTABLE();
        };
    };

    class asCullable : public Base {
    protected:
        static MM2FnHook<void> $$dtor;
        static MM2FnHook<void> $Cull;
    public:
        virtual ~asCullable() {
            PUSH_VTABLE();
            $$dtor(this);
            POP_VTABLE();
        };

        virtual void Cull(void) {
            $Cull(this);
        };
    };

    class asNode : public asCullable {
        asNode *next;
        asNode *child; // not sure on this one
        asNode *parent;

        unsigned int flags; // not sure how these work yet

        LPCSTR name;
    protected:
        static MM2FnHook<void> $FileIO;
        static MM2FnHook<void> $AfterLoad;
        static MM2FnHook<void> $BeforeSave;

        static MM2FnHook<char *> $GetClassNameA;

        static MM2FnHook<void> $$ctor;
        static MM2FnHook<void> $$dtor;

        static MM2FnHook<void> $SetName;

        static MM2FnHook<const char *> $GetDirName;

        static MM2FnHook<void> $Update;
        static MM2FnHook<void> $UpdatePaused;
        static MM2FnHook<void> $Reset;
        static MM2FnHook<void> $ResChange;

        static MM2FnHook<int> $AddChild;
        static MM2FnHook<int> $InsertChild;
        static MM2FnHook<int> $RemoveChild_$1;
        static MM2FnHook<int> $RemoveChild_$2;
        static MM2FnHook<void> $RemoveAllChildren;
        static MM2FnHook<asNode *> $GetChild;
        static MM2FnHook<asNode *> $GetNext;
        static MM2FnHook<asNode *> $GetLastChild;
        static MM2FnHook<int> $NumChildren;
        static MM2FnHook<void> $SwitchTo;

        static MM2FnHook<bool> $Load;
        static MM2FnHook<bool> $Save;
    public:
        AGE_API asNode() {
            PUSH_VTABLE();
            $$ctor(this);
            POP_VTABLE();
        };

        AGE_API virtual ~asNode() {
            PUSH_VTABLE();
            $$dtor(this);
            POP_VTABLE();
        };

        AGE_API void SetName(LPCSTR name) {
            $SetName(this, name);
        };

        AGE_API int AddChild(asNode *child) {
            return $AddChild(this, child);
        };

        AGE_API int InsertChild(int index, asNode *child) {
            return $InsertChild(this, index, child);
        };

        AGE_API int RemoveChild(int index) {
            return $RemoveChild_$1(this, index);
        };

        AGE_API int RemoveChild(asNode *child) {
            return $RemoveChild_$2(this, child);
        };

        AGE_API void RemoveAllChildren(void) {
            $RemoveAllChildren(this);
        };

        AGE_API asNode * GetChild(int index) {
            return $GetChild(this, index);
        };

        AGE_API asNode* GetNext(void) {
            return $GetNext(this);
        };

        AGE_API asNode* GetLastChild(void) {
            return $GetLastChild(this);
        };

        AGE_API int NumChildren(void) {
            return $NumChildren(this);
        };

        AGE_API void SwitchTo(int index) {
            $SwitchTo(this, index);
        };

        /*
            *** DO NOT CHANGE THE ORDER OF THESE ***
            They must EXACTLY match the virtual tables in MM2!
        */

        AGE_API virtual void Update(void) {
            $Update(this);
        };

        AGE_API virtual void Reset(void) {
            $Reset(this);
        };

        AGE_API virtual void ResChange(int width, int height) {
            $ResChange(this, width, height);
        };

        AGE_API virtual void UpdatePaused(void) {
            $UpdatePaused(this);
        };

        AGE_API virtual void FileIO(datParser &parser) {
            $FileIO(this, &parser);
        };

        AGE_API virtual void AfterLoad(void) {
            $AfterLoad(this);
        };

        AGE_API virtual void BeforeSave(void) {
            $BeforeSave(this);
        };

        AGE_API virtual bool Save(void) {
            return $Save(this);
        };

        AGE_API virtual bool Load(void) {
            return $Load(this);
        };

        AGE_API virtual char * GetClassName(void) {
            return $GetClassNameA(this);
        };

        AGE_API virtual const char * GetDirName(void) {
            return $GetDirName(this);
        };
    };
}