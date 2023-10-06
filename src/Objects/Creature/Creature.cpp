#include "Creature.h"

#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>

#include <box2d/b2_math.h>
#include <box2d/b2_world.h>
#include <box2d/box2d.h>

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

#include "BodyPart.h"
#include "BodySegment.h"
#include "Joint.h"
#include "Mouth.h"

#include "../../ObjectFactory.h"
#include "../../Globals.h"
#include "../../Util.h"

#include "../../NEAT/NEAT.h"

using namespace std;

Creature::Creature(string genes, b2Vec2 pos) {
	this->genes = genes;
	this->startingPos = pos;
	this->alive = true;
	this->isPlayer = false;

	this->maxHealth = 5;
	this->health = maxHealth;
	this->maxEnergy = 10;
	this->energy = maxEnergy;

	vector<string> inputLabels = {
		"const",
		"sin",
	};
	for (int i = 0; i < extraInputCount; i++)
		inputLabels.push_back("in" + to_string(i));

	vector<string> outputLabels = {

	};
	for (int i = 0; i < extraOutputCount; i++)
		outputLabels.push_back("out" + to_string(i));

	this->baseInputs = inputLabels.size() - extraInputCount;
	this->baseOutputs = outputLabels.size() - extraOutputCount;

	nn = make_shared<NEAT>(NEAT(inputLabels, outputLabels));

	for (int i = 0; i < 30; i++)
		nn->MutateAddConnection();
}

Creature::Creature(string genes, b2Vec2 pos, shared_ptr<NEAT> nn) : Creature(genes, pos) {
	this->genes = genes;
	this->startingPos = pos;
	this->alive = true;
	this->isPlayer = false;
	this->nn = nn;

	this->maxHealth = 5;
	this->health = maxHealth;
	this->maxEnergy = 10;
	this->energy = maxEnergy;

	this->baseInputs = nn->GetInputsCount() - extraInputCount;
	this->baseOutputs = nn->GetOutputsCount() - extraOutputCount;
}

Creature::~Creature() {

}

void Creature::Init() {
	ApplyGenes();
}

void Creature::ApplyGenes() {
	ApplyGenes(this->genes);
}

void Creature::SetAsPlayer(bool val) {
	this->isPlayer = val;
}

void Creature::Update() {
	// Inputs
	vector<double> inputs = {
		1.0,
		sin(al_get_time())
	};

	for (int i = 0; i < extraInputCount; i++)
		inputs.push_back(0);

	for (auto part : bodySegments) {
		int index = part->GetNerveInputIndex();
		float val = part->GetNerveOutput();
		//cout << "input " << val << " at " << index << endl;

		inputs[index] += val;
	}

	// Calculations
	this->nn->Calculate(inputs);


	if (!isPlayer) {
		// Outputs
		vector<double> output = nn->GetOutputs();
		for (auto part : bodySegments) {
			int index = part->GetNerveOutputIndex();
			float val = output[index];

			part->SetNerveInput(val);
		}
	}

	for (int i = bodySegments.size() - 1; i >= 0; i--) {
		auto part = bodySegments[i];
		part->Update();

		if (!part->IsAlive()) {
			if (!head.expired() && part == head.lock())
				head.reset();

			part->Destroy();
			bodySegments[i] = nullptr;
			bodySegments.erase(bodySegments.begin() + i);
		}
	}


	if (health <= 0 || head.expired())
		alive = false;

}

void Creature::Draw() {
	if (!bodySegments.size() || head.expired())
		return;


	for (auto part : bodySegments)
		part->Draw();

}


void Creature::ApplyForce(b2Vec2 force) {
	if (shared_ptr<BodySegment> headPtr = head.lock()) {
		headPtr->GetBody()->ApplyForce(force, headPtr->GetBody()->GetPosition(), true);
	}
}

void Creature::DestroyAllJoints() {
	cout << "Destroy all joints!" << endl;
	for (auto part : bodySegments)
		part->DestroyJoint();
}


void Creature::AddPart(shared_ptr<BodyPart> part) {
	this->bodySegments.push_back(part);
	part->UpdateObjectUserData();
}

void Creature::TakeDamage(double amount) {
	this->health -= amount;
}

void Creature::MakeEgg() {
	string eggGenes = GetMutatedGenes();
	shared_ptr<NEAT> eggNN = GetMutatedNN();
	ObjectFactory::CreateEgg(eggGenes, Util::metersToPixels(this->head.lock()->GetPos()), eggNN);
}


bool Creature::IsAlive() {
	return alive;
}

float Creature::decimalFromSubstring(string str, int wholeDigits, int decimalDigits) {
	assert(str.size() >= wholeDigits + decimalDigits);
	str = str.substr(0, wholeDigits + decimalDigits);
	str.insert(wholeDigits, ".");
	return stof(str);
}

float Creature::GetNextGene(string &gene, int wholeDigits, int decimalDigits) {
	float val = decimalFromSubstring(gene, wholeDigits, decimalDigits);
	int delta = wholeDigits + decimalDigits;
	gene = gene.substr(delta, gene.size() - delta);
	return val;
}

string Creature::GetMutatedGenes() {
	string newGenes = "";
	for (int i = 0; i < genes.size(); i++) {
		if (Util::Random() <= Globals::GENE_MUTATE_CHANCE)
			newGenes += to_string(rand() % 10);
		else if (i % Globals::GENE_LENGTH == 0 && Util::Random() <= Globals::GENE_CREATE_CHANCE) {
			cout << "Adding new gene!!!!!!" << endl;
			for (int j = 0; j < Globals::GENE_LENGTH; j++)
				newGenes += to_string(rand() % 10);
		}
		else
			newGenes += genes[i];
	}
	return newGenes;
}

shared_ptr<NEAT> Creature::GetMutatedNN() {
	shared_ptr<NEAT> newNN = nn->Copy();
	newNN -> Mutate();
	return newNN;
}

vector<shared_ptr<BodyPart>> Creature::GetAllParts() {
	return bodySegments;
}

shared_ptr<NEAT> Creature::GetNN() {
	return nn;
}

double Creature::GetTotalEnergy() {
	double total = 0;
	for (auto part : bodySegments) {
		total += part->GetEnergy();
	}
	return total;
}


void Creature::SetBiting(bool val) {
	for (auto part : bodySegments) {
		if (shared_ptr<Mouth> mouth = dynamic_pointer_cast<Mouth>(part)) {
			mouth->SetNerveInput(val);
		}
	}
}
