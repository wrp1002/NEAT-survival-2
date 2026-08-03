#include "UserInput.h"
#include "../Util.h"

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


	void Init() {
		dragStartPos = b2Vec2();
		mousePos = b2Vec2();
		isDragging = false;
		mouseWheel = 0;
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
		//b2Vec2 screenWorldPos = Camera::ScreenPos2WorldPos(b2Vec2(mousePos.x, mousePos.y));
		//b2Vec2 worldPos = Util::pixelsToMeters(screenWorldPos);
		//b2MouseObject->SetTransform(worldPos, 0);
	}

	void SetMousePos(float x, float y) {
		mousePos = b2Vec2(x, y);
	}

	shared_ptr<Object> GetObjectAtMouse() {
		b2Vec2 worldPos = Util::pixelsToMeters(Camera::ScreenPos2WorldPos(mousePos));
		return MousePicker::GetObjectAt(worldPos);
	}

}

