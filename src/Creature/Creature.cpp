#include "Creature.h"

#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>

#include <box2d/b2_math.h>
#include <box2d/b2_world.h>
#include <box2d/box2d.h>

#include <iostream>
#include <unordered_map>

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

	for (auto child : bodySegments)
		GameManager::world.DestroyBody(child->GetBody());
}

void Creature::Init() {
	ApplyGenes();
}

void Creature::ApplyGenes() {
	ApplyGenes(this->genes);
}

void Creature::Update() {
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

	this->nn->Calculate(inputs);

	for (auto joint : joints) {
		joint->Update();
	}

	for (auto part : bodySegments)
		part->Update();

	vector<double> output = nn->GetOutputs();

	for (auto part : bodySegments) {
		int index = part->GetNerveInputIndex();
		float val = output[index];

		part->SetNerveInput(val);
	}

}

void Creature::Draw() {
	if (!bodySegments.size())
		return;

	head->Draw();

	for (auto joint : joints)
		joint->Draw();

	/*
	for (auto joint : joints) {
		b2Vec2 pos;
		pos = Util::metersToPixels(joint->GetAnchorA());
		al_draw_filled_circle(pos.x, pos.y, 2, al_map_rgb(0, 255, 0));

		pos = Util::metersToPixels(joint->GetAnchorB());
		al_draw_filled_circle(pos.x, pos.y, 2, al_map_rgb(0, 255, 0));
	}
	*/
}


void Creature::AddJoint(shared_ptr<Joint> newJoint) {
	joints.push_back(newJoint);
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