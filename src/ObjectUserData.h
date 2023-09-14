#pragma once

#include <string>
#include <memory>

class Object;

using namespace std;

class ObjectUserData {
	public:
		string objectType;
		weak_ptr<Object> parentObject;

		ObjectUserData();
		ObjectUserData(string objectType, weak_ptr<Object> parentObject);
};
