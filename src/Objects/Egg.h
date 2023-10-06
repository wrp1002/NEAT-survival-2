#pragma once

#include <box2d/b2_math.h>
#include <string>
#include <memory>

#include "Object.h"

class NEAT;
class Creature;

using namespace std;


class Egg : public Object {
	private:
		string genes;
		shared_ptr<NEAT> nn;

		int hatchTimer;
		int generation;


	public:
		Egg(string genes, shared_ptr<NEAT> nn, double health, double energy, int generation, b2Vec2 pos);

		void Update();
		void Draw();

		bool ShouldHatch();

		string GetGenes();
		shared_ptr<NEAT> GetNN();
};

