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

#include "../../Util.h"
#include "../Object.h"
#include "BodyPart.h"
#include "Creature.h"
#include "Joint.h"

using namespace std;

BodySegment::BodySegment(shared_ptr<Creature> parentCreature, b2Vec2 pixelSize, ALLEGRO_COLOR color, int shapeType, b2Vec2 pos, float angle, NerveInfo &nerveInfo) :
				BodyPart(parentCreature, pos, pixelSize, -angle, color, shapeType, nerveInfo) {

	this->parentJoint = nullptr;
	this->polymorphic_id = "BodySegment";

	SetValidAngles(pixelSize);
}

BodySegment::BodySegment(shared_ptr<Creature> parentCreature, b2Vec2 pixelSize, ALLEGRO_COLOR color, int shapeType, shared_ptr<BodyPart> parent, float angleOnParent, float angleOffset, Joint::JointInfo jointInfo, NerveInfo &nerveInfo) :
		BodyPart(
			parentCreature,
			GetPosOnParent(parent, angleOnParent, angleOffset, Util::pixelsToMeters(pixelSize)),
			pixelSize,
			parent->GetBody()->GetAngle() - (angleOffset + angleOnParent) - M_PI_2,
			color,
			shapeType,
			nerveInfo
		) {

	this->angleOnParent = Util::RadiansToDegrees(angleOnParent);
	this->polymorphic_id = "BodySegment";
	this->parentPart = parent;

	SetValidAngles(pixelSize);

	// joint together
	b2RevoluteJointDef jointDef;
	b2Vec2 jointPos = parent->GetEdgePoint(-angleOnParent + parent->GetBody()->GetAngle());

	shared_ptr<Joint> newJoint = make_shared<Joint>(Joint(jointInfo, jointPos, body, parent->GetBody()));
	SetParentJoint(newJoint);
}

void BodySegment::AddChild(shared_ptr<BodyPart> child, int angle) {
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

	al_draw_arc(0, 0, 5, 0, (health / maxHealth) * M_PI * 2, al_map_rgb(30, 255, 30), 5);
	al_draw_arc(0, 0, 10, 0, (energy / maxEnergy) * M_PI * 2, al_map_rgb(30, 30, 255), 5);
}

