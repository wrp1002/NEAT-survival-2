#include "BodySegment.h"

#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>

#include <allegro5/transformations.h>
#include <box2d/b2_joint.h>
#include <box2d/b2_math.h>
#include <box2d/b2_shape.h>
#include <box2d/box2d.h>

#include <fcntl.h>
#include <vector>
#include <memory>
#include <iostream>

#include "../Util.h"
#include "../Object.h"
#include "Creature.h"

using namespace std;

BodySegment::BodySegment(b2World &world, shared_ptr<Creature> parentCreature, b2Vec2 size, ALLEGRO_COLOR color, int shapeType, b2Vec2 pos, float angle) : Object(world, pos, size, angle, color, shapeType) {
    this->creature = parentCreature;
}

BodySegment::BodySegment(b2World &world, shared_ptr<Creature> parentCreature, b2Vec2 size, ALLEGRO_COLOR color, int shapeType, shared_ptr<BodySegment> parent, float angleOnParent, float angleOffset) :
        Object(
            world,
            GetPosOnParent(parent, angleOnParent, angleOffset),
            size, parent->body->GetAngle() - (angleOffset + angleOnParent),
            color,
            shapeType
        ) {

    this->creature = parentCreature;
    this->angleOnParent = Util::RadiansToDegrees(angleOnParent);

    // joint together
    b2RevoluteJointDef jointDef;
    b2Vec2 jointPos = parent->GetEdgePoint(-angleOnParent + parent->body->GetAngle());
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
    /*
    for (auto child : children) {
        if (child.get()->angleOnBody == angle)
            return false;
    }
    */
    return true;
}

void BodySegment::Draw() {
    for (auto child : children)
        child->Draw();

    Object::Draw();
}


b2Vec2 BodySegment::GetPosOnParent(shared_ptr<BodySegment> otherObject, float angleOnObject, float angleOffset) {
    b2Vec2 parentEdgePos = otherObject->GetEdgePoint(-angleOnObject + angleOffset + otherObject->body->GetAngle());
    b2Vec2 relPos = b2Vec2(cos(otherObject->body->GetAngle() - (angleOffset + angleOnObject)) * worldSize.x, sin(otherObject->body->GetAngle() - (angleOffset + angleOnObject)) * worldSize.x);

    b2Vec2 pos = parentEdgePos + relPos;
    return Util::metersToPixels(pos);
}
