#pragma once

#include <fmt/core.h>
#include <chrono>

#include "UI/Font.h"

using namespace std;
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;

namespace SimStats {
	extern float lastPhysicsTime;
	extern float lastUpdateTime;
	extern float lastDrawTime;

	extern high_resolution_clock::time_point physicsStart;
	extern high_resolution_clock::time_point updateStart;
	extern high_resolution_clock::time_point drawStart;

	void Init();

	void ResetTimes();

	void StartUpdateTimer();
	void StartPhysicsTimer();
	void StartDrawTimer();

	void EndUpdateTimer();
	void EndPhysicsTimer();
	void EndDrawTimer();

	void Draw();

}