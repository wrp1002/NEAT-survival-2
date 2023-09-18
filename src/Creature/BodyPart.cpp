#include "BodyPart.h"

#include <box2d/b2_math.h>

#include "../Object.h"
#include "../Util.h"

#include "BodySegment.h"


BodyPart::BodyPart(shared_ptr<Creature> parentCreature, b2Vec2 pos, b2Vec2 pixelSize, float angle, ALLEGRO_COLOR color, int shapeType, NerveInfo &nerveInfo) : Object(pos, pixelSize, angle, color, shapeType) {
	this->nerveInfo = nerveInfo;
	this->creature = parentCreature;
	this->polymorphic_id = "BodyPart";
}

string BodyPart::GetPolymorphicID() {
	return polymorphic_id;
}


b2Vec2 BodyPart::GetPosOnParent(shared_ptr<BodyPart> otherObject, float angleOnObject, float angleOffset, b2Vec2 thisWorldSize) {
	b2Vec2 parentEdgePos = otherObject->GetEdgePoint(-angleOnObject + otherObject->body->GetAngle());

	float angle = otherObject->body->GetAngle() - (angleOffset + angleOnObject);

	b2Vec2 relPos = b2Vec2(
		cos(angle) * thisWorldSize.y,
		sin(angle) * thisWorldSize.y
	);

	b2Vec2 pos = parentEdgePos + relPos ;
	return Util::metersToPixels(pos);
}


void BodyPart::Update() {

}

void BodyPart::Draw() {
	Object::Draw();
}


b2Body *BodyPart::GetBody() {
	return this->body;
}

weak_ptr<Creature> BodyPart::GetParentCreature() {
	return creature;
}

bool BodyPart::CanAddChild() {
	return false;
}

bool BodyPart::childAngleValid(int angle) {
	return false;
}

int BodyPart::GetValidChildAngle(int angleGene) {
	return 0;
}

void BodyPart::AddChild(shared_ptr<BodyPart> child, int angle) {

}


float BodyPart::GetNerveOutput() {
	if (!parentJoint)
		return 0;

	return parentJoint->GetAngle();
}

int BodyPart::GetNerveOutputIndex() {
	return this->nerveInfo.outputIndex;
}

int BodyPart::GetNerveInputIndex() {
	return nerveInfo.inputIndex;
}

void BodyPart::SetNerveInput(float val) {
	if (!parentJoint)
		return;

	parentJoint->SetSpeed(val);
}

