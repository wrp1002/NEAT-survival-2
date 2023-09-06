#include "Joint.h"

#include <allegro5/allegro_primitives.h>

#include <box2d/b2_distance_joint.h>
#include <box2d/b2_joint.h>
#include <box2d/b2_revolute_joint.h>

#include "../GameManager.h"
#include "../Globals.h"
#include "../Util.h"
#include "../Camera.h"

Joint::Joint(JointInfo jointInfo, b2Vec2 jointPos, b2Body *bodyA, b2Body *bodyB) {
    this->broken = false;
    this->shouldDeleteJoints = true;
    this->revoluteJoint = nullptr;
    this->springJoint = nullptr;

    // joint together
    b2RevoluteJointDef jointDef;
    jointDef.Initialize(bodyA, bodyB, jointPos);
    jointDef.lowerAngle = -jointInfo.angleLimit;
    jointDef.upperAngle = jointInfo.angleLimit;
    jointDef.enableLimit = jointInfo.enableLimit;
    jointDef.maxMotorTorque = jointInfo.maxMotorTorque;
    jointDef.motorSpeed = jointInfo.motorSpeed;
    jointDef.enableMotor = jointInfo.enableMotor;
    jointDef.collideConnected = false;

    revoluteJoint = (b2RevoluteJoint *)GameManager::world.CreateJoint(&jointDef);
    allJoints.push_back(revoluteJoint);


    if (jointInfo.useSpring) {
        b2DistanceJointDef distanceJointDef;
        distanceJointDef.Initialize(bodyA, bodyB, bodyA->GetPosition(), bodyB->GetPosition());
        float frequencyHz = 4.0f;
        float dampingRatio = 0.5f;
        b2LinearStiffness(distanceJointDef.stiffness, distanceJointDef.damping, frequencyHz, dampingRatio, jointDef.bodyA, jointDef.bodyB);

        springJoint = (b2DistanceJoint *)GameManager::world.CreateJoint(&distanceJointDef);
        allJoints.push_back(springJoint);
    }
}


Joint::~Joint() {
    cout << "joint destoryor" << endl;

}

void Joint::Destroy() {
    if (revoluteJoint) {
        RemoveJoint(revoluteJoint);
        GameManager::world.DestroyJoint(revoluteJoint);
        revoluteJoint = nullptr;
    }
    if (springJoint) {
        RemoveJoint(springJoint);
        GameManager::world.DestroyJoint(springJoint);
        springJoint = nullptr;
    }
}

void Joint::RemoveJoint(b2Joint *joint) {
    for (int i = allJoints.size() - 1; i >= 0; i--) {
        if (allJoints[i] == joint) {
            allJoints.erase(allJoints.begin() + i);
        }
    }
}


void Joint::Update() {
    if (revoluteJoint) {
        if (JointShouldBreak(revoluteJoint)) {
            RemoveJoint(revoluteJoint);
            GameManager::world.DestroyJoint(revoluteJoint);
            revoluteJoint = nullptr;
            cout << "revoluteJoint broke" << endl;
        }
    }

    if (springJoint) {
        if (JointShouldBreak(springJoint)) {
            RemoveJoint(springJoint);
            GameManager::world.DestroyJoint(springJoint);
            springJoint = nullptr;
            cout << "springJoint broke" << endl;
        }
    }
}


void Joint::Draw() {
    for (auto joint : allJoints) {
        b2Vec2 pos = Util::metersToPixels(joint->GetAnchorA());

        ALLEGRO_TRANSFORM t;
        al_identity_transform(&t);
        al_translate_transform(&t, pos.x, pos.y);
        al_compose_transform(&t, &Camera::transform);
        al_use_transform(&t);
        al_draw_filled_circle(0, 0, 2, al_map_rgb(0, 255, 0));

        pos = Util::metersToPixels(joint->GetAnchorB());
        al_identity_transform(&t);
        al_translate_transform(&t, pos.x, pos.y);
        al_compose_transform(&t, &Camera::transform);
        al_use_transform(&t);
        al_draw_filled_circle(0, 0, 2, al_map_rgb(0, 255, 0));

    }
}


void Joint::SetBroken(bool val) {
    this->broken = val;
}

void Joint::SetShouldDeleteJoints(bool val) {
    this->shouldDeleteJoints = val;
}


bool Joint::IsBroken() {
    return this->broken;
}

bool Joint::JointShouldBreak(b2Joint *joint) {
    b2Vec2 reactionForce = joint->GetReactionForce(Globals::FPS);
    float forceModuleSq = reactionForce.LengthSquared();
    float maxForce = 0.02;
    return forceModuleSq >= maxForce;
}
