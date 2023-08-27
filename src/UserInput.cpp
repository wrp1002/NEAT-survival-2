#include "UserInput.h"

#include <box2d/b2_math.h>


namespace UserInput {
	unordered_map<int, bool> keyStates;
	b2Vec2 dragStartPos;
	b2Vec2 mousePos;
	bool isDragging;
	int mouseWheel;


	void Init() {
		dragStartPos = b2Vec2();
		mousePos = b2Vec2();;
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
	}

	void SetMousePos(float x, float y) {
		mousePos = b2Vec2(x, y);
	}
}

