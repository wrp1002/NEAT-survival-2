#include "JointDestructionListener.h"
#include "../Objects/UserData/JointUserData.h"

#include "../Objects/Creature/Joint.h"

#include <iostream>

using namespace std;

void JointDestructionListener::SayGoodbye(b2Joint* joint) {
	// remove all references to joint.
	cout << "joint break!" << endl;

	JointUserData *userData = reinterpret_cast<JointUserData *>(joint->GetUserData());

	if (!userData) {
		cout << "missing joint user data!" << endl;
		return;
	}

	cout << userData->objectType << endl;

	if (userData->parentObject.expired())
		return;

	if (shared_ptr<Joint> parentJoint = userData->parentObject.lock()) {
		parentJoint->RemoveJoint(joint);
		parentJoint->SetBroken(true);
	}
}

void JointDestructionListener::SayGoodbye(b2Fixture* fixture) {
	//cout << "fixture break" << endl;
}