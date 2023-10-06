#include "Mouth.h"

#include "BodyPart.h"
#include "BodySegment.h"
#include "Creature.h"

#include "../../Util.h"
#include "../../UI/Camera.h"
#include <allegro5/allegro_primitives.h>
#include <box2d/b2_math.h>
#include <memory>

#include "../UserData/ObjectUserData.h"
#include "../../GameManager.h"


Mouth::Mouth(shared_ptr<Creature> parentCreature, shared_ptr<BodySegment> parentPart, b2Vec2 pixelSize, ALLEGRO_COLOR color, float angleOnParent, float angleOffset, Joint::JointInfo jointInfo, NerveInfo &nerveInfo) :
	BodyPart(
		parentCreature,
		color,
		nerveInfo
	) {

	this->triggerBiteDamage = false;
	this->cooldownTimer = 0;
	this->animationRate = 0.2;
	this->animationFrame = 0;
	this->animationState = 0;
	this->polymorphic_id = "Mouth";
	this->biting = false;
	this->parentPart = parentPart;

	this->shapeType = SHAPE_TYPES::RECT;
	this->pixelSize = b2Vec2(15, 10);
	this->worldSize = Util::pixelsToMeters(this->pixelSize);

	b2BodyDef bodyDef;
	b2FixtureDef fixtureDef;
	b2PolygonShape rectShapeDef;
	rectShapeDef.SetAsBox(worldSize.x, worldSize.y);


	bodyDef.type = b2_dynamicBody;
	bodyDef.position = Util::pixelsToMeters(GetPosOnParent(parentPart, angleOnParent, angleOffset, this->worldSize));
	bodyDef.angle = parentPart->GetBody()->GetAngle() - (angleOffset + angleOnParent) - M_PI_2;
	bodyDef.linearDamping = 0.1;
	bodyDef.angularDamping = 0.1;
	bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(this->objectUserData.get());

	this->body = GameManager::world.CreateBody(&bodyDef);

	fixtureDef.shape = &rectShapeDef;
	fixtureDef.isSensor = true;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.3f;
	fixtureDef.restitution = 0.5f;
	fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(this->objectUserData.get());

	body->CreateFixture(&fixtureDef);



	// joint together
	b2Vec2 jointPos = parentPart->GetEdgePoint(-angleOnParent + parentPart->GetBody()->GetAngle());

	shared_ptr<Joint> newJoint = make_shared<Joint>(Joint(jointInfo, jointPos, body, parentPart->GetBody()));
	SetParentJoint(newJoint);
}

void Mouth::Update() {
	if (!parentJoint)
		alive = false;

	UpdateJoint();

	if (creature.expired())
		return;

	if (animationState > 0.1) {
		animationFrame += animationRate;
		animationState = abs(sin(animationFrame));
		return;
	}

	if (cooldownTimer > 0)
		cooldownTimer--;

	if (!biting)
		return;

	biting = false;

	// Animation is over. trigger bite
	for (b2ContactEdge* ce = body->GetContactList(); ce; ce = ce->next) {
		b2Contact* contact = ce->contact;
		if (!contact->IsTouching())
			continue;

		shared_ptr<Object> otherObject;

		ObjectUserData *userDataA = reinterpret_cast<ObjectUserData *>(contact->GetFixtureA()->GetUserData().pointer);
		ObjectUserData *userDataB = reinterpret_cast<ObjectUserData *>(contact->GetFixtureB()->GetUserData().pointer);

		if (userDataA && userDataA->parentObject.lock() && userDataA->parentObject.lock().get() != this)
			otherObject = userDataA->parentObject.lock();
		if (userDataB && userDataB->parentObject.lock() && userDataB->parentObject.lock().get() != this)
			otherObject = userDataB->parentObject.lock();

		if (!otherObject)
			continue;

		if (shared_ptr<BodyPart> bodyPart = dynamic_pointer_cast<BodyPart>(otherObject)) {
			if (bodyPart->GetParentCreature().lock() == this->GetParentCreature().lock())
				continue;

			cout << "doing damage to " << bodyPart->GetType() << endl;
			bodyPart->TakeDamage(5);
			break;
		}

	}

}

void Mouth::Draw() {
	//Object::Draw();

	float angle = body->GetAngle();
	b2Vec2 pos = Util::metersToPixels(body->GetPosition());
	b2Vec2 origin(pixelSize.y * sin(-angle), pixelSize.y * cos(-angle));

	pos -= origin;

	ALLEGRO_TRANSFORM t;
	al_identity_transform(&t);

	al_rotate_transform(&t, angle);
	al_translate_transform(&t, pos.x, pos.y);
	al_compose_transform(&t, &Camera::transform);

	al_use_transform(&t);

	al_draw_filled_triangle(0, 0, 10, -5, 15 * animationState, 20, al_map_rgb(255, 0, 0));
	al_draw_filled_triangle(0, 0, -10, -5, -15 * animationState, 20, al_map_rgb(255, 0, 0));

	//al_draw_arc(0, 0, 25, angle, M_PI_2, al_map_rgb(255, 255, 255), 2);

}


void Mouth::UpdateJoint() {
	BodyPart::UpdateJoint();
	if (!parentJoint || parentJoint->IsBroken())
		alive = false;
}


float Mouth::GetNerveOutput() {
	return 0;
}

void Mouth::SetNerveInput(float val) {
	if (val > 0 && CanBite()) {
		biting = true;
		animationState = 0.15;
		cooldownTimer = biteCooldown;
	}
}

bool Mouth::CanBite() {
	return cooldownTimer <= 0;
}
