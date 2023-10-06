#pragma once

#include "Creature.h"
#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>

#include <box2d/b2_body.h>
#include <box2d/b2_joint.h>
#include <box2d/b2_math.h>
#include <box2d/b2_shape.h>
#include <box2d/box2d.h>

#include <vector>
#include <memory>

#include "BodyPart.h"
#include "Joint.h"

using namespace std;


class BodySegment : public BodyPart {
	private:
		vector<weak_ptr<BodyPart>> children;
		vector<float> validChildAngles;
		int angleOnParent;

	public:
		BodySegment(shared_ptr<Creature> parentCreature, b2Vec2 pixelSize, ALLEGRO_COLOR color, int shapeType, b2Vec2 pos, float angle, NerveInfo &nerveInfo);
		BodySegment(shared_ptr<Creature> parentCreature, b2Vec2 pixelSize, ALLEGRO_COLOR color, int shapeType, shared_ptr<BodyPart> parent, float angleOnParent, float angleOffset, Joint::JointInfo jointInfo, NerveInfo &nerveInfo);


		void Draw();

		void AddChild(shared_ptr<BodyPart> child, int angle);

		void SetValidAngles(b2Vec2 pixelSize);
		bool childAngleValid(int angle);
		bool CanAddChild();
		int GetValidChildAngle(int angleGene);
};