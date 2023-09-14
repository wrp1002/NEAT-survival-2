#pragma once

#include <allegro5/color.h>

#include <box2d/b2_body.h>
#include <box2d/b2_circle_shape.h>
#include <box2d/b2_common.h>
#include <box2d/b2_math.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_shape.h>
#include <box2d/b2_world.h>
#include <box2d/box2d.h>

#include <memory>
#include <string>

#include "ObjectUserData.h"

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

	public:
		enum SHAPE_TYPES {RECT, CIRCLE};

		Object();
		Object(b2Vec2 pos, b2Vec2 pixelSize, float angle, ALLEGRO_COLOR color, int shapeType);
		void UpdateObjectUserData();

		virtual void Update();
		virtual void Draw();

		void ApplyForce(b2Vec2 force);

		b2Vec2 GetEdgePoint(float angle);
		b2Vec2 GetPos();
		string GetType();
};

