#pragma once

#include <string>
#include <memory>

class Joint;

using namespace std;

class JointUserData {
	public:
		string objectType;
		weak_ptr<Joint> parentObject;

		JointUserData();
		JointUserData(string objectType, weak_ptr<Joint> parentObject);
};
