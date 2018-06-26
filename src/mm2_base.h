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
        static void BindLua(luaClassBinder<TNode> *lc) {
            lc->addProperty("name", &GetName, &SetName);

            lc->addFunction("AddChild", &AddChild);
            lc->addFunction("InsertChild", &InsertChild);
            lc->addFunction("RemoveChild", static_cast<int (TNode::*)(int)>(&RemoveChild));
            lc->addFunction("RemoveAllChildren", &RemoveAllChildren);
            lc->addFunction("GetChild", &GetChild);
            lc->addFunction("GetNext", &GetNext);
            lc->addFunction("GetLastChild", &GetLastChild);
            lc->addFunction("NumChildren", &NumChildren);
            lc->addFunction("SwitchTo", &SwitchTo);

            lc->addFunction("Update", base_t<TNode>::cast(&Update));
            lc->addFunction("Reset", base_t<TNode>::cast(&Reset));
            lc->addFunction("ResChange", base_t<TNode>::cast(&ResChange));
            lc->addFunction("UpdatePaused", base_t<TNode>::cast(&UpdatePaused));
            lc->addFunction("FileIO", base_t<TNode>::cast(&FileIO));
            lc->addFunction("AfterLoad", base_t<TNode>::cast(&AfterLoad));
            lc->addFunction("BeforeSave", base_t<TNode>::cast(&BeforeSave));
            lc->addFunction("Save", base_t<TNode>::cast(&Save));
            lc->addFunction("Load", base_t<TNode>::cast(&Load));
            lc->addFunction("GetClassName", base_t<TNode>::cast(&GetClassName));
            lc->addFunction("GetDirName", base_t<TNode>::cast(&GetDirName));
        };
    };

    class asRoot : public asNode {
    private:
        Matrix34 matrix; // used for asLinearCS::CurrentMatrix

        bool isPaused;
        bool unk_49; // pauses the game if set to true, then turns itself off (unused -- never set by anything)
        bool useNanSignal;
    public:
        AGE_API asRoot(void) {
            PUSH_VTABLE();
            ageHook::Thunk<0x4A0B70>::Call<void>(this);
            POP_VTABLE();
        }

        AGE_API virtual ~asRoot(void) {
            PUSH_VTABLE();
            ageHook::Thunk<0x4A0C90>::Call<void>(this);
            POP_VTABLE();
        }

        inline Matrix34 & getMatrix(void) {
            return matrix;
        }

        inline void setMatrix(Matrix34 &mtx) {
            memcpy(&matrix, &mtx, sizeof(Matrix34));
        }

        AGE_API void Init(bool)                             { ageHook::Thunk<0x4A0BE0>::Call<void>(this);  }
        AGE_API void Quit(void)                             { ageHook::Thunk<0x4A0C50>::Call<void>(this);  }
        AGE_API bool IsPaused(void)                         { return ageHook::Thunk<0x4A0C60>::Call<bool>(this); }
        AGE_API void TogglePause(void)                      { ageHook::Thunk<0x4A0C70>::Call<void>(this); }
        AGE_API void SetPause(bool paused)                  { ageHook::Thunk<0x4A0C80>::Call<void>(this, paused); }

        /*
            asNode virtuals
        */

        virtual AGE_API void Update(void) override          { ageHook::Thunk<0x4A0C00>::Call<void>(this); };
        virtual AGE_API void Reset(void) override           { ageHook::Thunk<0x4A0C40>::Call<void>(this); };
        virtual AGE_API char * GetClassName(void) override  { return ageHook::Thunk<0x4A0CD0>::Call<char *>(this); };
    };

    declhook(0x661738, _TypeProxy<asRoot>, ROOT);

    template<>
    void luaAddModule<module_base>(LuaState L) {
        luaBind<asNode>(L, "asNode");
    }
}