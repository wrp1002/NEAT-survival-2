#pragma once

#include <Box2D/Box2D.h>

#include <memory>
#include <vector>
#include <iostream>

#include "../UserData/JointUserData.h"

using namespace std;


class Joint : public enable_shared_from_this<Joint> {
	private:
		shared_ptr<JointUserData> userData;
		b2RevoluteJoint *revoluteJoint;
		b2DistanceJoint *springJoint;
		vector<b2Joint *> allJoints;
		bool broken;
		bool shouldDeleteJoints;
		float speed;

		bool JointShouldBreak(b2Joint *joint);

	public:
		struct JointInfo {
			bool useSpring;
			bool enableLimit;
			float angleLimit;
			bool enableMotor;
			float maxMotorTorque;
			float motorSpeed;
		};

		Joint(JointInfo jointInfo, b2Vec2 jointPos, b2Body *bodyA, b2Body *bodyB);
		~Joint();
		void UpdateUserData();


		void Update();
		void Draw();
		void Destroy();
		void RemoveJoint(b2Joint *joint);


		void SetBroken(bool val);
		void SetShouldDeleteJoints(bool val);
		void SetSpeed(float val);

		bool IsBroken();
		float GetAngle();
};