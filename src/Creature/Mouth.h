#pragma once

#include "BodyPart.h"
#include "Joint.h"

class Creature;

using namespace std;

class Mouth : public BodyPart {
	private:
		bool isBiting;
		float animationRate;
		float animationFrame;
		float animationState;

	public:
		Mouth(shared_ptr<Creature> parentCreature, shared_ptr<BodySegment> parentPart, b2Vec2 pixelSize, ALLEGRO_COLOR color, float angleOnParent, float angleOffset, Joint::JointInfo jointInfo, NerveInfo &nerveInfo);

		void Update();
		void Draw();


		float GetNerveOutput();
		void SetNerveInput(float val);
};