#include "ObjectFactory.h"

#include <iostream>
#include <memory>
#include <vector>

#include "Globals.h"
#include "GameManager.h"
#include "Util.h"
#include "NEAT/NEAT.h"

#include "Objects/Egg.h"

using namespace std;

namespace  ObjectFactory {
	vector<string> inputLabels;
	vector<string> outputLabels;

	void Init() {
		inputLabels = {
			"const",
			"sin",
		};
		for (int i = 0; i < extraInputCount; i++)
			inputLabels.push_back("in" + to_string(i));

		outputLabels = {
			"lay_egg",
		};
		for (int i = 0; i < extraOutputCount; i++)
			outputLabels.push_back("out" + to_string(i));
	}


	string GetRandomGenes(int length) {
		string genes = "";
		for (int i = 0; i < length; i++) {
			string gene = "";
			for (int j = 0; j < 16; j++) {
				gene += to_string(rand() % 10);
			}
			//cout << "adding gene:" << gene << endl;
			genes += gene;
		}
		return genes;
	}


	shared_ptr<Egg> CreateEgg() {
		string genes = GetRandomGenes(50);
		//cout << genes << endl;

		float angle = Util::RandomDir();
		int dist = Globals::WORLD_SIZE_PX * sqrt(Util::Random());
		double energy = 200;

		return CreateEgg(genes, b2Vec2(cos(angle) * dist, sin(angle) * dist), NEAT::RandomNN(inputLabels, outputLabels, 10), energy);
	}

	shared_ptr<Egg> CreateEgg(string genes, b2Vec2 pos, shared_ptr<NEAT> nn, double energy) {
		shared_ptr<Egg> newEgg = make_shared<Egg>(Egg(genes, nn, energy, 0, pos));
		GameManager::eggs.push_back(newEgg);
		return newEgg;
	}

	shared_ptr<Creature> CreateAgent() {
		string genes = GetRandomGenes(50);
		cout << genes << endl;

		float angle = Util::RandomDir();
		int dist = Globals::WORLD_SIZE_PX * sqrt(Util::Random());
		return CreateAgent(genes, b2Vec2(cos(angle) * dist, sin(angle) * dist));
	}

	shared_ptr<Creature> CreateAgent(string genes, b2Vec2 pos) {
		shared_ptr<Creature> creature = make_shared<Creature>(Creature(genes, pos));
		creature->Init();
		GameManager::agents.push_back(creature);
		cout << GameManager::agents.size() << endl;
		return creature;
	}

	shared_ptr<Creature> CreateAgent(string genes, b2Vec2 pos, shared_ptr<NEAT> nn, double energy) {
		shared_ptr<Creature> creature = make_shared<Creature>(Creature(genes, pos, nn, energy));
		creature->Init();
		GameManager::agents.push_back(creature);
		cout << GameManager::agents.size() << endl;
		return creature;
	}

	shared_ptr<NEAT> RandomNN() {
		return NEAT::RandomNN(inputLabels, outputLabels, 50);
	}

}