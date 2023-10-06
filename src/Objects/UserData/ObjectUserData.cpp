#include "ObjectUserData.h"

ObjectUserData::ObjectUserData() {
	this->objectType = "null";
	this->parentObject.reset();
}

ObjectUserData::ObjectUserData(string objectType, weak_ptr<Object> parentObject) {
	this->objectType = objectType;
	this->parentObject = parentObject;
}
