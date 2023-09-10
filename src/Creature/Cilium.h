#pragma once

#include <memory>

#include "BodyPart.h"
#include "BodySegment.h"

class Creature;
class BodySegment;

class Cilium : public BodyPart {
    private:
        float angle;
        float maxSpeed;
        float currentSpeed;

    public:
        Cilium(shared_ptr<Creature> parentCreature, shared_ptr<BodySegment> parentPart, b2Vec2 pixelSize, ALLEGRO_COLOR color, float angleOnParent, float angleOffset, Joint::JointInfo jointInfo, NerveInfo &nerveInfo);

    void Update();
    void Draw();


    float GetNerveOutput();
    void SetNerveInput(float val);
};
