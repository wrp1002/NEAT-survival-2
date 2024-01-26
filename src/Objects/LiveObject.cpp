#include "LiveObject.h"

LiveObject::LiveObject() : Object() {
	polymorphic_id = "LiveObject";
	health = 0;
	maxHealth = 0;
}

LiveObject::LiveObject(b2Vec2 pos, b2Vec2 pixelSize, float angle, ALLEGRO_COLOR color, int shapeType, double health) : Object(pos, pixelSize, angle,color, shapeType) {
	polymorphic_id = "LiveObject";
	this->health = health;
}

void LiveObject::Update() {
	if (health <= 0)
		alive = false;
}

double LiveObject::TakeDamage(double amount) {
	amount = min(amount, health);
	this->health -= amount;

	return amount;
}

double LiveObject::GetHealth() {
	return health;
}
