#include "Joint.h"

#include <allegro5/allegro_primitives.h>

#include <box2d/b2_distance_joint.h>
#include <box2d/b2_joint.h>
#include <box2d/b2_math.h>
#include <box2d/b2_revolute_joint.h>
#include <cstdint>

#include "../GameManager.h"
#include "../Globals.h"
#include "../Util.h"
#include "../Camera.h"
#include "../JointUserData.h"

Joint::Joint(JointInfo jointInfo, b2Vec2 jointPos, b2Body *bodyA, b2Body *bodyB) {
	this->broken = false;
	this->speed = 0;
	this->shouldDeleteJoints = true;
	this->revoluteJoint = nullptr;
	this->springJoint = nullptr;
	this->userData = make_shared<JointUserData>(JointUserData());

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
	jointDef.userData.pointer = reinterpret_cast<uintptr_t>(this->userData.get());

	this->revoluteJoint = (b2RevoluteJoint *)GameManager::world.CreateJoint(&jointDef);
	allJoints.push_back(revoluteJoint);


	if (jointInfo.useSpring) {
		b2DistanceJointDef distanceJointDef;
		distanceJointDef.Initialize(bodyA, bodyB, bodyA->GetPosition(), bodyB->GetPosition());
		distanceJointDef.userData.pointer = reinterpret_cast<uintptr_t>(this->userData.get());
		float frequencyHz = 4.0f;
		float dampingRatio = 0.5f;
		b2LinearStiffness(distanceJointDef.stiffness, distanceJointDef.damping, frequencyHz, dampingRatio, jointDef.bodyA, jointDef.bodyB);

		this->springJoint = (b2DistanceJoint *)GameManager::world.CreateJoint(&distanceJointDef);
		allJoints.push_back(springJoint);
	}
}

void Joint::UpdateUserData() {
	userData->parentObject = shared_from_this();
	userData->objectType = "Joint";
}


Joint::~Joint() {
}

void Joint::Destroy() {
	if (revoluteJoint != nullptr) {
		GameManager::world.DestroyJoint(revoluteJoint);
		revoluteJoint = nullptr;
	}
	if (springJoint != nullptr) {
		GameManager::world.DestroyJoint(springJoint);
		springJoint = nullptr;
	}
	allJoints.clear();
	broken = true;
}

void Joint::RemoveJoint(b2Joint *joint) {
	if (joint == revoluteJoint)
		revoluteJoint = nullptr;
	else if (joint == springJoint)
		springJoint = nullptr;

	for (int i = allJoints.size() - 1; i >= 0; i--) {
		if (allJoints[i] == joint) {
			allJoints.erase(allJoints.begin() + i);
			break;
		}
	}
	if (allJoints.size() == 0)
		broken = true;
}


void Joint::Update() {
	if (broken)
		return;

	if (JointShouldBreak(revoluteJoint) && JointShouldBreak(springJoint)) {
		Destroy();
	}

	if (!revoluteJoint && !springJoint)
		broken = true;
}


void Joint::Draw() {
	if (broken)
		return;

	for (auto joint : allJoints) {
		if (!joint)
			continue;

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

void Joint::SetSpeed(float val) {
	if (!revoluteJoint)
		return;

	revoluteJoint->SetMotorSpeed(val*3);
}


bool Joint::IsBroken() {
	return this->broken;
}

bool Joint::JointShouldBreak(b2Joint *joint) {
	if (joint == nullptr)
		return true;

	b2Vec2 reactionForce = joint->GetReactionForce(Globals::FPS);
	float forceModuleSq = reactionForce.LengthSquared();
	float maxForce = 0.02;
	return forceModuleSq >= maxForce;
}

float Joint::GetAngle() {
	if (!revoluteJoint)
		return 0;

	return revoluteJoint->GetJointAngle();
}
