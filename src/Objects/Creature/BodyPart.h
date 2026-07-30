#pragma once

#include <allegro5/color.h>
#include <Box2D/Box2D.h>

#include <memory>
#include <string>
#include <vector>

#include "../LiveObject.h"

class BodySegment;
class Creature;
class Joint;

enum NerveType {
	Activation,
	Direction
};
struct NerveInfo {
	bool inputEnabled = false;
	bool outputEnabled = false;

	int inputIndex = -1;
	int outputIndex = -1;

	NerveType type = NerveType::Activation;

	NerveInfo(
		NerveType type,
		int inputIndex = -1,
		int outputIndex = -1
	)
	{
		this->type = type;
		this->outputIndex = outputIndex;
		this->inputIndex = inputIndex;

		outputEnabled = outputIndex >= 0;
		inputEnabled = inputIndex >= 0;
	}
};

class BodyPart : public LiveObject {
	protected:
		vector<NerveInfo> nerves;
		shared_ptr<Joint> parentJoint;
		weak_ptr<Creature> creature;
		weak_ptr<BodyPart> parentPart;

		double energyUsage;

		static b2Vec2 GetPosOnParent(shared_ptr<BodyPart> parent, float angleOnObject, float angleOffset, b2Vec2 thisWorldSize);

	public:
		BodyPart(shared_ptr<Creature> parentCreature, ALLEGRO_COLOR color, vector<NerveInfo> &nerveInfo);
		BodyPart(shared_ptr<Creature> parentCreature, b2Vec2 pos, b2Vec2 pixelSize, float angle, ALLEGRO_COLOR color, int shapeType, vector<NerveInfo> &nerveInfo);

		virtual void Update();
		virtual void Draw();
		virtual void Destroy();
		virtual void Print();

		virtual void UpdateJoint();
		void SetParentJoint(shared_ptr<Joint> newJoint);
		void DestroyJoint();

		b2Body *GetBody();
		weak_ptr<Creature> GetParentCreature();

		virtual bool CanAddChild();
		virtual bool childAngleValid(int angle);
		virtual int GetValidChildAngle(int angleGene);
		virtual void AddChild(shared_ptr<BodyPart> child, int angle);

		bool NerveInputEnabled();
		bool NerveOutputEnabled();
		virtual float GetNerveOutput(NerveType type);
		virtual void SetNerveInput(NerveType type, float val);

		vector<NerveInfo>& GetNerves();

		double SetHealth(double amount);

		double GetEnergyUsage();

};

