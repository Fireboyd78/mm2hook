#pragma once
#include <modules\ai.h>
#include <mm2_audio.h>

#include "aiVehicle.h"

namespace MM2
{
    // Forward declarations
    class aiVehicleSpline;

    // External declarations
    extern class AudImpact;
    extern class aiRailSet;

    // Class definitions

    class aiVehicleSpline : public aiVehicle {
    private:
        hook::Field<0x10, aiRailSet> _railSet;
        hook::Field<0xE6, short> _playerId;
        hook::Field<0xF4, float> _curSpeed;
    protected:
        byte _buffer[0x184];
    public:
        aiVehicleSpline(void)                               DONOTCALL;
        aiVehicleSpline(const aiVehicleSpline &&)           DONOTCALL;

        void Position(Vector3 &a1) override                 FORWARD_THUNK;
        float Speed(void) override                          FORWARD_THUNK;
        int CurrentRoadIdx(aiPath **a1, const bool *a2, int *a3) override
                                                            FORWARD_THUNK;
        int CurrentRdVert(void) override                    FORWARD_THUNK;

        void Update(void) override                          FORWARD_THUNK;
        void Reset(void) override                           FORWARD_THUNK;
        int Type(void) override                             FORWARD_THUNK;
        Matrix34 & GetMatrix(void) override                 FORWARD_THUNK;
        float FrontBumperDistance(void) override            FORWARD_THUNK;
        float BackBumperDistance(void) override             FORWARD_THUNK;
        float LSideDistance(void) override                  FORWARD_THUNK;
        float RSideDistance(void) override                  FORWARD_THUNK;
        int CurrentLane(void) override                      FORWARD_THUNK;
        int CurrentRoadId(void) override                    FORWARD_THUNK;
        void DrawId(void) override                          FORWARD_THUNK;

        virtual void Impact(int a1)                         FORWARD_THUNK;
        virtual AudImpact * GetAudImpactPtr(void)           FORWARD_THUNK;
        virtual void PlayHorn(float a1, float a2)           FORWARD_THUNK;
        virtual void StopVoice(void)                        FORWARD_THUNK;

        //fields
        inline float getCurSpeed()
        {
            return _curSpeed.get(this);
        }

        inline void setCurSpeed(float speed)
        {
            _curSpeed.set(this, speed);
        }

        inline short getPlayerId()
        {
            return _playerId.get(this);
        }

        inline aiRailSet * getRailSet()
        {
            return _railSet.ptr(this);
        }

        inline aiVehicleInstance * getVehicleInstance(void) const {
            return *getPtr<aiVehicleInstance*>(this, 0xD4);
        }

        AGE_API void SolveYPositionAndOrientation()         { hook::Thunk<0x5690C0>::Call<void>(this); }

        //lua
        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<aiVehicleSpline, aiVehicle>("aiVehicleSpline")
                .addPropertyReadOnly("VehicleInstance", &getVehicleInstance)
                .endClass();
        }
    };

    ASSERT_SIZEOF(aiVehicleSpline, 0x18C);

    // Lua initialization

}