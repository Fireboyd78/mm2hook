#pragma once
#include <modules\node.h>

namespace MM2
{
    // Forward declarations
    class luaNode;

    // External declarations
    extern class datParser;
    extern class asCullManager;

    // Class definitions
    class luaNode : public asNode {
    private:
        LPCSTR m_ClassName;
        LuaRef m_UpdateFunction;
        LuaRef m_UpdatePausedFunction;
        LuaRef m_ResetFunction;
        LuaRef m_ResChangeFunction;
        LuaRef m_CullFunction;
        bool m_AutoDeclareCullbale;
    private:
        static bool CheckFunctionAndWarn(LuaRef ref, LPCSTR functionName) {
            bool retVal = ref.isValid() && ref.isFunction();
            if (!retVal) {
                Warningf("%s must recieve a valid lua function as it's parameter.", functionName);
            }
            return retVal;
        }
    public:
        ANGEL_ALLOCATOR

        luaNode(LPCSTR name) {
            m_ClassName = "luaNode";
            this->SetName(name);
            this->m_AutoDeclareCullbale = false;
        }

        //lua setters
        void SetUpdateFunction(LuaRef function) {
            if (!CheckFunctionAndWarn(function, "SetUpdateFunction"))
                return;
            m_UpdateFunction = function;
        }

        void SetUpdatePausedFunction(LuaRef function) {
            if (!CheckFunctionAndWarn(function, "SetUpdatePausedFunction"))
                return;
            m_UpdatePausedFunction = function;
        }

        void SetResetFunction(LuaRef function) {
            if (!CheckFunctionAndWarn(function, "SetResetFunction"))
                return;
            m_ResetFunction = function;
        }

        void SetResChangeFunction(LuaRef function) {
            if (!CheckFunctionAndWarn(function, "SetResChangeFunction"))
                return;
            m_ResChangeFunction = function;
        }

        void SetCullFunction(LuaRef function) {
            if (!CheckFunctionAndWarn(function, "SetCullFunction"))
                return;
            m_CullFunction = function;
        }

        //overrides
        char* GetClassName() override {
            return (char*)m_ClassName;
        }

        void Update() override {
            if (m_UpdateFunction.isValid() && m_UpdateFunction.isFunction())
                m_UpdateFunction();
            //if (m_AutoDeclareCullbale)
                //asCullManager::Instance->DeclareCullable(this);
            asNode::Update();
        }

        void UpdatePaused() override {
            if (m_UpdatePausedFunction.isValid() && m_UpdatePausedFunction.isFunction())
                m_UpdatePausedFunction();
            asNode::UpdatePaused();
        }

        void Reset() override {
            if (m_ResetFunction.isValid() && m_ResetFunction.isFunction())
                m_ResetFunction();
            asNode::Reset();
        }

        void ResChange(int width, int height) override {
            if (m_ResChangeFunction.isValid() && m_ResChangeFunction.isFunction())
                m_ResChangeFunction(width, height);
            asNode::ResChange(width, height);
        }

        void Cull() override {
            if (m_CullFunction.isValid() && m_CullFunction.isFunction())
                m_CullFunction();
            //don't call base here because base does nothing
        }

        //lua
        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<luaNode, asNode>("luaNode")
                .addConstructor(LUA_ARGS(LPCSTR))
                .addVariableRef("AutoDeclareCullable", &luaNode::m_AutoDeclareCullbale)
                .addFunction("SetUpdateFunction", &SetUpdateFunction)
                .addFunction("SetUpdatePausedFunction", &SetUpdatePausedFunction)
                .addFunction("SetResetFunction", &SetResetFunction)
                .addFunction("SetResChangeFunction", &SetResChangeFunction)
                .addFunction("SetCullFunction", &SetCullFunction)
                .endClass();
        }
    };
}