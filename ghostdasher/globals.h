#pragma once
#include "global-includes.h"

struct Globals
{
	float curtime;
	int tick_count;
	float frametime;
	float interval_per_tick;
};

extern std::unique_ptr<Globals> globals;