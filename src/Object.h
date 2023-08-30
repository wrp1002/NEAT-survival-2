#pragma once

#include <allegro5/color.h>

#include <box2d/b2_body.h>
#include <box2d/b2_common.h>
#include <box2d/b2_math.h>
#include <box2d/b2_world.h>
#include <box2d/box2d.h>

using namespace std;

class Object {
    protected:
        b2Body *body;
        b2Vec2 pixelSize;
        b2Vec2 worldSize;
        ALLEGRO_COLOR color;

    public:
        Object();
        Object(b2Vec2 size, ALLEGRO_COLOR color);

        virtual void Update();
        virtual void Draw();

        b2Vec2 GetEdgePoint(float angle);
};

