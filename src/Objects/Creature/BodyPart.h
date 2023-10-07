#pragma once

#include <allegro5/color.h>
#include <box2d/b2_math.h>

#include <memory>
#include <string>

#include "../Object.h"

class BodySegment;
class Creature;
class Joint;

class BodyPart : public Object {
	public:
		struct NerveInfo {
			bool inputEnabled;
			bool outputEnabled;
			int inputIndex;
			bool outputIndex;
		};

	protected:
		NerveInfo nerveInfo;
		shared_ptr<Joint> parentJoint;
		weak_ptr<Creature> creature;
		weak_ptr<BodyPart> parentPart;

		float health, maxHealth;
		float energy, maxEnergy;

		static b2Vec2 GetPosOnParent(shared_ptr<BodyPart> parent, float angleOnObject, float angleOffset, b2Vec2 thisWorldSize);

	public:
		BodyPart(shared_ptr<Creature> parentCreature, ALLEGRO_COLOR color, NerveInfo &nerveInfo);
		BodyPart(shared_ptr<Creature> parentCreature, b2Vec2 pos, b2Vec2 pixelSize, float angle, ALLEGRO_COLOR color, int shapeType, NerveInfo &nerveInfo);

		virtual void Update();
		virtual void Draw();
		virtual void Destroy();
		virtual void Print();

		virtual void UpdateJoint();
		void SetParentJoint(shared_ptr<Joint> newJoint);
		void DestroyJoint();

		b2Body *GetBody();
		weak_ptr<Creature> GetParentCreature();

		void TakeDamage(double amount);

		virtual bool CanAddChild();
		virtual bool childAngleValid(int angle);
		virtual int GetValidChildAngle(int angleGene);
		virtual void AddChild(shared_ptr<BodyPart> child, int angle);

		virtual float GetNerveOutput();
		int GetNerveOutputIndex();
		int GetNerveInputIndex();
		virtual void SetNerveInput(float val);

		double GetEnergy();
};

