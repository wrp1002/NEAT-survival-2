#include "Cilium.h"
#include "BodyPart.h"

#include "../../Util.h"
#include "../../UI/Camera.h"
#include <Box2D/Box2D.h>

Cilium::Cilium(shared_ptr<Creature> parentCreature, shared_ptr<BodySegment> parentPart, b2Vec2 pixelSize, ALLEGRO_COLOR color, float angleOnParent, float angleOffset, Joint::JointInfo jointInfo, NerveInfo &nerveInfo) :
	BodyPart(
		parentCreature,
		GetPosOnParent(parentPart, angleOnParent, angleOffset, Util::pixelsToMeters(b2Vec2(10, 10))),
		b2Vec2(10, 20),
		parentPart->GetBody()->GetAngle() - (angleOffset + angleOnParent) - M_PI_2,
		color,
		Object::SHAPE_TYPES::RECT,
		nerveInfo
		) {


	this->polymorphic_id = "Cilium";
	this->maxSpeed = 2;
	this->currentSpeed = 0;
	this->parentPart = parentPart;

	// joint together
	b2Vec2 jointPos = parentPart->GetEdgePoint(-angleOnParent + parentPart->GetBody()->GetAngle());

	shared_ptr<Joint> newJoint = make_shared<Joint>(Joint(jointInfo, jointPos, body, parentPart->GetBody()));
	SetParentJoint(newJoint);
}

void Cilium::Update() {
	UpdateJoint();

	if (creature.expired())
		return;

	double requestedEnergy = 0.005 * currentSpeed;

	// Not enough energy, so do nothing
	if (creature.lock()->GetUsableEnergy() < requestedEnergy) {
		energyUsage = 0;
		return;
	}

	this->animationAngle = body->GetAngle() + sin(al_get_time() * currentSpeed);

	this->body->ApplyForce(currentSpeed * b2Vec2(cos(body->GetAngle() - M_PI_2), sin(body->GetAngle() - M_PI_2)), body->GetPosition(), true);

	this->energyUsage = requestedEnergy;
}

void Cilium::Draw() {
	//Object::Draw();
	b2Vec2 pos = Util::metersToPixels(body->GetPosition());
	float angle = body->GetAngle();
	b2Vec2 origin((pixelSize.y / 2) * sin(-angle), pixelSize.y / 2 * cos(-angle));

	pos -= origin;

	ALLEGRO_TRANSFORM t;
	al_identity_transform(&t);

	al_rotate_transform(&t, animationAngle);
	al_translate_transform(&t, pos.x, pos.y);
	al_compose_transform(&t, &Camera::transform);

	al_use_transform(&t);

	al_draw_filled_circle(0, 0, 2, al_map_rgb(255, 0, 255));
	al_draw_line(0, 0, 0, pixelSize.y, al_map_rgb(255, 255, 255), 2);

	//al_draw_arc(0, 0, 25, angle, M_PI_2, al_map_rgb(255, 255, 255), 2);

}

void Cilium::UpdateJoint() {
	BodyPart::UpdateJoint();
	if (!parentJoint || parentJoint->IsBroken())
		alive = false;
}

float Cilium::GetNerveOutput() {
	return 0;
}

void Cilium::SetNerveInput(float val) {
	this->currentSpeed = abs(val) * maxSpeed;
}
