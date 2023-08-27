#include "Creature.h"

#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>

#include <box2d/b2_world.h>
#include <box2d/box2d.h>

#include <iostream>

#include "BodySegment.h"
#include "../Globals.h"
#include "../Util.h"

using namespace std;

Creature::Creature(string genes) {
    this->genes = genes;
}

void Creature::Init(b2World &world) {
    ApplyGenes(world);
}

void Creature::ApplyGenes(b2World &world) {
    ApplyGenes(world, this->genes);
}

void Creature::ApplyGenes(b2World &world, string genes) {
    int section = 0;
    int geneLength = 16;
    string colorGene = genes.substr(section * geneLength, section * geneLength + geneLength);
    cout << colorGene << endl;
    int first = colorGene[0];
    int r = decimalFromSubstring(colorGene.substr(1, 3), 3, 0);
    int g = decimalFromSubstring(colorGene.substr(4, 3), 3, 0);
    int b = decimalFromSubstring(colorGene.substr(7, 3), 3, 0);
    cout << r << " " << g << " " << b << endl;


    this->head = make_shared<BodySegment>(BodySegment(world, shared_from_this(), Util::pixelsToMeters(Globals::SCREEN_WIDTH / 2.0, Globals::SCREEN_HEIGHT / 2.0), 50, al_map_rgb(r, g, b)));
    shared_ptr<BodySegment> prevPart = head;

    for (int i = 0; i < 5; i ++) {
        shared_ptr<BodySegment> newPart = make_shared<BodySegment>(BodySegment(world, shared_from_this(), prevPart, -45, 0, 50, al_map_rgb(255, 0, 255)));
        prevPart->AddChild(newPart);
        prevPart = newPart;
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