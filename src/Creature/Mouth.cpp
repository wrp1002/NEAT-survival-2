#include "Mouth.h"

#include "BodySegment.h"
#include "Creature.h"

#include "../Util.h"
#include "../Camera.h"
#include <allegro5/allegro_primitives.h>
#include <box2d/b2_math.h>

Mouth::Mouth(shared_ptr<Creature> parentCreature, shared_ptr<BodySegment> parentPart, b2Vec2 pixelSize, ALLEGRO_COLOR color, float angleOnParent, float angleOffset, Joint::JointInfo jointInfo, NerveInfo &nerveInfo) :
	BodyPart(
		parentCreature,
		GetPosOnParent(parentPart, angleOnParent, 0, Util::pixelsToMeters(b2Vec2(10, 10))),
		b2Vec2(15, 10),
		parentPart->GetBody()->GetAngle() - (angleOnParent) - M_PI_2,
		color,
		Object::SHAPE_TYPES::RECT,
		nerveInfo
	) {

	this->animationRate = 0.1;
	this->animationFrame = 0;
	this->animationState = 0;
	this->polymorphic_id = "Mouth";

	// joint together
	b2Vec2 jointPos = parentPart->GetEdgePoint(-angleOnParent + parentPart->GetBody()->GetAngle());

	shared_ptr<Joint> newJoint = make_shared<Joint>(Joint(jointInfo, jointPos, body, parentPart->GetBody()));
	if (shared_ptr<Creature> creaturePtr = creature.lock())
		creaturePtr->AddJoint(newJoint);
	this->parentJoint = newJoint;
}

void Mouth::Update() {
	if (creature.expired())
		return;

	animationFrame += animationRate;
	animationState = abs(sin(animationFrame));
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


float Mouth::GetNerveOutput() {
	return 0;
}

void Mouth::SetNerveInput(float val) {

}