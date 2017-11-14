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
    static void Install();
};

class aiPathHandler {
public:
    void UpdatePedestrians();
    static void Install();
};