#pragma once
#include <modules\level.h>

namespace MM2
{
    // Forward declarations
    class dgPath;
    class dgPathSet;

    // External declarations


    // Class definitions
    enum dgPathType : __int8 {
        SINGLE_POINTS = 0x0,
        DIRECTED_POINTS = 0x1,
        LINE_STRIP = 0x2,
    };

    struct dgPathPoint {
    public:
        dgPathPoint() {

        }

        dgPathPoint(float x, float y, float z) {
            this->x = x;
            this->y = y;
            this->z = z;
        }

        float x;
        float y;
        float z;
        int unk;

        
        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<dgPathPoint>("dgPathPoint")
                .addConstructor(LUA_ARGS(float, float, float))
                .addVariableRef("x", &dgPathPoint::x)
                .addVariableRef("y", &dgPathPoint::y)
                .addVariableRef("z", &dgPathPoint::z)
                .addVariableRef("unk", &dgPathPoint::unk)
                .endClass();
        }
    };

    class dgPath {
    private:
        char Name[32];
        dgPathPoint* Points;
        int NumPoints;
        int NumPoints2;
        int Unk1;
        dgPathType Type;
        float Spacing;
    private:
        //lua helpers
        inline LPCSTR getName()
        {
            return (LPCSTR)this->Name;
        }

        inline int getNumPoints() {
            return NumPoints;
        }

        inline int getType() {
            return Type;
        }

        inline void setType(int type) {
            Type = (dgPathType)type;
        }

        inline float getSpacing() {
            return Spacing;
        }

        inline void setSpacing(float spacing) {
            Spacing = spacing;
        }

        //enumeration helpers
        static LuaRef sm_LuaCurrentCallback;
        static bool sm_LuaCallbackSet;

        static void luaEnumerateCallback(const LPCSTR name, const Matrix34* matrix, bool a3) {
            sm_LuaCurrentCallback.call(name, matrix, a3);
        }

        void luaEnumerate(LuaRef callback, float a3)
        {
            assert(!sm_LuaCallbackSet);
            if (callback.type() != LuaTypeID::FUNCTION) {
                Errorf("dgPath.luaEnumerate must take in a lua function as the first argument.");
                return;
            }

            sm_LuaCurrentCallback = callback;
            sm_LuaCallbackSet = true;
            this->Enumerate(luaEnumerateCallback, a3, 0.f);
            sm_LuaCallbackSet = false;
            sm_LuaCurrentCallback = nullptr;
        }
    public:
        //general api
        void setPointCount(int count) {
            if (this->Points)
                ageHook::StaticThunk<0x577380>::Call<void>(this->Points); //angel delete operator
            this->Points = (dgPathPoint*)ageHook::StaticThunk<0x577360>::Call<void*>(sizeof(dgPathPoint) * count); //angel new operator
            this->NumPoints = count;
            this->NumPoints2 = count;
        }

        dgPathPoint* getPoint(int num) {
            if (num >= this->NumPoints)
                return nullptr;
            return &this->Points[num];
        }

        /*
            dgPath
        */
        AGE_API dgPath(LPCSTR name)                         { ageHook::Thunk<0x466B40>::Call<void>(this, name); }
        AGE_API ~dgPath()                                   { ageHook::Thunk<0x466BC0>::Call<void>(this); }

        static dgPath* Load(Stream *stream)                 { return ageHook::StaticThunk<0x466BD0>::Call<dgPath*>(stream); }
        void SetName(LPCSTR name)                           { ageHook::Thunk<0x466B80>::Call<void>(this, name); }
        void Enumerate(void(* a2)(LPCSTR const, Matrix34 const*, bool), float a3, float a4)
                                                            { ageHook::Thunk<0x466D40>::Call<void>(this, a2, a3, a4); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<dgPath>("dgPath")
                .addConstructor(LUA_ARGS(LPCSTR))

                .addStaticFunction("Load", &Load)

                .addProperty("Name", &getName, &SetName)
                .addProperty("Spacing", &getSpacing, &setSpacing)
                .addProperty("Type", &getType, &setType)
                .addPropertyReadOnly("NumPoints", &getNumPoints)

                .addFunction("Enumerate", &luaEnumerate)
                .addFunction("GetPoint", &getPoint)
                .addFunction("SetPointCount", &setPointCount)
                

                .endClass();
        }
    };
    
    class dgPathSet {
    private:
        dgPath* Paths[192];
        int PathCount;
        int unk0;
        int unk1;
    private:
        //lua helpers
        inline int getPathCount() {
            return PathCount;
        }
    public:
        //general api
        dgPath* getPath(int path) {
            if (path >= this->PathCount || path >= 192)
                return nullptr;
            return Paths[path];
        }

        void addPath(dgPath* path) {
            if (this->PathCount >= 192) {
                Errorf("dgPthSet.addPath: No more room!");
                return;
            }

            this->Paths[this->PathCount] = path;
            this->PathCount++;
        }
        
        /*
            dgPathSet
        */
        AGE_API dgPathSet()                         
        {
            scoped_vtable x(this);
            ageHook::Thunk<0x467150>::Call<void>(this); 
        }

        AGE_API virtual ~dgPathSet()
        {
            scoped_vtable x(this);
            ageHook::Thunk<0x467170>::Call<void>(this); 
        }

        AGE_API bool Load(LPCSTR dirName, LPCSTR pathsetName)
                                                            { return ageHook::Thunk<0x467210>::Call<bool>(this, dirName, pathsetName); }
        AGE_API void Kill()                                 { return ageHook::Thunk<0x4671C0>::Call<void>(this); }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<dgPathSet>("dgPathSet")
                .addConstructor(LUA_ARGS())

                .addPropertyReadOnly("NumPaths", &getPathCount)
                .addFunction("GetPath", &getPath)
                .addFunction("AddPath", &addPath)
                .addFunction("Load", &Load)
            
                .endClass();
        }
    };

    ASSERT_SIZEOF(dgPath, 0x38);
    ASSERT_SIZEOF(dgPathSet, 0x310);

    // Lua initialization

}