#pragma once

#include "Object.h"
#include <box2d/b2_math.h>
#include <box2d/b2_world.h>

#include <memory>


class RectObject : public Object {
    private:

    public:
        RectObject(b2World &world, b2Vec2 pos, b2Vec2 size, float angle, ALLEGRO_COLOR color);

        virtual void Draw();
};