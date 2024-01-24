#pragma once

#include <allegro5/color.h>

#include <Box2D/Box2D.h>

#include <memory>
#include <string>

#include "UserData/ObjectUserData.h"

using namespace std;

class Object : public enable_shared_from_this<Object> {
	protected:
		b2Body *body;
		b2Vec2 pixelSize;
		b2Vec2 worldSize;
		ALLEGRO_COLOR color;
		int shapeType;
		string polymorphic_id;
		shared_ptr<ObjectUserData> objectUserData;
		bool alive;

	public:
		enum SHAPE_TYPES {RECT, CIRCLE};

		Object();
		Object(b2Vec2 pos, b2Vec2 pixelSize, float angle, ALLEGRO_COLOR color, int shapeType);
		void UpdateObjectUserData();

		virtual void Update();
		virtual void Draw();
		virtual void Destroy();
		virtual void Print();

		void ApplyForce(b2Vec2 force);
		void Kill();

		b2Vec2 GetEdgePoint(float angle);
		b2Vec2 GetPos();
		string GetType();

		bool IsAlive();
};

