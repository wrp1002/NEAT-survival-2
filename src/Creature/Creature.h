#pragma once

#include <allegro5/transformations.h>
#include <box2d/b2_joint.h>
#include <box2d/b2_math.h>
#include <box2d/b2_world.h>
#include <box2d/b2_world_callbacks.h>
#include <string>
#include <vector>
#include <memory>

class BodySegment;
class Joint;

using namespace std;


class Creature : public std::enable_shared_from_this<Creature> {
    private:
        b2Vec2 startingPos;
        string genes;
        vector<shared_ptr<BodySegment>> bodySegments;
        vector<shared_ptr<Joint>> joints;
        shared_ptr<BodySegment> head;

    public:
        Creature(string genes, b2Vec2 pos);
        ~Creature();
        void Init();

        void ApplyGenes(string genes);
        void ApplyGenes();

        void Update();
        void Draw();

        void AddJoint(shared_ptr<Joint> newJoint);


        float decimalFromSubstring(string str, int wholeDigits, int decimalDigits);
        float GetNextGene(string &gene, int wholeDigits, int decimalDigits);
};