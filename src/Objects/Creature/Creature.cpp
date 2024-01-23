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

	this->maxEnergy = 100;
	this->energy = maxEnergy;
	this->energyUsage = 0;

	this->eggHatchTimer = 0;
	this->geneMutationCoef = 0;
	this->nnMutationCoef = 0;

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

Creature::Creature(string genes, b2Vec2 pos, shared_ptr<NEAT> nn, double energy) : Creature(genes, pos) {
	this->genes = genes;
	this->startingPos = pos;
	this->alive = true;
	this->isPlayer = false;
	this->nn = nn;

	this->eggHatchTimer = 0;
	this->geneMutationCoef = 0;
	this->nnMutationCoef = 0;

	this->maxEnergy = 300;
	this->energy = energy;
	this->energyUsage = 0;

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
		if (!part->NerveOutputEnabled())
			continue;

		int index = part->GetNerveInputIndex();
		float val = part->GetNerveOutput();
		inputs[index] += val;
	}

	// Calculations
	this->nn->Calculate(inputs);


	if (!isPlayer) {
		// Outputs
		vector<double> output = nn->GetOutputs();

		for (auto part : bodySegments) {
			if (!part->NerveInputEnabled())
				continue;

			int index = part->GetNerveOutputIndex();
			float val = output[index];
			part->SetNerveInput(val);
		}

		bool wantsEgg = output[0] || energy >= 200;
		if (wantsEgg && energy >= 50) {
			MakeEgg();
		}
	}

	for (int i = bodySegments.size() - 1; i >= 0; i--) {
		auto part = bodySegments[i];
		part->Update();

		double energyUsage = part->GetEnergyUsage();
		this->energy -= energyUsage;

		if (!part->IsAlive()) {
			if (!head.expired() && part == head.lock())
				head.reset();

			part->Destroy();
			bodySegments[i] = nullptr;
			bodySegments.erase(bodySegments.begin() + i);
		}
	}


	if (head.expired())
		alive = false;

	if (energy <= 0)
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

void Creature::PrintInfo() {
	cout << "Selected creature " << this << endl;
	cout << "DNA: " << endl << endl << genes << endl << endl;
	cout << "Parts: " << bodySegments.size() << endl;
	cout << "Energy: " << energy << " / " << maxEnergy << endl;
	cout << "EggHatchTimer: " << eggHatchTimer << "  geneMutationCoef: " << geneMutationCoef << "  nnMutationCoef: " << nnMutationCoef << endl;

	for (auto part : bodySegments)
		part->Print();
}

void Creature::DestroyAllJoints() {
	//cout << "Destroy all joints!" << endl;
	for (auto part : bodySegments)
		part->DestroyJoint();
}


void Creature::AddPart(shared_ptr<BodyPart> part) {
	this->bodySegments.push_back(part);
	part->UpdateObjectUserData();
}

void Creature::AddEnergy(double amount) {
	this->energy += amount;
}

void Creature::TakeDamage(double amount) {

}

void Creature::MakeEgg() {
	double eggEnergy = energy / 2.0;
	this->energy -= eggEnergy;

	string eggGenes = GetMutatedGenes();
	shared_ptr<NEAT> eggNN = GetMutatedNN();
	b2Vec2 eggPos = Util::metersToPixels(this->head.lock()->GetPos());
	float eggDir = this->head.lock()->GetBody()->GetAngle() + M_PI / 2;
	eggPos += 200 * b2Vec2(cos(eggDir), sin(eggDir));

	ObjectFactory::CreateEgg(eggGenes, eggPos, eggNN, eggEnergy);
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
		else if (i % Globals::GENE_LENGTH == 0 && Util::Random() <= Globals::GENE_CREATE_CHANCE * geneMutationCoef) {
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

double Creature::GetUsableEnergy() {
	return this->energy;
}

// TODO: total energy + health
double Creature::GetTotalEnergy() {
	double total = 0;
	for (auto part : bodySegments) {
		total += 0;
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
