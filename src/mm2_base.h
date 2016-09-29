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
    private:
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
        asNode() {
            PUSH_VTABLE();
            $$ctor(this);
            POP_VTABLE();
        };

        virtual ~asNode() {
            PUSH_VTABLE();
            $$dtor(this);
            POP_VTABLE();
        };

        int AddChild(asNode *child) {
            return $AddChild(this, child);
        };

        int InsertChild(int index, asNode *child) {
            return $InsertChild(this, index, child);
        };

        int RemoveChild(int index) {
            return $RemoveChild_$1(this, index);
        };

        int RemoveChild(asNode *child) {
            return $RemoveChild_$2(this, child);
        };

        void RemoveAllChildren(void) {
            $RemoveAllChildren(this);
        };

        asNode * GetChild(int index) {
            return $GetChild(this, index);
        };

        asNode * GetNext(void) {
            return $GetNext(this);
        };

        asNode * GetLastChild(void) {
            return $GetLastChild(this);
        };

        int NumChildren(void) {
            return $NumChildren(this);
        };

        void SwitchTo(int index) {
            $SwitchTo(this, index);
        };

        /*
            *** DO NOT CHANGE THE ORDER OF THESE ***
            They must EXACTLY match the virtual tables in MM2!
        */

        virtual void Update(void) {
            $Update(this);
        };

        virtual void Reset(void) {
            $Reset(this);
        };

        virtual void ResChange(int width, int height) {
            $ResChange(this, width, height);
        };

        virtual void UpdatePaused(void) {
            $UpdatePaused(this);
        };

        virtual void FileIO(datParser &parser) {
            $FileIO(this, &parser);
        };

        virtual void AfterLoad(void) {
            $AfterLoad(this);
        };

        virtual void BeforeSave(void) {
            $BeforeSave(this);
        };

        virtual bool Save(void) {
            return $Save(this);
        };

        virtual bool Load(void) {
            return $Load(this);
        };

        virtual char * GetClassName(void) {
            return $GetClassNameA(this);
        };

        virtual const char * GetDirName(void) {
            return $GetDirName(this);
        };
    };
}