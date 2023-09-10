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

#include "../Util.h"

using namespace std;


void Creature::ApplyGenes(string genes) {
	int r = 100, g = 100, b = 100;
	int width = 50, height = 50;
	int shapeType = 0;
	int geneLength = 16;
	int angleOffset = 0;
	int childAngleGene = 0;

	this->head = nullptr;

	int symmetryID = 0;
	int selectedParentID = 0;
	unordered_map<int, vector<shared_ptr<BodyPart>>> symmetryMap;
	bool doSymmetry = true;

	int instructionTypes = 5;
	int maxSize = 50;

	// gene length == 16

	for (int i = 0; i < genes.size(); i += geneLength) {
		string gene = genes.substr(i, geneLength);
		cout << endl;
		cout << gene << endl;
		int instructionType = int(GetNextGene(gene, 3, 0)) % instructionTypes;;
		cout << "instructionType: " << instructionType << endl;

		switch(instructionType) {
			// shapeType(1), width(3), height(3), leftover(2)
			case 0: {
				shapeType = int(GetNextGene(gene, 1, 0)) % 2;
				width = Util::clamp(GetNextGene(gene, 0, 3) * maxSize, 10.0, 50.0);
				height = Util::clamp(GetNextGene(gene, 0, 3) * maxSize, 10.0, 50.0);
				cout << "shapetype:" << shapeType << " width: " << width << " height: " << height << endl;

				break;
			}
			// r(3), g(3), b(3), leftover(6)
			case 1: {
				r = GetNextGene(gene, 0, 3) * 255;
				g = GetNextGene(gene, 0, 3) * 255;
				b = GetNextGene(gene, 0, 3) * 255;
				cout << "Set color: " << r << " " << g << " " << b << endl;
				break;
			}
			// parentID(1)
			case 2: {
				cout << "Creating new object" << endl;
				if (!head) {
					cout << "Creating head" << endl;
					BodyPart::NerveInfo nerveInfo;
					nerveInfo.inputEnabled = int(GetNextGene(gene, 1, 0)) % 2 == 0;
					nerveInfo.outputEnabled = int(GetNextGene(gene, 1, 0)) % 2 == 0;
					nerveInfo.inputIndex = int(GetNextGene(gene, 0, 2) * extraInputCount) + baseInputs;
					nerveInfo.outputIndex = int(GetNextGene(gene, 0, 2) * extraOutputCount) + baseOutputs;
					this->head = make_shared<BodySegment>(BodySegment(shared_from_this(), b2Vec2(width, height), al_map_rgb(r, g, b), shapeType, startingPos, Util::DegreesToRadians(0), nerveInfo));
					bodySegments.push_back(head);

					for (int i = 0; i < 2; i++)
						symmetryMap[symmetryID].push_back(head);

					symmetryID++;
				}
				else {
					cout << "Creating BodySegment" << endl;
					cout << "body segments: " << bodySegments.size() << " selectedParentID:" << selectedParentID << endl;

					vector<shared_ptr<BodyPart>> parentObjects = symmetryMap[selectedParentID];

					if (parentObjects[0]->CanAddChild() && parentObjects[1]->CanAddChild()) {
                        cout << "type3id: " << parentObjects[0]->GetPolymorphicID() << endl;
                        int partType = GetNextGene(gene, 1, 0);

						shared_ptr<BodyPart> newPart;

                        if (partType == 9) {
							Joint::JointInfo jointInfo;
                            jointInfo.useSpring = false;
                            jointInfo.enableMotor = false;
                            jointInfo.maxMotorTorque = 0;
                            jointInfo.motorSpeed = 0;
                            jointInfo.enableLimit = true;
                            jointInfo.angleLimit = 0.1;

                            BodyPart::NerveInfo nerveInfo;
                            nerveInfo.inputEnabled = int(GetNextGene(gene, 1, 0)) % 2 == 0;
                            nerveInfo.outputEnabled = int(GetNextGene(gene, 1, 0)) % 2 == 0;
                            nerveInfo.inputIndex = int(GetNextGene(gene, 0, 2) * extraInputCount) + baseInputs;
                            nerveInfo.outputIndex = int(GetNextGene(gene, 0, 2) * extraOutputCount) + baseOutputs;


                            int angleOnParent = parentObjects[0]->GetValidChildAngle(childAngleGene);
                            cout << "angleOnParent1: " << angleOnParent << endl;

							newPart = make_shared<Cilium>(Cilium(shared_from_this(), dynamic_pointer_cast<BodySegment>(parentObjects[0]), b2Vec2(width, height), al_map_rgb(255, 0, 0), Util::DegreesToRadians(angleOnParent), Util::DegreesToRadians(angleOffset), jointInfo, nerveInfo));

							bodySegments.push_back(newPart);
                            parentObjects[0]->AddChild(newPart, angleOnParent);
                            symmetryMap[symmetryID].push_back((newPart));

                            if (doSymmetry) {
                                // Don't do symmetry if angle is down
                                if (angleOnParent != 270)
                                    angleOnParent = (180 - angleOnParent + 360) % 360;

                                if (parentObjects[1]->childAngleValid(angleOnParent)) {
                                    nerveInfo.inputIndex = (nerveInfo.inputIndex + extraInputCount / 2) % (baseInputs + extraInputCount);
                                    nerveInfo.outputIndex = (nerveInfo.outputIndex + extraOutputCount / 2) % (baseOutputs + extraOutputCount);

                                    cout << "angleOnParent2: " << angleOnParent << endl;

                                    newPart = make_shared<Cilium>(Cilium(shared_from_this(), dynamic_pointer_cast<BodySegment>(parentObjects[1]), b2Vec2(width, height), al_map_rgb(255, 0, 0), Util::DegreesToRadians(angleOnParent), Util::DegreesToRadians(-angleOffset), jointInfo, nerveInfo));

                                    bodySegments.push_back(newPart);
                                    parentObjects[1]->AddChild(newPart, angleOnParent);
                                }
                            }

                            symmetryMap[symmetryID].push_back((newPart));

                            symmetryID++;
                            selectedParentID = symmetryID - 1;
                        }
                        else if (partType < 9) {
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


                            int angleOnParent = parentObjects[0]->GetValidChildAngle(childAngleGene);
                            cout << "angleOnParent1: " << angleOnParent << endl;
                            newPart = make_shared<BodySegment>(BodySegment(shared_from_this(), b2Vec2(width, height), al_map_rgb(r, g, b), shapeType, parentObjects[0], Util::DegreesToRadians(angleOnParent), Util::DegreesToRadians(angleOffset), jointInfo, nerveInfo));
                            bodySegments.push_back(newPart);
                            parentObjects[0]->AddChild(newPart, angleOnParent);
                            symmetryMap[symmetryID].push_back((newPart));

                            if (doSymmetry) {
                                // Don't do symmetry if angle is down
                                if (angleOnParent != 270)
                                    angleOnParent = (180 - angleOnParent + 360) % 360;

                                if (parentObjects[1]->childAngleValid(angleOnParent)) {
                                    nerveInfo.inputIndex = (nerveInfo.inputIndex + extraInputCount / 2) % (baseInputs + extraInputCount);
                                    nerveInfo.outputIndex = (nerveInfo.outputIndex + extraOutputCount / 2) % (baseOutputs + extraOutputCount);

                                    cout << "angleOnParent2: " << angleOnParent << endl;
                                    newPart = make_shared<BodySegment>(BodySegment(shared_from_this(), b2Vec2(width, height), al_map_rgb(r, g, b), shapeType, parentObjects[1], Util::DegreesToRadians(angleOnParent), Util::DegreesToRadians(-angleOffset), jointInfo, nerveInfo));
                                    bodySegments.push_back(newPart);
                                    parentObjects[1]->AddChild(newPart, angleOnParent);
                                }
                            }

                            symmetryMap[symmetryID].push_back((newPart));

                            symmetryID++;
                            selectedParentID = symmetryID - 1;
                        }
					}
				}
				break;
			}
			// angleOnParent(1), angleOffset(3)
			case 3: {
				childAngleGene = GetNextGene(gene, 1, 0);
				angleOffset = GetNextGene(gene, 0, 3) * 90 - 45;
				cout << "Set childAngleGene:" << childAngleGene << " angleOffset:" << angleOffset << endl;
				break;
			}
			// parentID(1)
			case 4: {
				if (bodySegments.size() > 0) {
					selectedParentID = int(GetNextGene(gene, 1, 0)) % symmetryID;
					cout << "Set symmetryID:" << symmetryID << endl;
				}
				break;
			}
		}

	}

}
