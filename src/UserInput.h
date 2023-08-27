#pragma once
#include <unordered_map>

#include <box2d/b2_math.h>

using namespace std;

namespace UserInput {
	extern unordered_map<int, bool> keyStates;
	extern b2Vec2 dragStartPos;
	extern b2Vec2 mousePos;
	extern bool isDragging;
	extern int mouseWheel;

	void Init();

	void StartDragging(float x, float y);
	void StartDragging(b2Vec2 pos);
	void StopDragging();

	void SetPressed(int key, bool pressed);
	bool IsPressed(int key);
	void SetMousePos(b2Vec2 pos);
	void SetMousePos(float x, float y);
};

