#include "BodyPart.h"

#include <box2d/b2_math.h>

#include "../Object.h"
#include "../Util.h"
#include "../GameManager.h"
#include "Joint.h"


BodyPart::BodyPart(shared_ptr<Creature> parentCreature, ALLEGRO_COLOR color, NerveInfo &nerveInfo) : Object() {
	this->creature = parentCreature;
	this->nerveInfo = nerveInfo;
	this->polymorphic_id = "BodyPart";
	this->color = color;

	this->maxHealth = 5;
	this->health = maxHealth;
	this->maxEnergy = 5;
	this->energy = maxEnergy;
}

BodyPart::BodyPart(shared_ptr<Creature> parentCreature, b2Vec2 pos, b2Vec2 pixelSize, float angle, ALLEGRO_COLOR color, int shapeType, NerveInfo &nerveInfo) : Object(pos, pixelSize, angle, color, shapeType) {
	this->nerveInfo = nerveInfo;
	this->creature = parentCreature;
	this->polymorphic_id = "BodyPart";
	this->color = color;

	this->maxHealth = 5;
	this->health = maxHealth;
	this->maxEnergy = 5;
	this->energy = maxEnergy;
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

void BodyPart::UpdateJoint() {
	if (!this->parentJoint)
		return;

	this->parentJoint->Update();
}

void BodyPart::SetParentJoint(shared_ptr<Joint> newJoint) {
	this->parentJoint = newJoint;
	newJoint->UpdateUserData();
}

void BodyPart::DestroyJoint() {
	if (!parentJoint)
		return;

	parentJoint->Destroy();
	parentJoint = nullptr;
}


void BodyPart::Update() {
	if (parentJoint && parentJoint->IsBroken())
		parentJoint.reset();

	if (health <= 0)
		alive = false;
}

void BodyPart::Draw() {
	Object::Draw();

}

void BodyPart::Destroy() {
	cout << "DESTROY!" << endl;
	GameManager::world.DestroyBody(this->body);
}


b2Body *BodyPart::GetBody() {
	return this->body;
}

weak_ptr<Creature> BodyPart::GetParentCreature() {
	return creature;
}

void BodyPart::TakeDamage(double amount) {
	this->health -= amount;
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


double BodyPart::GetEnergy() {
	return energy;
}
