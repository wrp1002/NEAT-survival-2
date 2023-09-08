#include "BodySegment.h"

#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>

#include <allegro5/transformations.h>
#include <box2d/b2_distance_joint.h>
#include <box2d/b2_joint.h>
#include <box2d/b2_math.h>
#include <box2d/b2_shape.h>
#include <box2d/box2d.h>

#include <cmath>
#include <fcntl.h>
#include <vector>
#include <memory>
#include <iostream>

#include "../GameManager.h"
#include "../Util.h"
#include "../Object.h"
#include "Creature.h"
#include "Joint.h"

using namespace std;

BodySegment::BodySegment(shared_ptr<Creature> parentCreature, b2Vec2 pixelSize, ALLEGRO_COLOR color, int shapeType, b2Vec2 pos, float angle, NerveInfo &nerveInfo) : Object(pos, pixelSize, -angle, color, shapeType) {
	this->creature = parentCreature;
	this->parentJoint = nullptr;
	this->nerveInfo = nerveInfo;

	SetValidAngles(pixelSize);
}

BodySegment::BodySegment(shared_ptr<Creature> parentCreature, b2Vec2 pixelSize, ALLEGRO_COLOR color, int shapeType, shared_ptr<BodySegment> parent, float angleOnParent, float angleOffset, Joint::JointInfo jointInfo, NerveInfo &nerveInfo) :
		Object(
			GetPosOnParent(parent, angleOnParent, angleOffset, Util::pixelsToMeters(pixelSize)),
			pixelSize,
			parent->body->GetAngle() - (angleOffset + angleOnParent) - M_PI_2,
			color,
			shapeType
		) {

	this->creature = parentCreature;
	this->angleOnParent = Util::RadiansToDegrees(angleOnParent);
	this->nerveInfo = nerveInfo;
	SetValidAngles(pixelSize);

	// joint together
	b2RevoluteJointDef jointDef;
	b2Vec2 jointPos = parent->GetEdgePoint(-angleOnParent + parent->body->GetAngle());

	shared_ptr<Joint> newJoint = make_shared<Joint>(Joint(jointInfo, jointPos, body, parent->body));
	if (shared_ptr<Creature> creaturePtr = creature.lock())
		creaturePtr->AddJoint(newJoint);
	this->parentJoint = newJoint;
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

bool BodySegment::childAngleValid(int angle) {
	for (auto testAngle : validChildAngles)
		if (angle == testAngle)
			return true;
	return false;
}

void BodySegment::Draw() {
	Object::Draw();

	for (auto child : children) {
		if (!child.expired())
			child.lock()->Draw();
	}

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


float BodySegment::GetNerveOutput() {
	if (!parentJoint)
		return 0;

	return parentJoint->GetAngle();
}

int BodySegment::GetNerveOutputIndex() {
	return nerveInfo.outputIndex;
}

int BodySegment::GetNerveInputIndex() {
	return nerveInfo.inputIndex;
}

void BodySegment::SetNerveInput(float val) {
	if (!parentJoint)
		return;

	parentJoint->SetSpeed(val);
}
