#include "UserInput.h"
#include "Util.h"

#include <box2d/b2_body.h>
#include <box2d/b2_circle_shape.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_math.h>

#include <cstdint>
#include <memory>
#include <iostream>

#include "GameManager.h"
#include "ObjectUserData.h"
#include "Camera.h"

using namespace std;


namespace UserInput {
	unordered_map<int, bool> keyStates;
	b2Vec2 dragStartPos;
	b2Vec2 mousePos;
	bool isDragging;
	int mouseWheel;
	b2Body *b2MouseObject;
	weak_ptr<Object> hoveredObject;


	void Init() {
		dragStartPos = b2Vec2();
		mousePos = b2Vec2();
		isDragging = false;
		mouseWheel = 0;

		ObjectUserData *objectUserData = new ObjectUserData();
		objectUserData->objectType = "mouse";

		b2BodyDef bodyDef;
		bodyDef.position = b2Vec2_zero;
		bodyDef.type = b2_staticBody;
		bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(objectUserData);

		b2MouseObject = GameManager::world.CreateBody(&bodyDef);

		b2CircleShape circleShapeDef;
		circleShapeDef.m_radius = Util::pixelsToMeters(5);

		b2FixtureDef fixtureDef;
		fixtureDef.isSensor = true;
		fixtureDef.shape = &circleShapeDef;
		fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(objectUserData);

		b2MouseObject->CreateFixture(&fixtureDef);
	}

	void StartDragging(float x, float y) {
		StartDragging(b2Vec2(x, y));
	}

	void StartDragging(b2Vec2 pos) {
		isDragging = true;
		dragStartPos = pos;
	}

	void StopDragging() {
		isDragging = false;
	}

	void SetPressed(int key, bool pressed) {
		keyStates[key] = pressed;
	}

	bool IsPressed(int key) {
		return keyStates[key];
	}

	void SetMousePos(b2Vec2 pos) {
		mousePos = pos;
		b2Vec2 screenWorldPos = Camera::ScreenPos2WorldPos(b2Vec2(mousePos.x, mousePos.y));
		b2Vec2 worldPos = Util::pixelsToMeters(screenWorldPos);
		b2MouseObject->SetTransform(worldPos, 0);
	}

	void SetMousePos(float x, float y) {
		mousePos = b2Vec2(x, y);
	}

	void ClearHoveredObject() {
		hoveredObject.reset();
	}

	void SetHoveredObject(weak_ptr<Object> obj) {
		hoveredObject = obj;
	}
}

