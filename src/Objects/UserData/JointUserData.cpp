#include "JointUserData.h"

JointUserData::JointUserData() {
	this->objectType = "null";
	this->parentObject.reset();
}

JointUserData::JointUserData(string objectType, weak_ptr<Joint> parentObject) {
	this->objectType = objectType;
	this->parentObject = parentObject;
}
