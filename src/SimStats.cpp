#include "SimStats.h"
#include <chrono>


namespace SimStats {
	float lastPhysicsTime;
	float lastUpdateTime;
	float lastDrawTime;

	high_resolution_clock::time_point physicsStart;
	high_resolution_clock::time_point updateStart;
	high_resolution_clock::time_point drawStart;

	void Init() {
		lastPhysicsTime = 0;
		lastUpdateTime = 0;
		lastDrawTime = 0;

	}

	void ResetTimes() {
		lastPhysicsTime = 0;
		lastUpdateTime = 0;
		lastDrawTime = 0;
	}

	void StartUpdateTimer() {
		updateStart = high_resolution_clock::now();
	}
	void StartPhysicsTimer() {
		physicsStart = high_resolution_clock::now();
	}
	void StartDrawTimer() {
		drawStart = high_resolution_clock::now();
	}

	void EndUpdateTimer() {
		auto endTime = high_resolution_clock::now();
		auto ms_int = duration_cast<milliseconds>(endTime - updateStart);
		lastUpdateTime = ms_int.count();
	}
	void EndPhysicsTimer() {
		auto endTime = high_resolution_clock::now();
		auto ms_int = duration_cast<milliseconds>(endTime - physicsStart);
		lastPhysicsTime = ms_int.count();
	}
	void EndDrawTimer() {
		auto endTime = high_resolution_clock::now();
		auto ms_int = duration_cast<milliseconds>(endTime - drawStart);
		lastDrawTime = ms_int.count();
	}


	void Draw() {
		Font::DrawText("arial.ttf", 16, fmt::format("Draw: {}ms", lastDrawTime), 10, 0);
		Font::DrawText("arial.ttf", 16, fmt::format("Update: {}ms", lastUpdateTime), 10, 16);
		Font::DrawText("arial.ttf", 16, fmt::format("Physics: {}ms", lastPhysicsTime), 10, 32);
	}

}