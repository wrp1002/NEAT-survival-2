#pragma once

#include "Creature.h"
#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>

#include <box2d/b2_math.h>
#include <box2d/b2_shape.h>
#include <box2d/box2d.h>

#include <vector>
#include <memory>

#include "../Object.h"

using namespace std;


class BodySegment : public Object {
    private:
        weak_ptr<Creature> creature;
        vector<shared_ptr<BodySegment>> children;
        int angleOnParent;

        b2Vec2 GetPosOnParent(shared_ptr<BodySegment> parent, float angleOnObject, float angleOffset, b2Vec2 thisWorldSize);

    public:

        BodySegment(b2World &world, shared_ptr<Creature> parentCreature, b2Vec2 pixelSize, ALLEGRO_COLOR color, int shapeType, b2Vec2 pos, float angle);
        BodySegment(b2World &world, shared_ptr<Creature> parentCreature, b2Vec2 pixelSize, ALLEGRO_COLOR color, int shapeType, shared_ptr<BodySegment> parent, float angleOnParent, float angleOffset);

        void Draw();

        void AddChild(shared_ptr<BodySegment> child);
        bool childAngleValid(int angle);
};