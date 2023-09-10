#pragma once

#include <box2d/b2_body.h>
#include <box2d/b2_distance_joint.h>
#include <box2d/b2_joint.h>
#include <box2d/b2_math.h>
#include <box2d/b2_revolute_joint.h>
#include <box2d/b2_world.h>

#include <vector>
#include <iostream>

using namespace std;


class Joint {
	private:
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