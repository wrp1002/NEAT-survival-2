#include "BodySegment.h"

#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>

#include <allegro5/transformations.h>
#include <box2d/b2_joint.h>
#include <box2d/b2_math.h>
#include <box2d/b2_shape.h>
#include <box2d/box2d.h>

#include <cmath>
#include <fcntl.h>
#include <vector>
#include <memory>
#include <iostream>

#include "../Util.h"
#include "../Object.h"
#include "Creature.h"

using namespace std;

BodySegment::BodySegment(b2World &world, shared_ptr<Creature> parentCreature, b2Vec2 pixelSize, ALLEGRO_COLOR color, int shapeType, b2Vec2 pos, float angle) : Object(world, pos, pixelSize, -angle, color, shapeType) {
    this->creature = parentCreature;

    SetValidAngles(pixelSize);
}

BodySegment::BodySegment(b2World &world, shared_ptr<Creature> parentCreature, b2Vec2 pixelSize, ALLEGRO_COLOR color, int shapeType, shared_ptr<BodySegment> parent, float angleOnParent, float angleOffset) :
        Object(
            world,
            GetPosOnParent(parent, angleOnParent, angleOffset, Util::pixelsToMeters(pixelSize)),
            pixelSize,
            parent->body->GetAngle() - (angleOffset + angleOnParent) - M_PI_2,
            color,
            shapeType
        ) {

    this->creature = parentCreature;
    this->angleOnParent = Util::RadiansToDegrees(angleOnParent);
    SetValidAngles(pixelSize);

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

void BodySegment::AddChild(shared_ptr<BodySegment> child, int angle) {
    children.push_back(child);
    for (int i = validChildAngles.size() - 1; i >= 0; i--) {
        if (validChildAngles[i] == angle) {
            validChildAngles.erase(validChildAngles.begin() + i);
        }
    }
}

void BodySegment::SetValidAngles(b2Vec2 pixelSize) {
    validChildAngles.push_back(0); // right
    validChildAngles.push_back(180); // left
    validChildAngles.push_back(270); // down

    if (shapeType == Object::CIRCLE) {
        validChildAngles.push_back(45);
        validChildAngles.push_back(135);
        validChildAngles.push_back(225);
        validChildAngles.push_back(315);
    }
    else {

    }
}

bool BodySegment::CanAddChild() {
    return validChildAngles.size() > 0;
}

int BodySegment::GetValidChildAngle(int angleGene) {
    return validChildAngles[angleGene % validChildAngles.size()];
}

void BodySegment::Draw() {
    Object::Draw();

    for (auto child : children)
        child->Draw();

}


b2Body *BodySegment::GetBody() {
    return this->body;
}


b2Vec2 BodySegment::GetPosOnParent(shared_ptr<BodySegment> otherObject, float angleOnObject, float angleOffset, b2Vec2 thisWorldSize) {
    b2Vec2 parentEdgePos = otherObject->GetEdgePoint(-angleOnObject + otherObject->body->GetAngle());

    float angle = otherObject->body->GetAngle() - (angleOffset + angleOnObject);

    b2Vec2 relPos = b2Vec2(
        cos(angle) * thisWorldSize.y,
        sin(angle) * thisWorldSize.y
    );

    b2Vec2 pos = parentEdgePos + relPos ;
    return Util::metersToPixels(pos);
}
