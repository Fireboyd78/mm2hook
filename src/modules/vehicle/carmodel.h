#pragma once
#include <modules\vehicle.h>
#include <mm2_gfx.h>

#include "car.h"
#include "breakable.h"

namespace MM2
{
    // Forward declarations
    class vehCarModel;

    // External declarations
    extern class dgPhysEntity;
    extern class lvlInstance;

    // Class definitions

    class vehCarModel : public lvlInstance {
    protected:
        hook::Field<0xAC, vehBreakableMgr *> _mechanicalBreakableMgr;
        hook::Field<0xA0, vehBreakableMgr *> _genericBreakableMgr;
        hook::Field<0x18, vehCar *> _car;
        hook::Field<0x24, byte> _variant;
    public:
        AGE_API vehCarModel() {
            scoped_vtable x(this);
            hook::Thunk<0x4CCF20>::Call<void>(this);
        }

        AGE_API ~vehCarModel() {
            scoped_vtable x(this);
            hook::Thunk<0x4CCF800>::Call<void>(this);
        }

        inline vehBreakableMgr * getGenBreakableMgr(void) const {
            return _genericBreakableMgr.get(this);
        }
        
        inline vehBreakableMgr * getMechBreakableMgr(void) const {
            return _mechanicalBreakableMgr.get(this);
        }

        inline vehCar * getCar(void) const {
            return _car.get(this);
        }

        inline int getVariant(void) const {
            return _variant.get(this);
        }

        inline void setVariant(int variant) {
            this->PreLoadShader(variant);
            _variant.set(this, variant);
        }

        inline ltLight* getHeadlight(int index) {
            //cap index
            if (index < 0)
                index = 0;
            if (index >= 2)
                index = 1;
            
            auto headlightLightsArray = *getPtr<ltLight*>(this, 0xB0);
            if (headlightLightsArray == nullptr)
                return NULL;
            return &headlightLightsArray[index];
        }

        AGE_API void BreakElectrics(Vector3* a1)            { hook::Thunk<0x4CEFE0>::Call<void>(this, a1); }
        AGE_API void ClearDamage()                          { hook::Thunk<0x4CDFF0>::Call<void>(this); }
        AGE_API void EjectOneshot()                         { hook::Thunk<0x4CDCA0>::Call<void>(this); }
        AGE_API bool GetVisible()                           { return hook::Thunk<0x4CF070>::Call<bool>(this); }
        AGE_API void SetVisible(bool a1)                    { hook::Thunk<0x4CF050>::Call<void>(this, a1); }
        AGE_API void DrawHeadlights(bool rotate)            { hook::Thunk<0x4CED50>::Call<void>(this, rotate); }
        
        /*
            lvlInstance virtuals
        */

        AGE_API void Reset()  override                      { hook::Thunk<0x4CDFD0>::Call<void>(this); }
        AGE_API const Vector3 & GetPosition() override
                                                            { return hook::Thunk<0x4CEF50>::Call<const Vector3 &>(this); }
        AGE_API const Matrix34 & GetMatrix(Matrix34 *a1)
                                                            { return hook::Thunk<0x4CEF90>::Call<const Matrix34 &>(this, a1); }
        AGE_API void SetMatrix(const Matrix34 *a1) override 
                                                            { hook::Thunk<0x4CEFA0>::Call<void>(this, a1); }
        AGE_API dgPhysEntity * GetEntity() override         { return hook::Thunk<0x4CEFC0>::Call<dgPhysEntity*>(this); }
        AGE_API dgPhysEntity * AttachEntity() override 
                                                            { return hook::Thunk<0x4CEFD0>::Call<dgPhysEntity*>(this); }
        AGE_API const Vector3 * GetVelocity() override
                                                            { return hook::Thunk<0x4CEF80>::Call<Vector3 const*>(this); }

        AGE_API void Draw(int a1) override                  { hook::Thunk<0x4CE040>::Call<void>(this, a1); }
        AGE_API void DrawShadow() override                  { hook::Thunk<0x4CE940>::Call<void>(this); }
        AGE_API void DrawShadowMap() override               { hook::Thunk<0x4CEA90>::Call<void>(this); }
        AGE_API void DrawGlow() override                    { hook::Thunk<0x4CEB90>::Call<void>(this); }
        AGE_API void DrawReflected(float a1) override
                                                            { hook::Thunk<0x4CF080>::Call<void>(this, a1); }
        AGE_API unsigned int SizeOf() override              { return hook::Thunk<0x4CDFE0>::Call<int>(this); }



        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<vehCarModel, lvlInstance>("vehCarModel")
                //properties
                .addPropertyReadOnly("Breakables", &getGenBreakableMgr)
                .addPropertyReadOnly("WheelBreakables", &getMechBreakableMgr)
                .addProperty("Variant", &getVariant, &setVariant)
                .addProperty("Visible", &GetVisible, &SetVisible)

                //lua functions
                .addFunction("GetHeadlight", &getHeadlight)

                //functions
                .addFunction("Reset", &Reset)
                .addFunction("BreakElectrics", &BreakElectrics)
                .addFunction("ClearDamage", &ClearDamage)
                .addFunction("EjectOneshot", &EjectOneshot)


            .endClass();
        }
    private:
        byte _buffer[0xCC];
    };

    // Lua initialization

}