#include "Creature.h"

#include <Box2D/Common/b2Math.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>

#include <Box2D/Box2D.h>

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
	this->healing = false;
	this->updateNN = 0;

	this->maxEnergy = 100;
	this->energy = maxEnergy;
	this->energyUsage = 0;
	this->eggTimer = 0;

	this->strength = 1;

	this->eggHatchTimer = 0;
	this->geneMutationCoef = 0;
	this->nnMutationCoef = 0;

	vector<string> inputLabels = GetInputLabels();
	vector<string> outputLabels = GetOutputLabels();

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
	this->updateNN = 0;

	this->eggHatchTimer = 0;
	this->eggTimer = 0;
	this->geneMutationCoef = 1;
	this->nnMutationCoef = 1;

	this->maxEnergy = 300;
	this->energy = energy;
	this->energyUsage = 0;

	this->strength = 1;

	this->baseInputs = nn->GetInputsCount() - extraInputCount;
	this->baseOutputs = nn->GetOutputsCount() - extraOutputCount;
}

Creature::~Creature() {

}

void Creature::Init() {
	ApplyGenes();

	double energyToHealth = this->energy / 2.0;
	this->energy -= energyToHealth;
	double leftover = this->DistributeHealth(energyToHealth);
	this->AddEnergy(leftover);

	CalculateStrength();
}

const std::vector<std::string>& Creature::GetInputLabels() {
	static const std::vector<std::string> labels = [] {
		std::vector<std::string> l = {
			"const",
			"sin",
			"energy",
			"health",
			"foward speed",
			"sideways speed",
			"rotational speed",
		};

		for (int i = 0; i < extraInputCount; i++)
			l.push_back("in" + std::to_string(i));

		return l;
	}();

	return labels;
}

const std::vector<std::string>& Creature::GetOutputLabels() {
	static const std::vector<std::string> labels = [] {
		std::vector<std::string> l = {
			"wants egg",
			"wants to heal",
			"rotate",
		};

		for (int i = 0; i < extraOutputCount; i++)
			l.push_back("out" + std::to_string(i));

		return l;
	}();

	return labels;
}

void Creature::ApplyGenes() {
	ApplyGenes(this->genes);
}

void Creature::SetAsPlayer(bool val) {
	this->isPlayer = val;
}

void Creature::Update() {

	updateNN++;
	if (updateNN == 3) {
		float forwardSpeed = 0;
		float sidewaysSpeed = 0;
		float angularSpeed = 0;
		if (shared_ptr<BodySegment> headPtr = head.lock()) {
			b2Body *headBody = headPtr->GetBody();
			b2Vec2 vel = headBody->GetLinearVelocity();
			float angle = headBody->GetAngle();
			b2Vec2 forward = Util::ForwardVector(angle);
			b2Vec2 right = Util::RightVector(angle);

			forwardSpeed = Util::clampForNN(b2Dot(vel, forward) / Globals::MAX_SPEED);
			sidewaysSpeed = Util::clampForNN(b2Dot(vel, right) / Globals::MAX_SPEED);
			angularSpeed = Util::clampForNN(headBody->GetAngularVelocity() / Globals::MAX_ROTATION);
		}

		// Inputs
		vector<double> inputs = {
			1.0,
			sin(al_get_time()),
			GetUsableEnergy() / 100.0,
			GetHealth() / GetTotalHealth(),
			forwardSpeed,
			sidewaysSpeed,
			angularSpeed
		};

		for (int i = 0; i < extraInputCount; i++)
			inputs.push_back(0);

		for (auto part : bodySegments) {

			for (auto& nerve : part->GetNerves()) {

				if (!nerve.inputEnabled)
					continue;

				if (nerve.inputIndex < 0)
					continue;

				float value = part->GetNerveOutput(nerve.type);

				inputs[nerve.inputIndex] += value;
			}
		}

		// Calculations
		this->nn->Calculate(inputs);
		this->updateNN = 0;
	}


	if (!isPlayer) {
		// Outputs
		vector<double> outputs = nn->GetOutputs();

		for (auto part : bodySegments) {
			for (auto& nerve : part->GetNerves()) {
				if (!nerve.outputEnabled)
					continue;

				if (nerve.outputIndex < 0)
					continue;

				float value = outputs[nerve.outputIndex];

				part->SetNerveInput(
					nerve.type,
					value
				);
			}
		}

		bool wantsEgg = output[0] || energy >= 200;
		if (wantsEgg && energy >= 50 && eggTimer <= 0) {
			MakeEgg();
		}
		eggTimer--;

		healing = output[1] > 0.5;
		if (healing) {
			double healAmount = 0.1;
			if (GetUsableEnergy() > healAmount * 2) {
				this->waste += healAmount;
				this->energy -= healAmount * 2;
				double leftover = this->DistributeHealth(healAmount, true);
				this->AddEnergy(leftover);
			}
		}

		if (shared_ptr<BodySegment> headPtr = head.lock()) {
			float rotation = outputs[2];
			float maxRotation = 2;
			rotation = std::clamp(rotation, -1.0f, 1.0f);
			headPtr->GetBody()->ApplyTorque(rotation * maxRotation, true);
		}

	}

	for (int i = bodySegments.size() - 1; i >= 0; i--) {
		auto part = bodySegments[i];
		part->Update();

		double energyUsage = part->GetEnergyUsage();
		this->energy -= energyUsage;
		this->waste += energyUsage;

		if (!part->IsAlive()) {
			this->waste += part->GetHealth();

			if (!head.expired() && part == head.lock())
				head.reset();

			part->Destroy();
			bodySegments[i] = nullptr;
			bodySegments.erase(bodySegments.begin() + i);
			CalculateStrength();
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

	if (healing) {
		b2Vec2 pos = GetHeadPosPX();

		ALLEGRO_TRANSFORM t;
		al_identity_transform(&t);

		al_translate_transform(&t, pos.x, pos.y);
		al_compose_transform(&t, &Camera::transform);

		al_use_transform(&t);
		al_draw_circle(0, 0, 200, al_map_rgb(255, 0, 0), 2);
	}
}


void Creature::ApplyForce(b2Vec2 force) {
	if (shared_ptr<BodySegment> headPtr = head.lock()) {
		headPtr->GetBody()->ApplyForce(force, headPtr->GetBody()->GetPosition(), true);
	}
}

void Creature::PrintGenes(string genes) {
	int headerSize = Globals::GENE_LENGTH * Globals::HEADER_GENES_COUNT;
	string headerGenes = genes.substr(0, headerSize);

	cout << "headerGenes --> " << headerGenes << endl;

	cout << "eggHatchTimer --> " << GetNextGene(headerGenes, 0, 3) << endl;
	cout << "geneMutationCoef --> " << GetNextGene(headerGenes, 1, 3) << endl;
	cout << "nnMutationCoef --> " << GetNextGene(headerGenes, 1, 3) << endl;

	cout << "body plan genes:" << endl;

	int instructionTypes = 5;
	int maxSize = 50;
	bool firstCreate = true;
	for (int i = headerSize; i < genes.size(); i += Globals::GENE_LENGTH) {
		string gene = genes.substr(i, Globals::GENE_LENGTH);

		cout << gene << " ====> ";

		if (gene.size() != Globals::GENE_LENGTH) {
			//cout << "what happened here??" << endl;
			continue;
		}
		int instructionType = int(GetNextGene(gene, 2, 0)) % instructionTypes;

		switch(instructionType) {
			// shapeType(1), width(3), height(3), leftover(2)
			case 0: {
				cout << " SHAPE -- ";
				cout << " TYPE:" << int(GetNextGene(gene, 1, 0)) % 2;
				cout << " WIDTH:" << Util::clamp(GetNextGene(gene, 0, 3) * maxSize, 10.0, 50.0);
				cout << " HEIGHT:" << Util::clamp(GetNextGene(gene, 0, 3) * maxSize, 10.0, 50.0);
				cout << endl;
				break;
			}
			// r(3), g(3), b(3), leftover(6)
			case 1: {
				cout << " COLOR -- ";
				cout << " R:" << GetNextGene(gene, 0, 3) * 255;
				cout << " G" << GetNextGene(gene, 0, 3) * 255;
				cout << " B" << GetNextGene(gene, 0, 3) * 255;
				cout << endl;
				break;
			}
			// parentID(1)
			case 2: {
				cout << " CREATE -- ";
				if (firstCreate) {
					cout << "HEAD";
					firstCreate = false;
				}
				else {
					int partType = GetNextGene(gene, 1, 0);
					if (partType == 9)
						cout << "CILIUM";
					else if (partType == 8)
						cout << "MOUTH";
					else if (partType == 7)
						cout << "EYE";
					else if (partType < 7)
						cout << "BODY";
				}
				cout << endl;
				break;
			}
			// angleOnParent(1), angleOffset(3)
			case 3: {
				cout << " ANGLE -- ";
				cout << " childAngleGene:" << GetNextGene(gene, 1, 0);
				cout << " angleOffset" << GetNextGene(gene, 0, 3) * 90 - 45;
				cout << endl;
				break;
			}
			// parentID(1)
			case 4: {
				cout << " PARENT -- " << int(GetNextGene(gene, 2, 0)) << endl;
				break;
			}
		}
	}

}

void Creature::PrintInfo() {
	cout << endl;
	cout << "Creature parts: " << endl;
	for (auto part : bodySegments)
		part->Print();

	cout << endl;
	cout << "Selected creature " << this << endl;
	cout << "DNA: " << endl << endl << genes << endl << endl;
	PrintGenes(genes);
	cout << "Parts: " << bodySegments.size() << endl;
	cout << "Energy: " << energy << " / " << maxEnergy << endl;
	cout << "EggHatchTimer: " << eggHatchTimer << "  nnMutationCoef: " << nnMutationCoef << endl;
	cout << "headerMutationRate: " << geneMutationRates.headerMutationRate;
	cout << "  parameterMutationRate: " << geneMutationRates.parameterMutationRate;
	cout << "  instructionMutationRate: " << geneMutationRates.instructionMutationRate;
	cout << "  duplicationRate: " << geneMutationRates.duplicationRate;
	cout << "  deletionRate: " << geneMutationRates.deletionRate;
	cout << "  rearrangementRate: " << geneMutationRates.rearrangementRate;
	cout << "  randomInsertionRate: " << geneMutationRates.randomInsertionRate << endl;
	cout << "Strength: " << strength << endl;
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

void Creature::DistributeEnergy(double amount) {

}

double Creature::DistributeHealth(double amount, bool add) {
	double leftover = 0;
	double amountPerPart = amount / double(this->bodySegments.size());
	for (auto part : this->bodySegments) {
		leftover += part->SetHealth(amountPerPart + (add ? part->GetHealth() : 0));
	}
	return leftover;
}

void Creature::MakeEgg() {
	double eggEnergy = energy / 2.0;
	double eggWaste = energy / 3.0;
	this->energy -= eggEnergy;
	this->energy -= eggWaste;
	this->waste += eggWaste;

	string eggGenes = GetMutatedGenes();
	shared_ptr<NEAT> eggNN = GetMutatedNN();
	b2Vec2 eggPos = Util::metersToPixels(this->head.lock()->GetPos());
	float eggDir = this->head.lock()->GetBody()->GetAngle() + M_PI / 2;
	eggPos += 200 * b2Vec2(cos(eggDir), sin(eggDir));

	ObjectFactory::CreateEgg(eggGenes, eggPos, eggNN, eggEnergy, this->eggHatchTimer);

	this->eggTimer = eggTimerStart;
}


void Creature::CalculateStrength() {
	double newStrengh = 0;
	for (auto part : bodySegments) {
		if (shared_ptr<BodySegment> bodySeg = dynamic_pointer_cast<BodySegment>(part)) {
			b2Vec2 size = bodySeg->GetWorldSize();
			float val = size.x * size.y * 10;
			newStrengh += val;
		}
	}
	this->strength = newStrengh;
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
		if (Util::Random() <= Globals::GENE_MUTATE_CHANCE * geneMutationCoef) {
			//cout << "mutating gene!" << endl;
			newGenes += to_string(rand() % 10);
		}
		else if (i % Globals::GENE_LENGTH == 0 && Util::Random() <= Globals::GENE_CREATE_CHANCE * geneMutationCoef) {
			//cout << "Adding new gene!!!!!!" << endl;
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
	newNN -> Mutate(this->nnMutationCoef);
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

double Creature::GetWaste() {
	return this->waste;
}

// TODO: total energy + health
double Creature::GetTotalEnergy() {
	double total = 0;

	total += this->energy;
	total += this->waste;

	for (auto part : bodySegments)
		total += part->GetHealth();

	return total;
}

b2Vec2 Creature::GetHeadPosPX() {
	if (head.expired())
		return b2Vec2(0, 0);

	return Util::metersToPixels(this->head.lock()->GetPos());
}

double Creature::GetStrength() {
	return strength;
}

weak_ptr<BodySegment> Creature::GetHead() {
	return head;
}

double Creature::GetHealth() {
	double total = 0;
	for (auto part : bodySegments)
		total += part->GetHealth();
	return total;
}
double Creature::GetTotalHealth() {
	double total = 0;
	for (auto part : bodySegments)
		total += part->GetMaxHealth();
	return total;
}

void Creature::SetBiting(bool val) {
	for (auto part : bodySegments) {
		if (shared_ptr<Mouth> mouth = dynamic_pointer_cast<Mouth>(part)) {
			mouth->SetNerveInput(NerveType::Activation, val);
		}
	}
}
