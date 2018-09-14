#pragma once
#include <modules\ai.h>

namespace MM2
{
    // Forward declarations
    class aiObstacle;
    class aiVehicle;

    // External declarations


    // Class definitions

    class aiObstacle {
    public:
        aiObstacle(void)                                    DONOTCALL;
        aiObstacle(const aiObstacle &&)                     DONOTCALL;

        virtual BOOL InAccident(void)                       { return ageHook::Thunk<0x53F5D0>::Call<BOOL>(this); }
        virtual void Position(Vector3 &a1)                  PURE;
        virtual float Speed(void)                           PURE;
        virtual float BreakThreshold(void)                  { return ageHook::Thunk<0x53F5E0>::Call<float>(this); }
        virtual BOOL Drivable(void)                         { return ageHook::Thunk<0x53F5F0>::Call<BOOL>(this); }
        virtual int CurrentRoadIdx(aiPath **a1, const bool *a2,int *a3)
                                                            PURE;
        virtual int CurrentRdVert(void)                     PURE;
        virtual void PreAvoid(const Vector3 &a1, const Vector3 &a2, float a3, Vector3 &a4, Vector3 &a5)
                                                            PURE;
        virtual float IsBlockingTarget(const Vector3 &a1, const Vector3 &a2, float a3, float a4)
                                                            PURE;
    };

    class aiVehicle : public aiObstacle {
    public:
        aiVehicle(void)                                     DONOTCALL;
        aiVehicle(const aiVehicle &&)                       DONOTCALL;

        void PreAvoid(const Vector3 &a1, const Vector3 &a2, float a3, Vector3 &a4, Vector3 &a5) override
                                                            FORWARD_THUNK;
        float IsBlockingTarget(const Vector3 &a1, const Vector3 &a2, float a3, float a4) override
                                                            FORWARD_THUNK;
        
        virtual void Update(void)                           { return ageHook::Thunk<0x556230>::Call<void>(this); };
        virtual void Reset(void)                            { return ageHook::Thunk<0x556210>::Call<void>(this); };
        virtual int Type(void)                              PURE;
        virtual Matrix34 & GetMatrix(void)                  PURE;
        virtual float FrontBumperDistance(void)             PURE;
        virtual float BackBumperDistance(void)              PURE;
        virtual float LSideDistance(void)                   PURE;
        virtual float RSideDistance(void)                   PURE;
        virtual int CurrentLane(void)                       PURE;
        virtual int CurrentRoadId(void)                     PURE;
        virtual void DrawId(void)                           PURE;
        virtual void ReplayDebug(void)                      { return ageHook::Thunk<0x556D00>::Call<void>(this); }
    };

    // Lua initialization

}