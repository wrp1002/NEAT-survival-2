#include "RectObject.h"

#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>
#include <box2d/b2_math.h>
#include <box2d/b2_polygon_shape.h>

#include "Util.h"
#include "Camera.h"

#include <iostream>

using namespace std;

/*
RectObject::RectObject(b2World &world, b2Vec2 pos, b2Vec2 size, float angle, ALLEGRO_COLOR color) : Object(size, color) {
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;

    b2Vec2 worldSize = Util::pixelsToMeters(pixelSize);

    cout << angle << endl;

    bodyDef.position = Util::pixelsToMeters(pos);
    bodyDef.angle = angle;

    this->body = world.CreateBody(&bodyDef);

    b2PolygonShape shapeDef;
    shapeDef.SetAsBox(worldSize.x, worldSize.y);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shapeDef;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;
    fixtureDef.restitution = 0.5f;

    this->body->CreateFixture(&fixtureDef);
}


void RectObject::Draw() {
    float angle = body->GetAngle();
    b2Vec2 pos = Util::metersToPixels(body->GetPosition());

    ALLEGRO_TRANSFORM t;
    al_identity_transform(&t);

    al_rotate_transform(&t, angle);
    al_translate_transform(&t, pos.x, pos.y);
    al_compose_transform(&t, &Camera::transform);

    al_use_transform(&t);

    al_draw_filled_rectangle(-pixelSize.x, -pixelSize.y, pixelSize.x, pixelSize.y, color);
    al_draw_line(0, 0, 0, -30, al_map_rgb(255, 255, 255), 2);

    al_identity_transform(&t);
    al_use_transform(&t);
}
*/