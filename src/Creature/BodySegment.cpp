#include "BodySegment.h"

#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>

#include <allegro5/transformations.h>
#include <box2d/b2_joint.h>
#include <box2d/b2_math.h>
#include <box2d/b2_settings.h>
#include <box2d/box2d.h>

#include <fcntl.h>
#include <vector>
#include <memory>

#include "../Util.h"
#include "../Camera.h"
#include "../RectObject.h"
#include "Creature.h"

using namespace std;

BodySegment::BodySegment(b2World &world, shared_ptr<Creature> parentCreature, b2Vec2 size, ALLEGRO_COLOR color, b2Vec2 pos, float angle) : RectObject(world, pos, size, -angle, color) {
    this->creature = parentCreature;
}

BodySegment::BodySegment(b2World &world, shared_ptr<Creature> parentCreature, b2Vec2 size, ALLEGRO_COLOR color, shared_ptr<BodySegment> parent, float angleOnParent, float angleFromParent) :
        RectObject(world, GetPosOnParent(parent, angleOnParent, angleFromParent), size, -angleFromParent + parent->body->GetAngle(), color) {

    this->creature = parentCreature;


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

    RectObject::Draw();
}


b2Vec2 BodySegment::GetPosOnParent(shared_ptr<BodySegment> otherObject, float angleOnObject, float angleFromObject) {
    b2Vec2 parentEdgePos = otherObject->GetEdgePoint(-angleOnObject + otherObject->body->GetAngle());
    b2Vec2 relPos = b2Vec2(cos(-angleFromObject + otherObject->body->GetAngle()) * worldSize.x, sin(-angleFromObject + otherObject->body->GetAngle()) * worldSize.x);

    b2Vec2 pos = parentEdgePos + relPos;
    return Util::metersToPixels(pos);
}
