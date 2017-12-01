#pragma once
#include "common.h"
#include "patch.h"
#include "mm2.h"

class gfxImageHandler
{
public:
    void Scale(int width, int height);

    static void Install();
};

class aiPedestrianHandler
{
public:
    void Update();

    static void Install();
};

class aiPoliceForceHandler {
public:
    void Reset();
    BOOL IsPerpDrivingMadly(MM2::vehCar *perpCar);

    static void Install();
};

class aiPathHandler {
public:
    void UpdatePedestrians();

    static void Install();
};

class vehCarHandler : handler_t {
public:
    void InitCar(LPCSTR vehName, int a2, int a3, bool a4, bool a5);
    void InitCarAudio(LPCSTR name, int a2);
    void InitBound(LPCSTR name, bool a2);
    const MM2::phBound * GetModelBound(int a1);

    static void Install(void);
};