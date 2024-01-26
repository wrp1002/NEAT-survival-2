#pragma once

#include <allegro5/color.h>

#include <Box2D/Box2D.h>

#include <memory>
#include <string>

#include "Object.h"

using namespace std;

class LiveObject : public Object {
	protected:
		double health, maxHealth;

	public:
		enum SHAPE_TYPES {RECT, CIRCLE};

		LiveObject();
		LiveObject(b2Vec2 pos, b2Vec2 pixelSize, float angle, ALLEGRO_COLOR color, int shapeType, double health);

		virtual void Update();

		double TakeDamage(double amount);

		double GetHealth();
};

