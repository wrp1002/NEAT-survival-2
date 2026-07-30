#pragma once

#include "BodyPart.h"
#include "Joint.h"

class Creature;

using namespace std;

class Mouth : public BodyPart {
	private:
		static const int biteCooldown = 20;

		bool biting;
		bool triggerBiteDamage;
		int cooldownTimer;
		float animationRate;
		float animationFrame;
		float animationState;

	public:
		Mouth(shared_ptr<Creature> parentCreature, shared_ptr<BodySegment> parentPart, b2Vec2 pixelSize, ALLEGRO_COLOR color, float angleOnParent, float angleOffset, Joint::JointInfo jointInfo, vector<NerveInfo> &nerveInfo);

		void Update();
		void Draw();
		void UpdateJoint();


		float GetNerveOutput(NerveType type);
		void SetNerveInput(NerveType type, float val);

		bool CanBite();
};
