#pragma once
#include "common.h"
#include "patch.h"
#include "mm2.h"

#include <vector>

static ConfigValue<bool> cfgWheelDebug("WheelsDebug", false);
static ConfigValue<bool> cfgAiDebug("AIDebug", false);

class aiMapDebugHandler {
public:
    void Cull();
    void UpdatePaused();
    static void Install();
};

class vehWheelDebugHandler {
public:
	void Cull();
	void Update();
	static void Install();
};