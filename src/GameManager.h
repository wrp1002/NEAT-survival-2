#pragma once

#include "Creature/Creature.h"
#include <box2d/b2_math.h>
#include <box2d/b2_world.h>

#include <vector>
#include <memory>

using namespace std;


namespace GameManager {
    extern ALLEGRO_DISPLAY *display;
    extern ALLEGRO_EVENT_QUEUE *event_queue;
    extern ALLEGRO_TIMER *timer;

    extern b2Vec2 gravity;
    extern b2World world;

    extern vector<shared_ptr<Creature>> agents;

    void Init();
    void InitAllegro();

    void Update();
    void Draw();

    void CreateAgent(string genes, b2Vec2 pos);
    void ClearAgents();

}