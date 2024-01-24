#pragma once
#include <allegro5/allegro.h>

#include <Box2D/Box2D.h>
#include <memory>

class Object;

namespace Camera {
	extern ALLEGRO_TRANSFORM transform;
	extern b2Vec2 pos;
	extern float zoom;
	extern float zoomFactor;
	extern float minZoom;
	//extern weak_ptr<Object> followObject;

	void Init();

	void UpdateTransform();
	void UpdateZoom(int diff);
	void ZoomIn();
	void ZoomOut();
	//void FollowObject(weak_ptr<Object> obj);

	b2Vec2 CalculatePos();
	b2Vec2 ScreenPos2WorldPos(b2Vec2 screenPos);
};

