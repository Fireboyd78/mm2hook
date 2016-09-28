#pragma once
#include "mm2.h"

namespace MM2
{
    // Forward declarations
    extern class Base;

    class Base {};

    // TODO: Implement full class support; add forward declaration
    // It's very important that the virtual methods match the virtual table layout EXACTLY
    // Otherwise we're not going to be able to do anything cool with it :(
#ifdef ASNODE_IMPL
    class asNode : public Base {
    protected:
        static MM2FnHook<void> $$ctor;
        static MM2FnHook<void> $$dtor;

    public:
        virtual void Cull(void);
        virtual void Update(void);
        virtual void Reset(void);
        virtual void ResChange(int, int);
        virtual void UpdatePaused(void);
        virtual void FileIO(datParser &);
        virtual void AfterLoad(void);
        virtual void BeforeSave(void);
        virtual bool Save(void);
        virtual bool Load(void);
        virtual char * GetClassName(void);
        virtual const char * GetDirName(void);
    };
#endif
}