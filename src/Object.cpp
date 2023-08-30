#include "Object.h"
#include "Util.h"
#include <allegro5/allegro_primitives.h>
#include <allegro5/color.h>
#include <box2d/b2_collision.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_math.h>
#include <box2d/b2_shape.h>

#include <iostream>

using namespace std;



Object::Object() {
    this->pixelSize = b2Vec2(10, 10);
    this->worldSize = Util::pixelsToMeters(pixelSize);
    this->color = al_map_rgb(255, 0, 255);
}

Object::Object(b2Vec2 size, ALLEGRO_COLOR color) {
    this->pixelSize = size;
    this->worldSize = Util::pixelsToMeters(size);
    this->color = color;
}

void Object::Update() {

}

void Object::Draw() {
    b2Vec2 currentPos = body->GetPosition();
    al_draw_filled_circle(currentPos.x, currentPos.y, pixelSize.x, al_map_rgb(255, 255, 255));
}


b2Vec2 Object::GetEdgePoint(float angle) {
    b2Transform transform;
    transform.SetIdentity();

    b2Vec2 pos = body->GetPosition();
    float distance = max(worldSize.x, worldSize.y) * 2;

    b2RayCastInput input;
    input.p1 = pos + distance * b2Vec2(cos(angle), sin(angle));
    input.p2.Set(pos.x, pos.y);

    input.maxFraction = 1.0;
    int32 childIndex = 0;

    b2Fixture fixture = body->GetFixtureList()[0];

    b2RayCastOutput output;
    bool hit = fixture.RayCast(&output, input, childIndex);

    if (hit)
    {
        b2Vec2 hitPoint = input.p1 + output.fraction * (input.p2 - input.p1);
        return hitPoint;
    }

    return b2Vec2(0, 0);
}