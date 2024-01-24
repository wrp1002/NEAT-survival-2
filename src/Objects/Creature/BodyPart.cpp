#include "BodyPart.h"

#include <Box2D/Box2D.h>

#include "../../Util.h"
#include "../../GameManager.h"
#include "Joint.h"


BodyPart::BodyPart(shared_ptr<Creature> parentCreature, ALLEGRO_COLOR color, NerveInfo &nerveInfo) : Object() {
	this->creature = parentCreature;
	this->nerveInfo = nerveInfo;
	this->polymorphic_id = "BodyPart";
	this->color = color;

	this->maxHealth = 20;
	this->health = 1;

	this->energyUsage = 0;
}

BodyPart::BodyPart(shared_ptr<Creature> parentCreature, b2Vec2 pos, b2Vec2 pixelSize, float angle, ALLEGRO_COLOR color, int shapeType, NerveInfo &nerveInfo) : Object(pos, pixelSize, angle, color, shapeType) {
	this->nerveInfo = nerveInfo;
	this->creature = parentCreature;
	this->polymorphic_id = "BodyPart";
	this->color = color;

	this->maxHealth = 20;
	this->health = 1;

	this->energyUsage = 0;
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
	if (parentJoint && parentJoint->IsBroken()) {
		parentJoint.reset();
	}

	if (health <= 0)
		alive = false;
}

void BodyPart::Draw() {
	Object::Draw();

}

void BodyPart::Destroy() {
	GameManager::world.DestroyBody(this->body);
}

void BodyPart::Print() {
	Object::Print();
	cout << "Nerve Info:" << endl;
	cout << "inputEnabled: " << nerveInfo.inputEnabled << "   inputIndex: " << nerveInfo.inputIndex << endl;
	cout << "outputEnabled: " << nerveInfo.outputEnabled << "   outputIndex: " << nerveInfo.outputIndex << endl;
}


b2Body *BodyPart::GetBody() {
	return this->body;
}

weak_ptr<Creature> BodyPart::GetParentCreature() {
	return creature;
}

double BodyPart::TakeDamage(double amount) {
	amount = min(amount, health);
	this->health -= amount;

	return amount;
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


bool BodyPart::NerveInputEnabled() {
	return this->nerveInfo.inputEnabled;
}

bool BodyPart::NerveOutputEnabled() {
	return this->nerveInfo.outputEnabled;
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
	return this->nerveInfo.inputIndex;
}

void BodyPart::SetNerveInput(float val) {
	if (!parentJoint)
		return;

	parentJoint->SetSpeed(val);
}


double BodyPart::SetHealth(double amount) {
	double extra = 0;
	this->health = amount;
	if (health > maxHealth) {
		extra = health - maxHealth;
		health -= extra;
	}
	return extra;
}

double BodyPart::GetEnergyUsage() {
	return energyUsage + 0.001;
}

double BodyPart::GetHealth() {
	return health;
}
