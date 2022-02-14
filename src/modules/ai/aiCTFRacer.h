#pragma once
#include <modules\ai.h>

#include "aiVehiclePhysics.h"
#include "aiCTFRacer.h"

namespace MM2
{
    // Forward declarations
    class aiCTFRacer;

    // External declarations
    extern class Base;

    // Class definitions

    class aiCTFRacer : public Base 
    {
    private:
        hook::Field<0x04, Vector3> _flagPosition;
        hook::Field<0x1C, Vector3> _deliverPosition;
        hook::Field<0x028, aiVehiclePhysics> _physics;
        hook::Field<0x979A, unsigned short> _id;
        byte buffer[0x9870];
    public:
        aiCTFRacer(void)                                    DONOTCALL;
        aiCTFRacer(const aiCTFRacer &&)                     DONOTCALL;

        AGE_API void Init(int id, char* basename)           { hook::Thunk<0x554470>::Call<void>(this, id, basename); }

        inline aiVehiclePhysics* getVehiclePhysics()
        {
            return _physics.ptr(this);
        }

        inline int getId()
        {
            return _id.get(this);
        }

        inline int getState()
        {
            return getVehiclePhysics()->getState();
        }

        inline Vector3 getFlagPosition()
        {
            return _flagPosition.get(this);
        }

        inline Vector3 getDeliverPosition()
        {
            return _deliverPosition.get(this);
        }

        inline void setFlagPosition(Vector3 pos)
        {
            _flagPosition.set(this, pos);
        }

        inline void setDeliverPosition(Vector3 pos)
        {
            _deliverPosition.set(this, pos);
        }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<aiCTFRacer, Base>("aiCTFRacer")
                .addPropertyReadOnly("State", &getState)
                .addPropertyReadOnly("ID", &getId)
                .addProperty("FlagPosition", &getFlagPosition, &setFlagPosition)
                .addProperty("DeliverPosition", &getDeliverPosition, &setDeliverPosition)
                .endClass();
        }

    };

    ASSERT_SIZEOF(aiCTFRacer, 0x9878);

    // Lua initialization

}