#include "Egg.h"

#include <allegro5/color.h>
#include <box2d/b2_math.h>
#include <memory>
#include <string>
#include <iostream>

#include "Creature/Creature.h"

#include "../Util.h"

using namespace std;

Egg::Egg(string genes, shared_ptr<NEAT> nn, double energy, int generation, b2Vec2 pos) : Object(pos, b2Vec2(25, 25), Util::RandomDir(), al_map_rgb(255, 255, 255), Object::CIRCLE) {
	this->genes = genes;
	this->nn = nn;
	this->generation = generation;
	this->polymorphic_id = "Egg";
	this->energy = energy;

	this->hatchTimer = rand() % 100;
	int forceAmount = 20;
	float dir = Util::RandomDir();
	body->ApplyForceToCenter(forceAmount * b2Vec2(cos(dir), sin(dir)), true);
}

void Egg::Update() {
	if (hatchTimer > 0)
		hatchTimer--;


}

void Egg::Draw() {
	Object::Draw();
}


bool Egg::ShouldHatch() {
	return hatchTimer <= 0;
}

double Egg::GetEnergy() {
	return energy;
}

string Egg::GetGenes() {
	return genes;
}

shared_ptr<NEAT> Egg::GetNN() {
	return nn;
}
