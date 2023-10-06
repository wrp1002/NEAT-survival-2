#include "BodyPart.h"
#include "Creature.h"

#include <box2d/b2_body.h>
#include <box2d/b2_math.h>
#include <memory>
#include <unordered_map>
#include <iostream>

#include "BodySegment.h"
#include "Joint.h"
#include "Cilium.h"
#include "Mouth.h"
#include "Eye.h"

#include "../Util.h"

using namespace std;


void Creature::ApplyGenes(string genes) {
	CurrentGenes currentGenes;
	this->head.reset();;
	int symmetryID = 0;
	int selectedParentID = 0;
	unordered_map<int, vector<shared_ptr<BodyPart>>> symmetryMap;

	int instructionTypes = 5;
	int maxSize = 50;

	for (int i = 0; i < genes.size(); i += geneLength) {
		string gene = genes.substr(i, geneLength);
		cout << endl;
		cout << gene << endl;
		int instructionType = int(GetNextGene(gene, 2, 0)) % instructionTypes;
		cout << "instructionType: " << instructionType << endl;

		switch(instructionType) {
			// shapeType(1), width(3), height(3), leftover(2)
			case 0: {
				currentGenes.shapeType = int(GetNextGene(gene, 1, 0)) % 2;
				currentGenes.width = Util::clamp(GetNextGene(gene, 0, 3) * maxSize, 10.0, 50.0);
				currentGenes.height = Util::clamp(GetNextGene(gene, 0, 3) * maxSize, 10.0, 50.0);
				cout << "shapetype:" << currentGenes.shapeType << " width: " << currentGenes.width << " height: " << currentGenes.height << endl;

				break;
			}
			// r(3), g(3), b(3), leftover(6)
			case 1: {
				currentGenes.r = GetNextGene(gene, 0, 3) * 255;
				currentGenes.g = GetNextGene(gene, 0, 3) * 255;
				currentGenes.b = GetNextGene(gene, 0, 3) * 255;
				cout << "Set color: " << currentGenes.r << " " << currentGenes.g << " " << currentGenes.b << endl;
				break;
			}
			// parentID(1)
			case 2: {
				cout << "Creating new object" << endl;
				if (head.expired()) {
					CreateHead(gene, currentGenes, symmetryMap, symmetryID);
					symmetryID++;
				}
				else {
					cout << "Creating BodySegment" << endl;
					cout << "body segments: " << bodySegments.size() << " selectedParentID:" << selectedParentID << endl;

					vector<shared_ptr<BodyPart>> parentObjects = symmetryMap[selectedParentID];

					if (parentObjects[0]->CanAddChild() && parentObjects[1]->CanAddChild()) {
						cout << "type3id: " << parentObjects[0]->GetType() << endl;
						int partType = GetNextGene(gene, 1, 0);

						shared_ptr<BodyPart> newPart;

						if (partType == 9)
							CreateCilium(gene, currentGenes, parentObjects, symmetryMap, symmetryID);
						else if (partType == 8)
							CreateMouth(gene, currentGenes, parentObjects, symmetryMap, symmetryID);
						else if (partType == 7)
							CreateEye(gene, currentGenes, parentObjects, symmetryMap, symmetryID);
						else if (partType < 7)
							CreateBodySegment(gene, currentGenes, parentObjects, symmetryMap, symmetryID);

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
				cout << "Set childAngleGene:" << currentGenes.childAngleGene << " angleOffset:" << currentGenes.angleOffset << endl;
				break;
			}
			// parentID(1)
			case 4: {
				if (bodySegments.size() > 0) {
					selectedParentID = int(GetNextGene(gene, 2, 0)) % symmetryID;
					cout << "Set selectedParentID:" << symmetryID << endl;
				}
				break;
			}
		}
	}

	if (this->head.expired()) {
		cout << "uh oh no head. time to crash" << endl;
		CreateHead("0000000000000000", currentGenes, symmetryMap, symmetryID);
		symmetryID++;
	}

}



void Creature::CreateHead(string gene, CurrentGenes &currentGenes, unordered_map<int, vector<shared_ptr<BodyPart>>> &symmetryMap, int &symmetryID) {
	cout << "Creating head" << endl;
	BodyPart::NerveInfo nerveInfo;
	nerveInfo.inputEnabled = false;
	nerveInfo.outputEnabled = false;
	nerveInfo.inputIndex = 0;
	nerveInfo.outputIndex = 0;

	float angle = GetNextGene(gene, 0, 3) * 360;

	shared_ptr<BodySegment> newPart = make_shared<BodySegment>(BodySegment(
		shared_from_this(),
		b2Vec2(currentGenes.width, currentGenes.height),
		al_map_rgb(currentGenes.r, currentGenes.g, currentGenes.b),
		currentGenes.shapeType,
		startingPos,
		Util::DegreesToRadians(angle),
		nerveInfo
	));
	AddPart(newPart);
	this->head = newPart;

	for (int i = 0; i < 2; i++)
		symmetryMap[symmetryID].push_back(newPart);
}

void Creature::CreateBodySegment(string gene, CurrentGenes &currentGenes, vector<shared_ptr<BodyPart>> &parentObjects, unordered_map<int, vector<shared_ptr<BodyPart>>> &symmetryMap, int &symmetryID) {
	Joint::JointInfo jointInfo;
	jointInfo.useSpring = bool(int(GetNextGene(gene, 1, 0)) % 2);
	jointInfo.enableMotor = bool(int(GetNextGene(gene, 1, 0)) % 2);
	jointInfo.maxMotorTorque = 20.0;
	jointInfo.motorSpeed = 0;
	jointInfo.enableLimit = bool(int(GetNextGene(gene, 1, 0)) % 2);
	jointInfo.angleLimit = GetNextGene(gene, 0, 3) * M_PI;

	BodyPart::NerveInfo nerveInfo;
	nerveInfo.inputEnabled = int(GetNextGene(gene, 1, 0)) % 2 == 0;
	nerveInfo.outputEnabled = int(GetNextGene(gene, 1, 0)) % 2 == 0;
	nerveInfo.inputIndex = int(GetNextGene(gene, 0, 2) * extraInputCount) + baseInputs;
	nerveInfo.outputIndex = int(GetNextGene(gene, 0, 2) * extraOutputCount) + baseOutputs;


	int angleOnParent = parentObjects[0]->GetValidChildAngle(currentGenes.childAngleGene);
	cout << "angleOnParent1: " << angleOnParent << endl;
	shared_ptr<BodyPart> newPart = make_shared<BodySegment>(BodySegment(
		shared_from_this(),
		b2Vec2(currentGenes.width, currentGenes.height),
		al_map_rgb(currentGenes.r, currentGenes.g, currentGenes.b),
		currentGenes.shapeType,
		parentObjects[0],
		Util::DegreesToRadians(angleOnParent),
		Util::DegreesToRadians(currentGenes.angleOffset),
		jointInfo,
		nerveInfo
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
			nerveInfo.inputIndex = (nerveInfo.inputIndex + extraInputCount / 2) % (baseInputs + extraInputCount);
			nerveInfo.outputIndex = (nerveInfo.outputIndex + extraOutputCount / 2) % (baseOutputs + extraOutputCount);

			cout << "angleOnParent2: " << angleOnParent << endl;
			newPart = make_shared<BodySegment>(BodySegment(
				shared_from_this(),
				b2Vec2(currentGenes.width, currentGenes.height),
				al_map_rgb(currentGenes.r, currentGenes.g, currentGenes.b),
				currentGenes.shapeType,
				parentObjects[1],
				Util::DegreesToRadians(angleOnParent),
				Util::DegreesToRadians(-currentGenes.angleOffset),
				jointInfo,
				nerveInfo
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

	BodyPart::NerveInfo nerveInfo;
	nerveInfo.inputEnabled = true;
	nerveInfo.outputEnabled = false;
	nerveInfo.inputIndex = int(GetNextGene(gene, 0, 2) * extraInputCount) + baseInputs;
	nerveInfo.outputIndex = int(GetNextGene(gene, 0, 2) * extraOutputCount) + baseOutputs;

	int angleOnParent = parentObjects[0]->GetValidChildAngle(currentGenes.childAngleGene);
	shared_ptr<BodyPart> newPart = make_shared<Mouth>(Mouth(
		shared_from_this(),
		dynamic_pointer_cast<BodySegment>(parentObjects[0]),
		b2Vec2(currentGenes.width, currentGenes.height),
		al_map_rgb(255, 0, 0),
		Util::DegreesToRadians(angleOnParent),
		Util::DegreesToRadians(currentGenes.angleOffset),
		jointInfo,
		nerveInfo
	));

	AddPart(newPart);
	parentObjects[0]->AddChild(newPart, angleOnParent);
	symmetryMap[symmetryID].push_back((newPart));

	// Don't do symmetry if angle is down
	if (angleOnParent != 270)
		angleOnParent = (180 - angleOnParent + 360) % 360;

	if (parentObjects[1]->childAngleValid(angleOnParent)) {
		nerveInfo.inputIndex = int(GetNextGene(gene, 0, 2) * extraInputCount) + baseInputs;
		nerveInfo.outputIndex = int(GetNextGene(gene, 0, 2) * extraOutputCount) + baseOutputs;

		newPart = make_shared<Mouth>(Mouth(
			shared_from_this(),
			dynamic_pointer_cast<BodySegment>(parentObjects[1]),
			b2Vec2(currentGenes.width, currentGenes.height),
			al_map_rgb(255, 0, 0),
			Util::DegreesToRadians(angleOnParent),
			Util::DegreesToRadians(-currentGenes.angleOffset),
			jointInfo,
			nerveInfo
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

	BodyPart::NerveInfo nerveInfo;
	nerveInfo.inputEnabled = int(GetNextGene(gene, 1, 0)) % 2 == 0;
	nerveInfo.outputEnabled = false;
	nerveInfo.inputIndex = int(GetNextGene(gene, 0, 2) * extraInputCount) + baseInputs;
	nerveInfo.outputIndex = int(GetNextGene(gene, 0, 2) * extraOutputCount) + baseOutputs;

	int angleOnParent = parentObjects[0]->GetValidChildAngle(currentGenes.childAngleGene);
	shared_ptr<BodyPart> newPart = make_shared<Cilium>(Cilium(shared_from_this(), dynamic_pointer_cast<BodySegment>(parentObjects[0]), b2Vec2(currentGenes.width, currentGenes.height), al_map_rgb(255, 0, 0), Util::DegreesToRadians(angleOnParent), Util::DegreesToRadians(currentGenes.angleOffset), jointInfo, nerveInfo));

	AddPart(newPart);
	parentObjects[0]->AddChild(newPart, angleOnParent);
	symmetryMap[symmetryID].push_back((newPart));

	// Don't do symmetry if angle is down
	if (angleOnParent != 270)
		angleOnParent = (180 - angleOnParent + 360) % 360;

	if (parentObjects[1]->childAngleValid(angleOnParent)) {
		nerveInfo.inputIndex = int(GetNextGene(gene, 0, 2) * extraInputCount) + baseInputs;
		nerveInfo.outputIndex = int(GetNextGene(gene, 0, 2) * extraOutputCount) + baseOutputs;

		newPart = make_shared<Cilium>(Cilium(shared_from_this(), dynamic_pointer_cast<BodySegment>(parentObjects[1]), b2Vec2(currentGenes.width, currentGenes.height), al_map_rgb(255, 0, 0), Util::DegreesToRadians(angleOnParent), Util::DegreesToRadians(-currentGenes.angleOffset), jointInfo, nerveInfo));

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

	BodyPart::NerveInfo nerveInfo;
	nerveInfo.inputEnabled = false;
	nerveInfo.outputEnabled = true;
	nerveInfo.inputIndex = int(GetNextGene(gene, 0, 2) * extraInputCount) + baseInputs;
	nerveInfo.outputIndex = int(GetNextGene(gene, 0, 2) * extraOutputCount) + baseOutputs;

	cout << "eye " << nerveInfo.inputIndex << " " << nerveInfo.outputIndex << endl;

	int angleOnParent = parentObjects[0]->GetValidChildAngle(currentGenes.childAngleGene);
	shared_ptr<BodyPart> newPart = make_shared<Eye>(Eye(
		shared_from_this(),
		dynamic_pointer_cast<BodySegment>(parentObjects[0]),
		b2Vec2(currentGenes.width, currentGenes.height),
		al_map_rgb(currentGenes.r, currentGenes.g, currentGenes.b),
		Util::DegreesToRadians(angleOnParent),
		Util::DegreesToRadians(currentGenes.angleOffset),
		jointInfo,
		nerveInfo
	));

	AddPart(newPart);
	parentObjects[0]->AddChild(newPart, angleOnParent);
	symmetryMap[symmetryID].push_back((newPart));

	// Don't do symmetry if angle is down
	if (angleOnParent != 270)
		angleOnParent = (180 - angleOnParent + 360) % 360;

	if (parentObjects[1]->childAngleValid(angleOnParent)) {
		nerveInfo.inputIndex = int(GetNextGene(gene, 0, 2) * extraInputCount) + baseInputs;
		nerveInfo.outputIndex = int(GetNextGene(gene, 0, 2) * extraOutputCount) + baseOutputs;
		cout << "eye " << nerveInfo.inputIndex << " " << nerveInfo.outputIndex << endl;

		newPart = make_shared<Eye>(Eye(
			shared_from_this(),
			dynamic_pointer_cast<BodySegment>(parentObjects[1]),
			b2Vec2(currentGenes.width, currentGenes.height),
			al_map_rgb(currentGenes.r, currentGenes.g, currentGenes.b),
			Util::DegreesToRadians(angleOnParent),
			Util::DegreesToRadians(-currentGenes.angleOffset),
			jointInfo,
			nerveInfo
		));

		AddPart(newPart);
		parentObjects[1]->AddChild(newPart, angleOnParent);
	}

	symmetryMap[symmetryID].push_back((newPart));
}
