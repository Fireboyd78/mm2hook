#pragma once
#include "mm2_common.h"

template<class TNode>
using luaClassBinder = CppBindClass<TNode, LuaBinding>;

namespace MM2
{
    // External declarations
    extern class asNode;
    extern class datParser;

    template<class TNode>
    class asLuaNode : public TNode {
        using TBase = TNode;
    public:
        static_assert(std::is_base_of<asNode, TNode>::value, "TNode must inherit from asNode.");

        asLuaNode() {
            PUSH_VTABLE();
            TNode::TNode();
            POP_VTABLE();
        }

       virtual ~asLuaNode() {
            PUSH_VTABLE();
            TNode::~TNode();
            POP_VTABLE();
        }

        virtual void Update(void) {
            TNode::Update();
        };

        virtual void Reset(void) {
            TNode::Reset();
        };

        virtual void ResChange(int width, int height) {
            TNode::ResChange(width, height);
        };

        virtual void UpdatePaused(void) {
            TNode::UpdatePaused();
        };

        virtual void FileIO(datParser &parser) {
            TNode::FileIO(parser);
        };

        virtual void AfterLoad(void) {
            TNode::AfterLoad();
        };

        virtual void BeforeSave(void) {
            TNode::BeforeSave();
        };

        virtual bool Save(void) {
            return TNode::Save();
        };

        virtual bool Load(void) {
            return TNode::Load();
        };

        virtual char * GetClassName(void) {
            return TNode::GetClassName();
        };

        virtual const char * GetDirName(void) {
            return TNode::GetDirName();
        };

        static void RegisterLua(lua_State *L, LPCSTR name) {
            LuaBinding l = LuaBinding(L);
            luaClassBinder<TBase> lc = l.beginClass<TBase>(name);

            TNode::RegisterLua(&lc);

            lc.addFunction("Update", static_cast<void (TBase::*)(void)>(&Update));
            lc.addFunction("Reset", static_cast<void (TBase::*)(void)>(&Reset));
            lc.addFunction("Save", static_cast<bool (TBase::*)(void)>(&Save));
            lc.addFunction("Load", static_cast<bool (TBase::*)(void)>(&Load));
            lc.addFunction("GetClassName", static_cast<char * (TBase::*)(void)>(&GetClassName));
            lc.addFunction("GetDirName", static_cast<const char * (TBase::*)(void)>(&GetDirName));

            lc.endClass();
        }
    };
}

namespace MM2Lua
{
    bool IsLoaded();

    void Initialize();
    void Reset();

    void OnTick();
    void OnRestart();
    void OnShutdown();
    void OnKeyPress(DWORD vKey);

    void SendCommand(LPCSTR command);
}