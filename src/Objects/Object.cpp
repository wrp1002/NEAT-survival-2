#include "Object.h"

#include <allegro5/allegro_primitives.h>
#include <allegro5/color.h>

#include <Box2D/Box2D.h>

#include "../UI/Camera.h"
#include "UserData/ObjectUserData.h"
#include "../Util.h"

#include <cstdint>
#include <iostream>
#include <memory>

#include "../GameManager.h"

using namespace std;


Object::Object() {
	this->pixelSize = b2Vec2(10, 10);
	this->worldSize = Util::pixelsToMeters(pixelSize);
	this->color = al_map_rgb(255, 0, 255);
	this->polymorphic_id = "Object";
	this->shapeType = SHAPE_TYPES::CIRCLE;
	this->objectUserData = make_shared<ObjectUserData>(ObjectUserData());
	this->alive = true;
}

Object::Object(b2Vec2 pos, b2Vec2 pixelSize, float angle, ALLEGRO_COLOR color, int shapeType) {
	this->pixelSize = pixelSize;
	this->worldSize = Util::pixelsToMeters(pixelSize);
	this->color = color;
	this->polymorphic_id = "Object";
	this->shapeType = shapeType;
	this->objectUserData = make_shared<ObjectUserData>(ObjectUserData());
	this->alive = true;

	b2BodyDef bodyDef;
	b2FixtureDef fixtureDef;
	b2CircleShape circleShapeDef; circleShapeDef.m_radius = worldSize.y;
	b2PolygonShape rectShapeDef;  rectShapeDef.SetAsBox(worldSize.x, worldSize.y);


	bodyDef.type = b2_dynamicBody;
	bodyDef.position = Util::pixelsToMeters(pos);
	bodyDef.angle = angle;
	bodyDef.linearDamping = 0.1;
	bodyDef.angularDamping = 0.1;
	//bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(this->objectUserData.get());
	bodyDef.userData = (void*)this->objectUserData.get();

	this->body = GameManager::world.CreateBody(&bodyDef);

	if (shapeType == SHAPE_TYPES::CIRCLE)
		fixtureDef.shape = &circleShapeDef;
	else if (shapeType == SHAPE_TYPES::RECT)
		fixtureDef.shape = &rectShapeDef;

	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.3f;
	fixtureDef.restitution = 0.5f;
	//fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(this->objectUserData.get());
	fixtureDef.userData = (void*)this->objectUserData.get();

	body->CreateFixture(&fixtureDef);
}

void Object::UpdateObjectUserData() {
	//cout << "update object data " << polymorphic_id << endl;
	objectUserData->parentObject = shared_from_this();
	objectUserData->objectType = polymorphic_id;
}


void Object::Update() {

}

void Object::Draw() {
	float angle = body->GetAngle();
	b2Vec2 pos = Util::metersToPixels(body->GetPosition());

	ALLEGRO_TRANSFORM t;
	al_identity_transform(&t);

	al_rotate_transform(&t, angle);
	al_translate_transform(&t, pos.x, pos.y);
	al_compose_transform(&t, &Camera::transform);

	al_use_transform(&t);

	switch (this->shapeType) {
		case SHAPE_TYPES::CIRCLE: {
			al_draw_filled_circle(0, 0, pixelSize.y, color);
			break;
		}
		case SHAPE_TYPES::RECT: {
			al_draw_filled_rounded_rectangle(-pixelSize.x, -pixelSize.y, pixelSize.x, pixelSize.y, 10, 10, color);
			break;
		}
	}

	al_draw_line(0, 0, 0, -pixelSize.y / 2, al_map_rgb(255, 255, 255), 2);
}

void Object::Destroy() {
	GameManager::world.DestroyBody(this->body);
}

void Object::Print() {
	cout << polymorphic_id << " selected " << this << endl;
}


void Object::ApplyForce(b2Vec2 force) {
	body->ApplyForce(force, body->GetPosition(), true);
}

void Object::Kill() {
	alive = false;
}


// Raycast from a point from this angle towards the object to find collision point on edge
b2Vec2 Object::GetEdgePoint(float angle) {
	b2Transform transform;
	transform.SetIdentity();

	b2Vec2 pos = body->GetPosition();
	float distance = max(worldSize.x, worldSize.y) * 2;

	b2RayCastInput input;
	input.p1 = pos + distance * b2Vec2(cos(angle), sin(angle));
	input.p2.Set(pos.x, pos.y);

	input.maxFraction = 1.0;
	int32 childIndex = 0;

	b2Fixture fixture = body->GetFixtureList()[0];

	b2RayCastOutput output;
	bool hit = fixture.RayCast(&output, input, childIndex);

	if (hit)
	{
		b2Vec2 hitPoint = input.p1 + output.fraction * (input.p2 - input.p1);
		return hitPoint;
	}

	return b2Vec2(0, 0);
}

b2Vec2 Object::GetPos() {
	return body->GetPosition();
}

b2Vec2 Object::GetPosPX() {
	return Util::metersToPixels(body->GetPosition());
}

string Object::GetType() {
	return polymorphic_id;
}


bool Object::IsAlive() {
	return alive;
}
