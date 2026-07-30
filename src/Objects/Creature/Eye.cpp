#include "Eye.h"

#include <allegro5/allegro_primitives.h>

#include "BodyPart.h"
#include "Creature.h"
#include "BodySegment.h"

#include "../../Util.h"
#include "../../GameManager.h"
#include "../../UI/Camera.h"

Eye::Eye(shared_ptr<Creature> parentCreature, shared_ptr<BodySegment> parentPart, b2Vec2 pixelSize, ALLEGRO_COLOR color, float angleOnParent, float angleOffset, Joint::JointInfo jointInfo, vector<NerveInfo> &nerveInfo, EyeInfo &eyeInfo) :
	BodyPart(
		parentCreature,
		color,
		nerveInfo
	) {

	this->polymorphic_id = "Eye";

	this->shapeType = SHAPE_TYPES::RECT;
	this->pixelSize = b2Vec2(10, 10);
	this->worldSize = Util::pixelsToMeters(this->pixelSize);
	this->seesObject = false;
	this->visionDirection = 0;
	this->visionStrength = 0;
	this->parentPart = parentPart;
	this->eyeInfo = eyeInfo;

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
	fixtureDef.isSensor = false;
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

void Eye::UpdateVisionValues() {
    float totalSignal = 0;
    float weightedDirection = 0;

    if (rayValues.empty())
        return;


    for (int i = 0; i < rayValues.size(); i++) {
        float value = rayValues[i];

        // Convert ray index into -1 to +1
        // left = -1, center = 0, right = +1
        float rayPosition = 0;

        if (rayValues.size() > 1) {
            rayPosition =
                (float)i / (rayValues.size() - 1);

            rayPosition =
                rayPosition * 2.0f - 1.0f;
        }


        totalSignal += value;

        weightedDirection += value * rayPosition;
    }


    // Strength of the thing being seen
    visionStrength = std::min(totalSignal, 1.0f);

    // Direction weighted by what was seen
    if (totalSignal > 0) {
        visionDirection =
            weightedDirection / totalSignal;

        // make peripheral objects less important
        visionDirection *= visionStrength;
    }
    else
    {
        visionDirection = 0;
    }
}

void Eye::Update() {
	LiveObject::Update();
	UpdateJoint();

	if (creature.expired())
		return;

	if (visionTimer == 0) {
		rayValues.assign(eyeInfo.rayCount, 0.0f);

		float startAngle = -eyeInfo.fov * 0.5f;
		float step = eyeInfo.rayCount > 1 ? eyeInfo.fov / (eyeInfo.rayCount - 1) : 0;

		for (int i = 0; i < eyeInfo.rayCount; i++) {

			float localAngle =
				Util::DegreesToRadians(
					startAngle + step * i
				);

			// Eye points along local +Y
			// Local eye direction
			b2Vec2 direction(
				sin(localAngle),
				cos(localAngle)
			);

			// Convert to world direction
			direction = body->GetWorldVector(direction);

			// Convert range to meters
			float rangeMeters = Util::pixelsToMeters(eyeInfo.range);

			// Make ray
			b2Vec2 start = body->GetWorldPoint(b2Vec2(0, 0));
			b2Vec2 end = start + rangeMeters * direction;

			EyeRayCastCallback callback;
			GameManager::world.RayCast(
				&callback,
				start,
				end
			);

			if (callback.hitObject) {

				rayValues[i] =
					1.0f - callback.closestFraction;

				seesObject = true;
			}
			else {
				rayValues[i] = 0;
			}
		}

		UpdateVisionValues();
		visionTimer = visionTimerStart;
	}
	visionTimer--;
}


void Eye::Draw() {
	float angle = body->GetAngle();

	b2Vec2 pos = Util::metersToPixels(body->GetPosition());

	// Offset so the eye is drawn from its base/tip correctly
	b2Vec2 origin(
		pixelSize.y * sin(-angle),
		pixelSize.y * cos(-angle)
	);

	pos -= origin;

	ALLEGRO_TRANSFORM t;
	al_identity_transform(&t);

	al_rotate_transform(&t, angle);
	al_translate_transform(&t, pos.x, pos.y);
	al_compose_transform(&t, &Camera::transform);

	al_use_transform(&t);

	// Draw rays
	if (eyeInfo.rayCount > 0) {

		float startAngle = -eyeInfo.fov * 0.5f;
		float step = (eyeInfo.rayCount > 1) ? eyeInfo.fov / (eyeInfo.rayCount - 1) : 0;

		for (int i = 0; i < eyeInfo.rayCount; i++) {
			float localAngle = startAngle + step * i;

			// Eye points along local +Y, not +X
			float radians = Util::DegreesToRadians(localAngle);

			float dx = sin(radians);
			float dy = cos(radians);

			// Draw ray in local eye space
			al_draw_line(
				0,
				0,
				dx * eyeInfo.range,
				dy * eyeInfo.range,
				al_map_rgba(100, 100, 100, 60),
				1
			);

			if (i < rayValues.size() && rayValues[i] > 0.0f) {

				float hitDistance = eyeInfo.range * (1.0f - rayValues[i]);

				float hx = dx * hitDistance;
				float hy = dy * hitDistance;

				float brightness = std::min(rayValues[i], 1.0f);

				al_draw_filled_circle(
					hx,
					hy,
					3,
					al_map_rgba_f(
						brightness,
						brightness,
						brightness,
						1.0f
					)
				);
			}
		}
	}

	// Draw eye
	al_draw_filled_circle(
		0,
		0,
		10,
		al_map_rgb(255, 255, 255)
	);
	// Pupil
	al_draw_filled_circle(
		0,
		5,
		5,
		color
	);
}


void Eye::UpdateJoint() {
	BodyPart::UpdateJoint();
	if (!parentJoint || parentJoint->IsBroken())
		alive = false;
}


float Eye::GetNerveOutput(NerveType type) {
	//cout << "get eye output" << visionStrength << " " << visionDirection << endl;
    switch(type) {
        case NerveType::Activation:
            return visionStrength;

        case NerveType::Direction:
            return visionDirection;

        default:
            return 0;
    }
}

void Eye::SetNerveInput(NerveType type, float val) {

}
