#include "Creature.h"

#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>

#include <box2d/b2_math.h>
#include <box2d/b2_world.h>
#include <box2d/box2d.h>

#include <iostream>
#include <unordered_map>

#include "BodyPart.h"
#include "BodySegment.h"
#include "Joint.h"

#include "../GameManager.h"
#include "../Globals.h"
#include "../Util.h"

#include "../NEAT/NEAT.h"

using namespace std;

Creature::Creature(string genes, b2Vec2 pos) {
	this->genes = genes;
	this->startingPos = pos;
	this->alive = true;

	vector<string> inputLabels = {
		"const",
		"sin",
	};
	for (int i = 0; i < extraInputCount; i++)
		inputLabels.push_back("in" + to_string(i));

	vector<string> outputLabels = {

	};
	for (int i = 0; i < extraInputCount; i++)
		outputLabels.push_back("out" + to_string(i));

	this->baseInputs = inputLabels.size() - extraInputCount;
	this->baseOutputs = outputLabels.size() - extraOutputCount;

	nn = make_shared<NEAT>(NEAT(inputLabels, outputLabels));

	for (int i = 0; i < 10; i++)
		nn->MutateAddConnection();
}

Creature::~Creature() {
	for (auto joint : joints)
		joint->SetShouldDeleteJoints(false);
}

void Creature::Init() {
	ApplyGenes();
}

void Creature::ApplyGenes() {
	ApplyGenes(this->genes);
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
		int index = part->GetNerveOutputIndex();
		float val = part->GetNerveOutput();

		inputs[index] += val;
	}

	// Calculations
	this->nn->Calculate(inputs);


	// Outputs
	vector<double> output = nn->GetOutputs();
	for (auto part : bodySegments) {
		int index = part->GetNerveInputIndex();
		float val = output[index];

		part->SetNerveInput(val);
	}


	// Update parts
	for (auto joint : joints) {
		joint->Update();
	}

	for (auto part : bodySegments)
		part->Update();


}

void Creature::Draw() {
	if (!bodySegments.size())
		return;

	head->Draw();

	for (auto joint : joints)
		joint->Draw();
}


void Creature::ApplyForce(b2Vec2 force) {
	this->head->GetBody()->ApplyForce(force, this->head->GetBody()->GetPosition(), true);
}


void Creature::AddJoint(shared_ptr<Joint> newJoint) {
	joints.push_back(newJoint);
}

void Creature::DestroyAllJoints() {
	for (auto joint : joints)
		joint->Destroy();
	joints.clear();
}


void Creature::AddPart(shared_ptr<BodyPart> part) {
	this->bodySegments.push_back(part);
	part->UpdateObjectUserData();
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

vector<shared_ptr<BodyPart>> Creature::GetAllParts() {
	return bodySegments;
}

shared_ptr<NEAT> Creature::GetNN() {
	return nn;
}
