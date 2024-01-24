#pragma once

#include <Box2D/Box2D.h>
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

		double energy;


	public:
		Egg(string genes, shared_ptr<NEAT> nn, double energy, int generation, b2Vec2 pos);

		void Update();
		void Draw();

		bool ShouldHatch();
		double GetEnergy();

		string GetGenes();
		shared_ptr<NEAT> GetNN();
};

