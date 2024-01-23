#pragma once

#include <box2d/b2_math.h>
#include <memory>
#include <vector>

class Egg;
class Creature;
class NEAT;

using namespace std;

namespace ObjectFactory {
	static const int extraInputCount = 10;
	static const int extraOutputCount = 10;
	static const int geneLength = 16;
	extern vector<string> NNInputLabels;
	extern vector<string> NNOutputLabels;


	void Init();

	string GetRandomGenes(int length);

	shared_ptr<Egg> CreateEgg();
	shared_ptr<Egg> CreateEgg(string genes, b2Vec2 pos, shared_ptr<NEAT> nn, double energy);

	shared_ptr<Creature> CreateAgent();
	shared_ptr<Creature> CreateAgent(string genes, b2Vec2 pos);
	shared_ptr<Creature> CreateAgent(string genes, b2Vec2 pos, shared_ptr<NEAT> nn, double energy);

	shared_ptr<NEAT> RandomNN();

}
