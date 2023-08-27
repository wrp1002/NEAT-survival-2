#include "BodySegment.h"

#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>

#include <allegro5/transformations.h>
#include <box2d/b2_joint.h>
#include <box2d/b2_settings.h>
#include <box2d/box2d.h>

#include <fcntl.h>
#include <vector>
#include <memory>

#include "../Util.h"
#include "../Camera.h"
#include "Creature.h"

using namespace std;

BodySegment::BodySegment(b2World &world, shared_ptr<Creature> parentCreature, b2Vec2 pos, int size, ALLEGRO_COLOR color) {
    this->creature = parentCreature;
    this->angleOnBody = 0;
    this->angleFromBody = 0;
    this->size = size;
    this->color = color;

    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = pos;
    this->body = world.CreateBody(&bodyDef);

    b2CircleShape circle;
    circle.m_radius = Util::pixelsToMeters(size);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &circle;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;
    fixtureDef.restitution = 0.5f;

    this->body->CreateFixture(&fixtureDef);
}

BodySegment::BodySegment(b2World &world,shared_ptr<Creature> parentCreature, shared_ptr<BodySegment> parent, int angleOnBody, int angleFromBody, int size, ALLEGRO_COLOR color) {
    this->creature = parentCreature;
    this->angleOnBody = angleOnBody;
    this->angleFromBody = angleFromBody;
    this->size = size;
    this->color = color;


    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;

    b2Vec2 parentPos = parent.get()->body->GetPosition();
    float angleF = angleOnBody * M_PI / 180;
    float distance = parent.get()->size + size - 5;
    bodyDef.position = parentPos + Util::pixelsToMeters(cos(angleF) * distance, sin(angleF) * distance);
    this->body = world.CreateBody(&bodyDef);


    b2CircleShape circle;
    circle.m_radius = Util::pixelsToMeters(size);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &circle;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;
    fixtureDef.restitution = 0.5f;

    this->body->CreateFixture(&fixtureDef);


    // joint together
    b2RevoluteJointDef jointDef;
    b2Vec2 jointPos = parentPos + Util::pixelsToMeters(cos(angleF) * parent.get()->size, sin(angleF) * parent.get()->size);
    jointDef.Initialize(body, parent.get()->body, jointPos);
    jointDef.lowerAngle = -0.5f * b2_pi;
    jointDef.upperAngle = 0.5f * b2_pi;
    jointDef.enableLimit = true;
    jointDef.maxMotorTorque = 2.0f;
    jointDef.motorSpeed = 0.0f;
    jointDef.enableMotor = false;
    jointDef.collideConnected = false;

    b2Joint *joint = world.CreateJoint(&jointDef);

    if (shared_ptr<Creature> creaturePtr = creature.lock()) {
        creaturePtr->AddJoint(joint);
    }

    //joints.push_back(joint);

}

void BodySegment::AddChild(shared_ptr<BodySegment> child) {
    children.push_back(child);
}

bool BodySegment::childAngleValid(int angle) {
    for (auto child : children) {
        if (child.get()->angleOnBody == angle)
            return false;
    }
    return true;
}

void BodySegment::Draw() {
    for (auto child : children)
        child->Draw();

    float angle = body->GetAngle();
    b2Vec2 pos = Util::metersToPixels(body->GetPosition());

    ALLEGRO_TRANSFORM t;
    al_identity_transform(&t);

    al_rotate_transform(&t, angle);
    al_translate_transform(&t, pos.x, pos.y);
    al_compose_transform(&t, &Camera::transform);

    al_use_transform(&t);

    al_draw_filled_circle(0, 0, size, color);

    al_identity_transform(&t);
    al_use_transform(&t);
}