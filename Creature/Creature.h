#pragma once

#include <string>
#include <vector>
#include <memory>

class BodySegment;

using namespace std;

class Creature {
    private:
    string genes;
    vector<shared_ptr<BodySegment>> bodySegments;
    shared_ptr<BodySegment> head;

    public:
    Creature(string genes);
};