#pragma once

#include "Creature.h"
#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>

#include <box2d/box2d.h>

#include <vector>
#include <memory>

using namespace std;


class BodySegment {
    private:
        b2Body *body;
        int size;
        int angleOnBody;
        int angleFromBody;
        weak_ptr<Creature> creature;
        vector<shared_ptr<BodySegment>> children;
        ALLEGRO_COLOR color;

    public:

        BodySegment(b2World &world, shared_ptr<Creature> parentCreature, b2Vec2 pos, int size, ALLEGRO_COLOR color);
        BodySegment(b2World &world, shared_ptr<Creature> parentCreature, shared_ptr<BodySegment> parent, int angleOnBody, int angleFromBody, int size, ALLEGRO_COLOR color);

        void Draw();

        void AddChild(shared_ptr<BodySegment> child);
        bool childAngleValid(int angle);
};