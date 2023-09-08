#pragma once

#include "Creature/Creature.h"
#include <box2d/b2_math.h>
#include <box2d/b2_world.h>

#include <vector>
#include <memory>

using namespace std;


namespace GameManager {
	extern int speed;
	extern bool paused;
	extern double simStartTime;
	extern double simTicks;
	extern int32 velocityIterations;
	extern int32 positionIterations;

	extern ALLEGRO_DISPLAY *display;
	extern ALLEGRO_EVENT_QUEUE *event_queue;
	extern ALLEGRO_TIMER *timer;

	extern b2Vec2 gravity;
	extern b2World world;

	extern vector<shared_ptr<Creature>> agents;

	void Init();
	void InitAllegro();
	void Shutdown();

	void Update();
	void Draw();

	void CreateAgent(string genes, b2Vec2 pos);
	void ClearAgents();

	double GetSimTime();
	string GetSimTimeStr();
	string GetSimTicksStr();

	bool IsPaused();
	void TogglePaused();
	void ResetSpeed();
	void IncreaseSpeed();
	void DecreaseSpeed();
}