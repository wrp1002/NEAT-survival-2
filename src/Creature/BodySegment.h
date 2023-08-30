#pragma once

#include "Creature.h"
#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>

#include <box2d/b2_math.h>
#include <box2d/box2d.h>

#include <vector>
#include <memory>

#include "../RectObject.h"

using namespace std;


class BodySegment : public RectObject {
    private:
        weak_ptr<Creature> creature;
        vector<shared_ptr<BodySegment>> children;

        b2Vec2 GetPosOnParent(shared_ptr<BodySegment> parent, float angleOnObject, float angleFromObject);

    public:

        BodySegment(b2World &world, shared_ptr<Creature> parentCreature, b2Vec2 size, ALLEGRO_COLOR color, b2Vec2 pos, float angle);
        BodySegment(b2World &world, shared_ptr<Creature> parentCreature, b2Vec2 size, ALLEGRO_COLOR color, shared_ptr<BodySegment> parent, float angleOnParent, float angleFromParent);

        void Draw();

        void AddChild(shared_ptr<BodySegment> child);
        bool childAngleValid(int angle);
};