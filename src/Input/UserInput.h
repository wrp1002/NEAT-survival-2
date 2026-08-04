#pragma once
#include <Box2D/Box2D.h>
#include <unordered_map>
#include <memory>

#include "../Objects/Object.h"

using namespace std;

namespace UserInput {
	extern unordered_map<int, bool> keyStates;
	extern b2Vec2 dragStartPos;
	extern b2Vec2 mousePos;
	extern bool isDragging;
	extern int mouseWheel;
	extern b2MouseJoint* mouseJoint;

	void Init();
	void Update();

	void StartDragging(float x, float y);
	void StartDragging(b2Vec2 pos);
	void StopDragging();
	void StartObjectDragging(shared_ptr<Object> object);
	void StopObjectDragging();

	void SetPressed(int key, bool pressed);
	bool IsPressed(int key);
	void SetMousePos(b2Vec2 pos);
	void SetMousePos(float x, float y);
	void DrawMouseJoint();

	shared_ptr<Object> GetObjectAtMouse();
};

