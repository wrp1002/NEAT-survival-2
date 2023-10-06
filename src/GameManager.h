#pragma once

#include "Creature/Creature.h"
#include <box2d/b2_body.h>
#include <box2d/b2_math.h>
#include <box2d/b2_world.h>

#include <vector>
#include <memory>

class Egg;

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
	extern b2Body *worldBorder;

	extern vector<shared_ptr<Creature>> agents;
	extern vector<shared_ptr<Egg>> eggs;
	extern vector<shared_ptr<Object>> looseObjects;
	extern vector<weak_ptr<Object>> objectsOutsideBorder;


	void Init();
	void InitAllegro();
	void Shutdown();
	b2Body * CreateWorldBorder();

	void Update();
	void Draw();

	shared_ptr<Creature> CreateAgent(string genes, b2Vec2 pos);
	void ClearAgents();

	void AddObject(shared_ptr<Object> newObject);

	double GetSimTime();
	string GetSimTimeStr();
	string GetSimTicksStr();

	bool IsPaused();
	void TogglePaused();
	void ResetSpeed();
	void IncreaseSpeed();
	void DecreaseSpeed();
}