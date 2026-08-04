#include "UserInput.h"
#include "../Util.h"

#include <Box2D/Common/b2Math.h>
#include <Box2D/Dynamics/Joints/b2MouseJoint.h>
#include <cstdint>
#include <memory>
#include <iostream>

#include "../GameManager.h"
#include "../Objects/UserData/ObjectUserData.h"
#include "../UI/Camera.h"
#include "../UI/Camera.h"
#include "MousePicker.h"

using namespace std;


namespace UserInput {
	unordered_map<int, bool> keyStates;
	b2Vec2 dragStartPos;
	b2Vec2 mousePos;
	bool isDragging;
	int mouseWheel;
	b2MouseJoint* mouseJoint;


	void Init() {
		dragStartPos = b2Vec2();
		mousePos = b2Vec2();
		isDragging = false;
		mouseWheel = 0;
	}

	void Update() {
		if (mouseJoint)
			mouseJoint->SetTarget(Util::pixelsToMeters(Camera::ScreenPos2WorldPos(mousePos)));
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

	void StartObjectDragging(shared_ptr<Object> object) {
		b2Vec2 mouseWorldPos = Util::pixelsToMeters(Camera::ScreenPos2WorldPos(mousePos));

		if (!object)
			return;

		b2Body* body = object->GetBody();

		if (!body)
			return;

		b2MouseJointDef jointDef;

		jointDef.bodyA = GameManager::mouseGroundBody;
		jointDef.bodyB = body;
		jointDef.target = mouseWorldPos;

		jointDef.maxForce = 1000.0f * body->GetMass();

		jointDef.frequencyHz = 5.0f;
		jointDef.dampingRatio = 0.7f;

		mouseJoint = (b2MouseJoint*)GameManager::world.CreateJoint(&jointDef);

		body->SetAwake(true);
	}

	void StopObjectDragging() {
		if (mouseJoint) {
			GameManager::world.DestroyJoint(mouseJoint);
			mouseJoint = nullptr;
		}
	}

	void SetPressed(int key, bool pressed) {
		keyStates[key] = pressed;
	}

	bool IsPressed(int key) {
		return keyStates[key];
	}

	void SetMousePos(b2Vec2 pos) {
		mousePos = pos;
	}

	void SetMousePos(float x, float y) {
		mousePos = b2Vec2(x, y);
	}

	void DrawMouseJoint() {
		if (mouseJoint) {
			b2Vec2 target = UserInput::mouseJoint->GetTarget();

			al_draw_line(
				Util::metersToPixels(mouseJoint->GetAnchorB().x),
				Util::metersToPixels(mouseJoint->GetAnchorB().y),
				Util::metersToPixels(target.x),
				Util::metersToPixels(target.y),
				al_map_rgb(100,100,100),
				2
			);
		}
	}

	shared_ptr<Object> GetObjectAtMouse() {
		b2Vec2 worldPos = Util::pixelsToMeters(Camera::ScreenPos2WorldPos(mousePos));
		return MousePicker::GetObjectAt(worldPos);
	}

}

