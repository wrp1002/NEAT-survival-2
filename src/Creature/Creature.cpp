#include "Creature.h"

#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>

#include <box2d/b2_math.h>
#include <box2d/b2_world.h>
#include <box2d/box2d.h>

#include <iostream>
#include <unordered_map>

#include "BodySegment.h"
#include "../Globals.h"
#include "../Util.h"

using namespace std;

Creature::Creature(string genes) {
    this->genes = genes;
}

Creature::~Creature() {
    for (auto child : bodySegments)
        this->world->DestroyBody(child->GetBody());
}

void Creature::Init(b2World &world) {
    this->world = &world;
    ApplyGenes(world);
}

void Creature::ApplyGenes(b2World &world) {
    ApplyGenes(world, this->genes);
}

void Creature::ApplyGenes(b2World &world, string genes) {
    int r = 100, g = 100, b = 100;
    int width = 50, height = 50;
    int shapeType = 0;
    int geneLength = 16;
    int angleOffset = 0;
    int parentID = 0;
    int childAngleGene = 0;

    this->head = nullptr;
    shared_ptr<BodySegment> prevPart = nullptr;

    unordered_map<int, vector<shared_ptr<BodySegment>>> symmetryMap;

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
                    this->head = make_shared<BodySegment>(BodySegment(world, shared_from_this(), b2Vec2(width, height), al_map_rgb(r, g, b), shapeType, b2Vec2(Globals::SCREEN_WIDTH / 2.0, Globals::SCREEN_HEIGHT / 2.0), Util::DegreesToRadians(0)));
                    prevPart = this->head;
                    bodySegments.push_back(head);
                }
                else {
                    cout << "Creating BodySegment" << endl;
                    cout << "body segments: " << bodySegments.size() << " parentID:" << parentID << endl;
                    shared_ptr<BodySegment> parentObject = bodySegments[parentID];

                    if (parentObject->CanAddChild()) {
                        int angleOnParent = parentObject->GetValidChildAngle(childAngleGene);

                        shared_ptr<BodySegment> newPart = make_shared<BodySegment>(BodySegment(world, shared_from_this(), b2Vec2(width, height), al_map_rgb(r, g, b), shapeType, parentObject, Util::DegreesToRadians(angleOnParent), Util::DegreesToRadians(angleOffset)));
                        bodySegments.push_back(newPart);
                        prevPart->AddChild(newPart, angleOnParent);
                        prevPart = newPart;
                        parentID = bodySegments.size() - 1;
                    }
                    else {
                        cout << "no space left" << endl;
                    }
                }
                break;
            }
            // angleOnParent(1), angleOffset(3)
            case 3: {
                childAngleGene = GetNextGene(gene, 1, 0);
                angleOffset = GetNextGene(gene, 0, 3) * 40 - 20;
                cout << "Set childAngleGene:" << childAngleGene << " angleOffset:" << angleOffset << endl;
                break;
            }
            // parentID(1)
            case 4: {
                if (bodySegments.size() > 0) {
                    parentID = int(GetNextGene(gene, 1, 0)) % bodySegments.size();
                    cout << "Set parentID:" << parentID << endl;
                }
                break;
            }
        }

    }

    //string colorGene = genes.substr(section * geneLength, section * geneLength + geneLength);
    //cout << colorGene << endl;
    //int first = colorGene[0];


    /*
    this->head = make_shared<BodySegment>(BodySegment(world, shared_from_this(), b2Vec2(50, 50), al_map_rgb(r, g, b), Object::SHAPE_TYPES::RECT, b2Vec2(Globals::SCREEN_WIDTH / 2.0, Globals::SCREEN_HEIGHT / 2.0), Util::DegreesToRadians(45)));
    shared_ptr<BodySegment> prevPart = head;

    for (int i = 0; i < 4; i ++) {
        int size = (i * 10) + 10;
        shared_ptr<BodySegment> newPart = make_shared<BodySegment>(BodySegment(world, shared_from_this(), b2Vec2(100, 10), al_map_rgb(r, g, b), Object::RECT, prevPart, Util::DegreesToRadians(15), Util::DegreesToRadians(10)));
        prevPart->AddChild(newPart);
        prevPart = newPart;
    }

    shared_ptr<BodySegment> newPart = make_shared<BodySegment>(BodySegment(world, shared_from_this(), b2Vec2(75, 25), al_map_rgb(r, g, b), Object::SHAPE_TYPES::RECT, head, Util::DegreesToRadians(180), Util::DegreesToRadians(0)));
    prevPart->AddChild(newPart);
    */

}


void Creature::Update() {
    for (int i = joints.size() - 1; i >= 0; i--) {
        b2Vec2 reactionForce = joints[i]->GetReactionForce(Globals::FPS);
        float forceModuleSq = reactionForce.LengthSquared();
        if(forceModuleSq > 0.0075) {
            cout << "deleting joint" << endl;
            world->DestroyJoint(joints[i]);
            joints.erase(joints.begin() + i);
        }
    }
}

void Creature::Draw() {
    head->Draw();

    for (auto joint : joints) {
        b2Vec2 pos;
        pos = Util::metersToPixels(joint->GetAnchorA());
        al_draw_filled_circle(pos.x, pos.y, 2, al_map_rgb(0, 255, 0));

        pos = Util::metersToPixels(joint->GetAnchorB());
        al_draw_filled_circle(pos.x, pos.y, 2, al_map_rgb(0, 255, 0));
    }
}


void Creature::AddJoint(b2Joint *joint) {
    joints.push_back(joint);
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