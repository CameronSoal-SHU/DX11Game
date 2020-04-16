#include "D3DUtil.h"
#include "D3D.h"

static float gTime = 0.f;

float GetClock() {
	return gTime;
}

void AddSecToClock(float sec) {
	gTime += sec;
}
