#pragma once

#include "BodyPart.h"
#include "Joint.h"

class Eye : public BodyPart {
	private:
		bool seesObject;

	public:
		Eye(shared_ptr<Creature> parentCreature, shared_ptr<BodySegment> parentPart, b2Vec2 pixelSize, ALLEGRO_COLOR color, float angleOnParent, float angleOffset, Joint::JointInfo jointInfo, NerveInfo &nerveInfo);

		void Update();
		void Draw();
		void UpdateJoint();


		float GetNerveOutput();
		void SetNerveInput(float val);

};