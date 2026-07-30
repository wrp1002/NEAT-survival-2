#pragma once

#include "BodyPart.h"
#include "Joint.h"

class EyeRayCastCallback : public b2RayCastCallback {
public:

	float closestFraction = 1.0f;
	shared_ptr<Object> hitObject = nullptr;

	float ReportFixture(
		b2Fixture* fixture,
		const b2Vec2& point,
		const b2Vec2& normal,
		float fraction
	) override {

		ObjectUserData* userData =
			reinterpret_cast<ObjectUserData*>(fixture->GetUserData());

		if (!userData)
			return -1;

		auto object = userData->parentObject.lock();

		if (!object)
			return -1;

		// Ignore self
		if (object->GetType() == "Eye")
			return -1;

		hitObject = object;
		closestFraction = fraction;

		// returning fraction means:
		// "keep looking, but this is the closest hit so far"
		return fraction;
	}
};


class Eye : public BodyPart {
	public:
		// Suggested ranges:
		// range:     50 - 400 pixels
		// fov:       5 - 120 degrees
		// rayCount:  1 - 7
		// targetHue: 0 - 360 degrees
		struct EyeInfo {
			float range = 200;
			float fov = 30;
			int rayCount = 5;
			float targetHue = 0;
		};

	private:
		bool seesObject;
		vector<float> rayValues;
		EyeInfo eyeInfo;
		float visionStrength;
		float visionDirection;
		int visionTimer = 0;
		int visionTimerStart = 5;

	public:
		Eye(shared_ptr<Creature> parentCreature, shared_ptr<BodySegment> parentPart, b2Vec2 pixelSize, ALLEGRO_COLOR color, float angleOnParent, float angleOffset, Joint::JointInfo jointInfo, vector<NerveInfo> &nerveInfo, EyeInfo &eyeInfo);

		void UpdateVisionValues();
		void Update();
		void Draw();
		void UpdateJoint();


		float GetNerveOutput(NerveType type);
		void SetNerveInput(NerveType type, float val);

};