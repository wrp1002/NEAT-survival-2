#pragma once

#include <allegro5/transformations.h>
#include <box2d/b2_joint.h>
#include <box2d/b2_world.h>
#include <box2d/b2_world_callbacks.h>
#include <string>
#include <vector>
#include <memory>

class BodySegment;

using namespace std;


class Creature : public std::enable_shared_from_this<Creature> {
    private:
        string genes;
        vector<shared_ptr<BodySegment>> bodySegments;
        vector<b2Joint *> joints;
        shared_ptr<BodySegment> head;
        b2World *world;

    public:
        Creature(string genes);
        ~Creature();
        void Init(b2World &world);

        void ApplyGenes(b2World &world, string genes);
        void ApplyGenes(b2World &world);

        void Update();
        void Draw();

        void AddJoint(b2Joint *joint);


        float decimalFromSubstring(string str, int wholeDigits, int decimalDigits);
        float GetNextGene(string &gene, int wholeDigits, int decimalDigits);
};