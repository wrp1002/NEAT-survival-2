#include "Egg.h"

#include <allegro5/color.h>
#include <Box2D/Box2D.h>
#include <memory>
#include <string>
#include <iostream>

#include "Creature/Creature.h"

#include "../Util.h"
#include "LiveObject.h"

using namespace std;

Egg::Egg(string genes, shared_ptr<NEAT> nn, double energy, int generation, b2Vec2 pos, int hatchTimer) : LiveObject(pos, b2Vec2(25, 25), Util::RandomDir(), al_map_rgb(255, 255, 255), Object::CIRCLE, energy / 2) {
	this->genes = genes;
	this->nn = nn;
	this->generation = generation;
	this->polymorphic_id = "Egg";
	this->energy = energy / 2;

	this->hatchTimer = hatchTimer;
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


void Egg::AddEnergy(double amount) {
	this->energy += amount;
}

double Egg::TakeDamage(double amount) {
	cout << "Egg take damage" << endl;
	double lostHealth = LiveObject::TakeDamage(amount);
	if (health <= 0) {
		cout << "egg died" << endl;
		cout << "energy returned before: " << lostHealth << endl;
		lostHealth += energy;
		energy = 0;
		Kill();
		cout << "energy returned after: " << lostHealth << endl;
	}

	return lostHealth;
}


bool Egg::ShouldHatch() {
	return hatchTimer <= 0;
}

double Egg::GetEnergy() {
	return energy;
}

int Egg::GetHatchTimer() {
	return hatchTimer;
}

string Egg::GetGenes() {
	return genes;
}

shared_ptr<NEAT> Egg::GetNN() {
	return nn;
}
