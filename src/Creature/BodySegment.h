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

#include "../Object.h"
#include "Joint.h"

using namespace std;


class BodySegment : public Object {
	private:
		weak_ptr<Creature> creature;
		vector<weak_ptr<BodySegment>> children;
		vector<float> validChildAngles;
		int angleOnParent;
		b2Joint *parentJoint;

		b2Vec2 GetPosOnParent(shared_ptr<BodySegment> parent, float angleOnObject, float angleOffset, b2Vec2 thisWorldSize);

	public:
		int angleOffset;
		int innovationNum;

		BodySegment(shared_ptr<Creature> parentCreature, b2Vec2 pixelSize, ALLEGRO_COLOR color, int shapeType, b2Vec2 pos, float angle);
		BodySegment(shared_ptr<Creature> parentCreature, b2Vec2 pixelSize, ALLEGRO_COLOR color, int shapeType, shared_ptr<BodySegment> parent, float angleOnParent, float angleOffset, Joint::JointInfo jointInfo);

		void Draw();

		b2Body *GetBody();

		void AddChild(shared_ptr<BodySegment> child, int angle);

		void SetValidAngles(b2Vec2 pixelSize);
		bool childAngleValid(int angle);
		bool CanAddChild();
		int GetValidChildAngle(int angleGene);
};