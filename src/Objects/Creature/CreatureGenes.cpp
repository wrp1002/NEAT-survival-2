#include "BodyPart.h"
#include "Creature.h"

#include <Box2D/Box2D.h>
#include <memory>
#include <unordered_map>
#include <iostream>

#include "BodySegment.h"
#include "Joint.h"
#include "Cilium.h"
#include "Mouth.h"
#include "Eye.h"

#include "../../NEAT/NEAT.h"
#include "../../Util.h"

using namespace std;


void Creature::ApplyGenes(string genes) {
	CurrentGenes currentGenes;
	this->head.reset();;
	int symmetryID = 0;
	int selectedParentID = 0;
	unordered_map<int, vector<shared_ptr<BodyPart>>> symmetryMap;

	int instructionTypes = 5;
	int maxSize = 50;

	int headerSize = geneLength * 2;
	string headerGenes = genes.substr(0, headerSize);

	eggHatchTimer = GetNextGene(headerGenes, 0, 3) * maxEggHatchTimer + minEggHatchTimer;
	geneMutationCoef = GetNextGene(headerGenes, 1, 3);
	nnMutationCoef = GetNextGene(headerGenes, 1, 3);



	for (int i = headerSize; i < genes.size(); i += geneLength) {
		string gene = genes.substr(i, geneLength);
		if (gene.size() != geneLength) {
			//cout << "what happened here??" << endl;
			continue;
		}
		//cout << endl;
		//cout << gene << endl;
		int instructionType = int(GetNextGene(gene, 2, 0)) % instructionTypes;
		//cout << "instructionType: " << instructionType << endl;

		switch(instructionType) {
			// shapeType(1), width(3), height(3), leftover(2)
			case 0: {
				currentGenes.shapeType = int(GetNextGene(gene, 1, 0)) % 2;
				currentGenes.width = Util::clamp(GetNextGene(gene, 0, 3) * maxSize, 10.0, 50.0);
				currentGenes.height = Util::clamp(GetNextGene(gene, 0, 3) * maxSize, 10.0, 50.0);
				//cout << "shapetype:" << currentGenes.shapeType << " width: " << currentGenes.width << " height: " << currentGenes.height << endl;

				break;
			}
			// r(3), g(3), b(3), leftover(6)
			case 1: {
				currentGenes.r = GetNextGene(gene, 0, 3) * 255;
				currentGenes.g = GetNextGene(gene, 0, 3) * 255;
				currentGenes.b = GetNextGene(gene, 0, 3) * 255;
				//cout << "Set color: " << currentGenes.r << " " << currentGenes.g << " " << currentGenes.b << endl;
				break;
			}
			// parentID(1)
			case 2: {
				//cout << "Creating new object" << endl;
				if (head.expired()) {
					CreateHead(gene, currentGenes, symmetryMap, symmetryID);
					symmetryID++;
				}
				else {
					//cout << "Creating body part" << endl;
					//cout << "body segments: " << bodySegments.size() << " selectedParentID:" << selectedParentID << endl;

					vector<shared_ptr<BodyPart>> parentObjects = symmetryMap[selectedParentID];

					if (parentObjects[0]->CanAddChild() && parentObjects[1]->CanAddChild()) {
						//cout << "type3id: " << parentObjects[0]->GetType() << endl;
						int partType = GetNextGene(gene, 1, 0);

						shared_ptr<BodyPart> newPart;

						if (partType == 9)
							CreateCilium(gene, currentGenes, parentObjects, symmetryMap, symmetryID);
						else if (partType == 8)
							CreateMouth(gene, currentGenes, parentObjects, symmetryMap, symmetryID);
						else if (partType == 7)
							CreateEye(gene, currentGenes, parentObjects, symmetryMap, symmetryID);
						else if (partType < 7) {
							CreateBodySegment(gene, currentGenes, parentObjects, symmetryMap, symmetryID);
						}

						symmetryID++;
						selectedParentID = symmetryID - 1;
					}
				}
				break;
			}
			// angleOnParent(1), angleOffset(3)
			case 3: {
				currentGenes.childAngleGene = GetNextGene(gene, 1, 0);
				currentGenes.angleOffset = GetNextGene(gene, 0, 3) * 90 - 45;
				//cout << "Set childAngleGene:" << currentGenes.childAngleGene << " angleOffset:" << currentGenes.angleOffset << endl;
				break;
			}
			// parentID(1)
			case 4: {
				if (bodySegments.size() > 0) {
					selectedParentID = int(GetNextGene(gene, 2, 0)) % symmetryID;
					//cout << "Set selectedParentID:" << symmetryID << endl;
				}
				break;
			}
			case 5: {
				currentGenes.eyeFov = GetNextGene(gene, 0, 3) * 90;
				currentGenes.eyeRange = GetNextGene(gene, 0, 3) * 500;
			}
		}
	}

	if (this->head.expired()) {
		cout << "uh oh no head. time to crash" << endl;
		CreateHead("0000000000000000", currentGenes, symmetryMap, symmetryID);
		symmetryID++;
	}

}

string Creature::MutateGeneParameters(string gene) {
	int instructionType = int(GetNextGene(gene, 2, 0)) % Globals::GENE_INSTRUCTION_TYPES;
	string newGene = "";
	newGene += Util::EncodeGeneValue(instructionType, 2, 0);

	if (instructionType == 0) {
		int shapeType = int(GetNextGene(gene, 1, 0));
		float width = GetNextGene(gene, 0, 3);
		float height = GetNextGene(gene, 0, 3);

		newGene += Util::EncodeGeneValue(shapeType, 1, 0);
		newGene += Util::EncodeGeneValue(Util::Tweak(width, 0.05f), 0, 3);
		newGene += Util::EncodeGeneValue(Util::Tweak(height, 0.05f), 0, 3);
	}
	else if (instructionType == 1) {
		float r = GetNextGene(gene, 0, 3);
		float g = GetNextGene(gene, 0, 3);
		float b = GetNextGene(gene, 0, 3);

		newGene += Util::EncodeGeneValue(Util::Tweak(r, 0.05f), 0, 3);
		newGene += Util::EncodeGeneValue(Util::Tweak(g, 0.05f), 0, 3);
		newGene += Util::EncodeGeneValue(Util::Tweak(b, 0.05f), 0, 3);
	}
	// angleOnParent(1), angleOffset(3)
	else if (instructionType == 3) {
		int childAngleGene = GetNextGene(gene, 1, 0);
		float angleOffset = GetNextGene(gene, 0, 3);

		newGene += Util::EncodeGeneValue(Util::Tweak(childAngleGene, 2), 1, 0);
		newGene += Util::EncodeGeneValue(Util::Tweak(angleOffset, 0.05f), 0, 3);
	}
	// parentID(1)
	else if (instructionType == 4) {
		int selectedParentID = int(GetNextGene(gene, 2, 0));
		newGene += Util::EncodeGeneValue(Util::Tweak(selectedParentID, 2), 2, 0);
	}

	newGene += gene;
	//cout << gene << " " << gene.length() << " " << newGene << " " << newGene.length() << endl;
	assert(newGene.length() == Globals::GENE_LENGTH);
	return newGene;
}

string Creature::MutateGeneRewrite(string gene) {
	for (unsigned i = 2; i < gene.length(); i++) {
		gene[i] = '0' + (rand() % 10);
	}
	return gene;
}

string Creature::GetMutatedGenes() {
	string newGenes = "";


	float headerMutationRate = this->geneMutationRates.headerMutationRate * Globals::mutationProfile.MAX_HEADER_MUTATION_RATE;
	float parameterMutationRate = this->geneMutationRates.parameterMutationRate * Globals::mutationProfile.MAX_PARAMETER_MUTATION;
	float instructionMutationRate = this->geneMutationRates.instructionMutationRate * Globals::mutationProfile.MAX_PARAMETER_REWRITE;
	float duplicationRate = this->geneMutationRates.duplicationRate * Globals::mutationProfile.MAX_DUPLICATION;
	float deletionRate = this->geneMutationRates.deletionRate * Globals::mutationProfile.MAX_DELETION;
	float rearrangementRate = this->geneMutationRates.rearrangementRate * Globals::mutationProfile.MAX_MOVE;
	float randomInsertionRate = this->geneMutationRates.randomInsertionRate * Globals::mutationProfile.MAX_RANDOM_INSERT;

	vector<string> genesToMove;
	vector<string> genesToDuplicate;

	// Mutate header genes
	for (int i = 0; i < Globals::HEADER_GENES_COUNT * Globals::GENE_LENGTH; i++) {
		if (Util::Random() <= headerMutationRate) {
			newGenes += to_string(rand() % 10);
		}
		else
			newGenes += genes[i];
	}

	for (unsigned i = Globals::HEADER_GENES_COUNT * Globals::GENE_LENGTH; i < genes.length(); i += Globals::GENE_LENGTH) {
		string gene = genes.substr(i, Globals::GENE_LENGTH);

		if (Util::Random() <= deletionRate) {
			// skip gene
		}
		else if (Util::Random() <= parameterMutationRate) {
			newGenes += MutateGeneParameters(gene);
		}
		else if (Util::Random() <= instructionMutationRate) {
			// Rewrite gene parameters
			newGenes += MutateGeneRewrite(gene);
		}
		else if (Util::Random() <= duplicationRate) {
			// Add to list and also add it now
			genesToMove.push_back(gene);
			newGenes += gene;
		}
		else if (Util::Random() <= rearrangementRate) {
			// Add to list and move it later
			genesToMove.push_back(gene);
		}
		else if (Util::Random() <= randomInsertionRate) {
			// Insert new gene along with current one
			for (int j = 0; j < Globals::GENE_LENGTH; j++)
				newGenes += to_string(rand() % 10);
			newGenes += gene;
		}
		else
			newGenes += gene;

	}
	return newGenes;
}



void Creature::CreateHead(string gene, CurrentGenes &currentGenes, unordered_map<int, vector<shared_ptr<BodyPart>>> &symmetryMap, int &symmetryID) {
	//cout << "Creating head" << endl;
	vector<NerveInfo> headNerves;

	float angle = GetNextGene(gene, 0, 3) * 360;

	shared_ptr<BodySegment> newPart = make_shared<BodySegment>(BodySegment(
		shared_from_this(),
		b2Vec2(currentGenes.width, currentGenes.height),
		al_map_rgb(currentGenes.r, currentGenes.g, currentGenes.b),
		currentGenes.shapeType,
		startingPos,
		Util::DegreesToRadians(angle),
		headNerves
	));
	AddPart(newPart);
	this->head = newPart;

	for (int i = 0; i < 2; i++)
		symmetryMap[symmetryID].push_back(newPart);


	Joint::JointInfo jointInfo;
	jointInfo.useSpring = false;
	jointInfo.enableMotor = false;
	jointInfo.maxMotorTorque = 0;
	jointInfo.motorSpeed = 0;
	jointInfo.enableLimit = true;
	jointInfo.angleLimit = 0.01;

	vector<NerveInfo> mouthNerves = {
		NerveInfo(
			NerveType::Activation,
			int(GetNextGene(gene, 0, 2) * extraInputCount) + baseInputs,
			int(GetNextGene(gene, 0, 2) * extraOutputCount) + baseOutputs
		)
	};


	int angleOnParent = 90;
	shared_ptr<BodyPart> newMouth = make_shared<Mouth>(Mouth(
		shared_from_this(),
		dynamic_pointer_cast<BodySegment>(newPart),
		b2Vec2(currentGenes.width, currentGenes.height),
		al_map_rgb(255, 0, 0),
		Util::DegreesToRadians(angleOnParent),
		Util::DegreesToRadians(currentGenes.angleOffset),
		jointInfo,
		mouthNerves
	));

	AddPart(newMouth);
	newPart->AddChild(newMouth, angleOnParent);

	//for (int i = 0; i < 2; i++)
	symmetryMap[symmetryID].push_back((newMouth));
}

void Creature::CreateBodySegment(string gene, CurrentGenes &currentGenes, vector<shared_ptr<BodyPart>> &parentObjects, unordered_map<int, vector<shared_ptr<BodyPart>>> &symmetryMap, int &symmetryID) {
	Joint::JointInfo jointInfo;
	jointInfo.useSpring = bool(int(GetNextGene(gene, 1, 0)) % 2);
	jointInfo.enableMotor = bool(int(GetNextGene(gene, 1, 0)) % 2);
	jointInfo.maxMotorTorque = 20.0;
	jointInfo.motorSpeed = 0;
	jointInfo.enableLimit = bool(int(GetNextGene(gene, 1, 0)) % 2);
	jointInfo.angleLimit = GetNextGene(gene, 0, 3) * M_PI;

	bool inputEnabled = int(GetNextGene(gene, 1, 0)) % 2;
	bool outputEnabled = int(GetNextGene(gene, 1, 0)) % 2;

	vector<NerveInfo> nerves = {
		NerveInfo(
			NerveType::Activation,
			inputEnabled ? int(GetNextGene(gene, 1, 0)) + baseInputs : -1,
			outputEnabled ? int(GetNextGene(gene, 1, 0)) + baseOutputs : -1
		)
	};

	int angleOnParent = parentObjects[0]->GetValidChildAngle(currentGenes.childAngleGene);
	//cout << "angleOnParent1: " << angleOnParent << endl;
	shared_ptr<BodyPart> newPart = make_shared<BodySegment>(BodySegment(
		shared_from_this(),
		b2Vec2(currentGenes.width, currentGenes.height),
		al_map_rgb(currentGenes.r, currentGenes.g, currentGenes.b),
		currentGenes.shapeType,
		parentObjects[0],
		Util::DegreesToRadians(angleOnParent),
		Util::DegreesToRadians(currentGenes.angleOffset),
		jointInfo,
		nerves
	));

	AddPart(newPart);
	parentObjects[0]->AddChild(newPart, angleOnParent);
	symmetryMap[symmetryID].push_back((newPart));

	bool doSymmetry = true;

	if (doSymmetry) {
		// Don't do symmetry if angle is down
		if (angleOnParent != 270)
			angleOnParent = (180 - angleOnParent + 360) % 360;

		if (parentObjects[1]->childAngleValid(angleOnParent)) {
			vector<NerveInfo> nerves2 = {
				NerveInfo(
					NerveType::Activation,
					inputEnabled ? int(GetNextGene(gene, 1, 0)) + baseInputs : -1,
					outputEnabled ? int(GetNextGene(gene, 1, 0)) + baseOutputs : -1
				)
			};

			//cout << "angleOnParent2: " << angleOnParent << endl;
			newPart = make_shared<BodySegment>(BodySegment(
				shared_from_this(),
				b2Vec2(currentGenes.width, currentGenes.height),
				al_map_rgb(currentGenes.r, currentGenes.g, currentGenes.b),
				currentGenes.shapeType,
				parentObjects[1],
				Util::DegreesToRadians(angleOnParent),
				Util::DegreesToRadians(-currentGenes.angleOffset),
				jointInfo,
				nerves2
			));
			AddPart(newPart);
			parentObjects[1]->AddChild(newPart, angleOnParent);
		}
	}

	symmetryMap[symmetryID].push_back((newPart));
}

void Creature::CreateMouth(string gene, CurrentGenes &currentGenes, vector<shared_ptr<BodyPart>> &parentObjects, unordered_map<int, vector<shared_ptr<BodyPart>>> &symmetryMap, int &symmetryID) {
	Joint::JointInfo jointInfo;
	jointInfo.useSpring = false;
	jointInfo.enableMotor = false;
	jointInfo.maxMotorTorque = 0;
	jointInfo.motorSpeed = 0;
	jointInfo.enableLimit = true;
	jointInfo.angleLimit = 0.01;

	vector<NerveInfo> nerves = {
		NerveInfo(
			NerveType::Activation,
			-1, //int(GetNextGene(gene, 1, 0)) % 2 ? int(GetNextGene(gene, 0, 2) * extraOutputCount) + baseOutputs : -1
			int(GetNextGene(gene, 1, 0)) % 2 ? int(GetNextGene(gene, 0, 2) * extraOutputCount) + baseOutputs : -1
		)
	};

	int angleOnParent = parentObjects[0]->GetValidChildAngle(currentGenes.childAngleGene);
	shared_ptr<BodyPart> newPart = make_shared<Mouth>(Mouth(
		shared_from_this(),
		dynamic_pointer_cast<BodySegment>(parentObjects[0]),
		b2Vec2(currentGenes.width, currentGenes.height),
		al_map_rgb(255, 0, 0),
		Util::DegreesToRadians(angleOnParent),
		Util::DegreesToRadians(currentGenes.angleOffset),
		jointInfo,
		nerves
	));

	AddPart(newPart);
	parentObjects[0]->AddChild(newPart, angleOnParent);
	symmetryMap[symmetryID].push_back((newPart));

	// Don't do symmetry if angle is down
	if (angleOnParent != 270)
		angleOnParent = (180 - angleOnParent + 360) % 360;

	if (parentObjects[1]->childAngleValid(angleOnParent)) {
		vector<NerveInfo> nerves2 = {
			NerveInfo(
				NerveType::Activation,
				-1, //int(GetNextGene(gene, 1, 0)) % 2 ? int(GetNextGene(gene, 0, 2) * extraOutputCount) + baseOutputs : -1
				int(GetNextGene(gene, 1, 0)) % 2 ? int(GetNextGene(gene, 0, 2) * extraOutputCount) + baseOutputs : -1
			)
		};

		newPart = make_shared<Mouth>(Mouth(
			shared_from_this(),
			dynamic_pointer_cast<BodySegment>(parentObjects[1]),
			b2Vec2(currentGenes.width, currentGenes.height),
			al_map_rgb(255, 0, 0),
			Util::DegreesToRadians(angleOnParent),
			Util::DegreesToRadians(-currentGenes.angleOffset),
			jointInfo,
			nerves2
		));

		AddPart(newPart);
		parentObjects[1]->AddChild(newPart, angleOnParent);
	}

	symmetryMap[symmetryID].push_back((newPart));
}

void Creature::CreateCilium(string gene, CurrentGenes &currentGenes, vector<shared_ptr<BodyPart>> &parentObjects, unordered_map<int, vector<shared_ptr<BodyPart>>> &symmetryMap, int &symmetryID) {
	Joint::JointInfo jointInfo;
	jointInfo.useSpring = false;
	jointInfo.enableMotor = false;
	jointInfo.maxMotorTorque = 0;
	jointInfo.motorSpeed = 0;
	jointInfo.enableLimit = true;
	jointInfo.angleLimit = 0.1;

	vector<NerveInfo> nerves = {
		NerveInfo(
			NerveType::Activation,
			-1,
			int(GetNextGene(gene, 0, 2) * extraOutputCount) + baseOutputs
		)
	};

	int angleOnParent = parentObjects[0]->GetValidChildAngle(currentGenes.childAngleGene);
        shared_ptr<BodyPart> newPart = make_shared<Cilium>(
            Cilium(shared_from_this(),
                   dynamic_pointer_cast<BodySegment>(parentObjects[0]),
                   b2Vec2(currentGenes.width, currentGenes.height),
                   al_map_rgb(255, 0, 0), Util::DegreesToRadians(angleOnParent),
                   Util::DegreesToRadians(currentGenes.angleOffset), jointInfo,
                   nerves
			)
		);

        AddPart(newPart);
	parentObjects[0]->AddChild(newPart, angleOnParent);
	symmetryMap[symmetryID].push_back((newPart));

	// Don't do symmetry if angle is down
	if (angleOnParent != 270)
		angleOnParent = (180 - angleOnParent + 360) % 360;

	if (parentObjects[1]->childAngleValid(angleOnParent)) {
		vector<NerveInfo> nerves2 = {
		NerveInfo(
				NerveType::Activation,
				-1,
				int(GetNextGene(gene, 0, 2) * extraOutputCount) + baseOutputs
			)
		};

		newPart = make_shared<Cilium>(
			Cilium(shared_from_this(),
					dynamic_pointer_cast<BodySegment>(parentObjects[1]),
					b2Vec2(currentGenes.width, currentGenes.height),
					al_map_rgb(255, 0, 0),
					Util::DegreesToRadians(angleOnParent),
					Util::DegreesToRadians(-currentGenes.angleOffset),
					jointInfo, nerves2
			)
		);

		AddPart(newPart);
		parentObjects[1]->AddChild(newPart, angleOnParent);
	}

	symmetryMap[symmetryID].push_back((newPart));
}


void Creature::CreateEye(string gene, CurrentGenes &currentGenes, vector<shared_ptr<BodyPart>> &parentObjects, unordered_map<int, vector<shared_ptr<BodyPart>>> &symmetryMap, int &symmetryID) {
	Joint::JointInfo jointInfo;
	jointInfo.useSpring = false;
	jointInfo.enableMotor = false;
	jointInfo.maxMotorTorque = 0;
	jointInfo.motorSpeed = 0;
	jointInfo.enableLimit = true;
	jointInfo.angleLimit = 0.01;

	vector<NerveInfo> nerves = {
		NerveInfo(
			NerveType::Activation,
			int(GetNextGene(gene, 1, 0)) + baseInputs
		),

		NerveInfo(
			NerveType::Direction,
			int(GetNextGene(gene, 1, 0)) + baseInputs
		)
	};

	/*
	float range = 200;
	float fov = 30;
	int rayCount = 5;
	float targetHue = 0;
	*/

	Eye::EyeInfo eyeInfo;
	eyeInfo.fov = currentGenes.eyeFov;
	eyeInfo.range = currentGenes.eyeRange;
	eyeInfo.rayCount = 5;
	eyeInfo.targetHue = 0;

	//cout << "eye " << nerveInfo.inputIndex << " " << nerveInfo.outputIndex << endl;

	int angleOnParent = parentObjects[0]->GetValidChildAngle(currentGenes.childAngleGene);
	shared_ptr<BodyPart> newPart = make_shared<Eye>(Eye(
		shared_from_this(),
		dynamic_pointer_cast<BodySegment>(parentObjects[0]),
		b2Vec2(currentGenes.width, currentGenes.height),
		al_map_rgb(currentGenes.r, currentGenes.g, currentGenes.b),
		Util::DegreesToRadians(angleOnParent),
		Util::DegreesToRadians(currentGenes.angleOffset),
		jointInfo,
		nerves,
		eyeInfo
	));

	AddPart(newPart);
	parentObjects[0]->AddChild(newPart, angleOnParent);
	symmetryMap[symmetryID].push_back((newPart));

	// Don't do symmetry if angle is down
	if (angleOnParent != 270)
		angleOnParent = (180 - angleOnParent + 360) % 360;

	if (parentObjects[1]->childAngleValid(angleOnParent)) {
		vector<NerveInfo> nerves2 = {
			NerveInfo(
				NerveType::Activation,
				int(GetNextGene(gene, 1, 0)) + baseInputs
			),
			NerveInfo(
				NerveType::Direction,
				int(GetNextGene(gene, 1, 0)) + baseInputs
			)
		};
		//cout << "eye " << nerveInfo.inputIndex << " " << nerveInfo.outputIndex << endl;

		newPart = make_shared<Eye>(Eye(
			shared_from_this(),
			dynamic_pointer_cast<BodySegment>(parentObjects[1]),
			b2Vec2(currentGenes.width, currentGenes.height),
			al_map_rgb(currentGenes.r, currentGenes.g, currentGenes.b),
			Util::DegreesToRadians(angleOnParent),
			Util::DegreesToRadians(-currentGenes.angleOffset),
			jointInfo,
			nerves2,
			eyeInfo
		));

		AddPart(newPart);
		parentObjects[1]->AddChild(newPart, angleOnParent);
	}

	symmetryMap[symmetryID].push_back((newPart));
}
