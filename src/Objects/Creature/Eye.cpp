#include "Eye.h"

#include <allegro5/allegro_primitives.h>

#include "BodyPart.h"
#include "Creature.h"
#include "BodySegment.h"

#include "../../Util.h"
#include "../../GameManager.h"
#include "../../UI/Camera.h"

Eye::Eye(shared_ptr<Creature> parentCreature, shared_ptr<BodySegment> parentPart, b2Vec2 pixelSize, ALLEGRO_COLOR color, float angleOnParent, float angleOffset, Joint::JointInfo jointInfo, NerveInfo &nerveInfo) :
	BodyPart(
		parentCreature,
		color,
		nerveInfo
	) {

	this->polymorphic_id = "Eye";

	this->shapeType = SHAPE_TYPES::RECT;
	this->pixelSize = b2Vec2(10, 100);
	this->worldSize = Util::pixelsToMeters(this->pixelSize);
	this->seesObject = false;
	this->parentPart = parentPart;

	b2BodyDef bodyDef;
	b2FixtureDef fixtureDef;
	b2PolygonShape rectShapeDef;
	rectShapeDef.SetAsBox(worldSize.x, worldSize.y);


	bodyDef.type = b2_dynamicBody;
	bodyDef.position = Util::pixelsToMeters(GetPosOnParent(parentPart, angleOnParent, angleOffset, this->worldSize));
	bodyDef.angle = parentPart->GetBody()->GetAngle() - (angleOffset + angleOnParent) - M_PI_2;
	bodyDef.linearDamping = 0.1;
	bodyDef.angularDamping = 0.1;
	//bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(this->objectUserData.get());
	bodyDef.userData = (void*)this->objectUserData.get();

	this->body = GameManager::world.CreateBody(&bodyDef);

	fixtureDef.shape = &rectShapeDef;
	fixtureDef.isSensor = true;
	fixtureDef.density = 0.01f;
	fixtureDef.friction = 0.3f;
	fixtureDef.restitution = 0.5f;
	//fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(this->objectUserData.get());
	bodyDef.userData = (void*)this->objectUserData.get();

	body->CreateFixture(&fixtureDef);



	// joint together
	b2Vec2 jointPos = parentPart->GetEdgePoint(-angleOnParent + parentPart->GetBody()->GetAngle());
	shared_ptr<Joint> newJoint = make_shared<Joint>(Joint(jointInfo, jointPos, body, parentPart->GetBody()));
	SetParentJoint(newJoint);
}


void Eye::Update() {
	LiveObject::Update();
	UpdateJoint();

	if (creature.expired())
		return;

	seesObject = false;

	for (b2ContactEdge* ce = body->GetContactList(); ce; ce = ce->next) {
		b2Contact* contact = ce->contact;
		if (!contact->IsTouching())
			continue;

		shared_ptr<Object> otherObject;

		ObjectUserData *userDataA = reinterpret_cast<ObjectUserData *>(contact->GetFixtureA()->GetUserData());
		ObjectUserData *userDataB = reinterpret_cast<ObjectUserData *>(contact->GetFixtureB()->GetUserData());

		if (userDataA && userDataA->parentObject.lock() && userDataA->parentObject.lock().get() != this)
			otherObject = userDataA->parentObject.lock();
		if (userDataB && userDataB->parentObject.lock() && userDataB->parentObject.lock().get() != this)
			otherObject = userDataB->parentObject.lock();

		if (!otherObject)
			continue;

		if (otherObject->GetType() == "Eye")
			continue;

		if (shared_ptr<BodyPart> bodyPart = dynamic_pointer_cast<BodyPart>(otherObject)) {
			if (bodyPart->GetParentCreature().lock() == this->GetParentCreature().lock())
				continue;

			seesObject = true;
			break;
		}
		else if (shared_ptr<LiveObject> bodyPart = dynamic_pointer_cast<LiveObject>(otherObject)) {
			seesObject = true;
			break;
		}

	}
}


void Eye::Draw() {
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

	ALLEGRO_COLOR drawColor = al_map_rgba(50, 50, 50, 50);
	if (seesObject)
		drawColor = al_map_rgba(255, 255, 255, 50);

	al_draw_line(0, 0, 0, pixelSize.y * 2, drawColor, pixelSize.x * 2);
	al_draw_filled_circle(0, 0, 10, al_map_rgb(255, 255, 255));
	al_draw_filled_circle(0, 5, 5, color);

}


void Eye::UpdateJoint() {
	BodyPart::UpdateJoint();
	if (!parentJoint || parentJoint->IsBroken())
		alive = false;
}


float Eye::GetNerveOutput() {
	return seesObject;
}

void Eye::SetNerveInput(float val) {

}
