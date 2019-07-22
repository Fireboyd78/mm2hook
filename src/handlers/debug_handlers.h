#pragma once
#include "common.h"
#include "patch.h"
#include "mm2.h"

#include <vector>

static ConfigValue<bool> cfgWheelDebug("WheelsDebug", false);

class vehWheelDebugHandler {
public:
	void Cull();
	void Update();
	static void Install();
};