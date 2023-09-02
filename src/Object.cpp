#include "Object.h"
#include "Camera.h"
#include "Util.h"
#include <allegro5/allegro_primitives.h>
#include <allegro5/color.h>
#include <box2d/b2_circle_shape.h>
#include <box2d/b2_collision.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_math.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_shape.h>

#include <iostream>

using namespace std;


Object::Object() {
    this->pixelSize = b2Vec2(10, 10);
    this->worldSize = Util::pixelsToMeters(pixelSize);
    this->color = al_map_rgb(255, 0, 255);
    shapeType = SHAPE_TYPES::CIRCLE;
}

Object::Object(b2World &world, b2Vec2 pos, b2Vec2 pixelSize, float angle, ALLEGRO_COLOR color, int shapeType) {
    this->pixelSize = pixelSize;
    this->worldSize = Util::pixelsToMeters(pixelSize);
    this->color = color;
    this->shapeType = shapeType;

    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;

    bodyDef.position = Util::pixelsToMeters(pos);
    bodyDef.angle = angle;

    this->body = world.CreateBody(&bodyDef);

    b2FixtureDef fixtureDef;
    b2CircleShape circleShapeDef; circleShapeDef.m_radius = worldSize.x;
    b2PolygonShape rectShapeDef;  rectShapeDef.SetAsBox(worldSize.x, worldSize.y);

    if (shapeType == SHAPE_TYPES::CIRCLE)
        fixtureDef.shape = &circleShapeDef;
    else if (shapeType == SHAPE_TYPES::RECT)
        fixtureDef.shape = &rectShapeDef;

    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;
    fixtureDef.restitution = 0.5f;

    body->CreateFixture(&fixtureDef);
}


void Object::Update() {

}

void Object::Draw() {
    float angle = body->GetAngle();
    b2Vec2 pos = Util::metersToPixels(body->GetPosition());

    ALLEGRO_TRANSFORM t;
    al_identity_transform(&t);

    al_rotate_transform(&t, angle);
    al_translate_transform(&t, pos.x, pos.y);
    al_compose_transform(&t, &Camera::transform);

    al_use_transform(&t);

    switch (this->shapeType) {
        case SHAPE_TYPES::CIRCLE: {
            al_draw_filled_circle(0, 0, pixelSize.x, color);
            break;
        }
        case SHAPE_TYPES::RECT: {
            al_draw_filled_rounded_rectangle(-pixelSize.x, -pixelSize.y, pixelSize.x, pixelSize.y, 10, 10, color);
            break;
        }
    }

    al_draw_line(0, 0, 0, -30, al_map_rgb(255, 255, 255), 2);

    al_identity_transform(&t);
    al_use_transform(&t);
}


// Raycast from a point from this angle towards the object to find collision point on edge
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