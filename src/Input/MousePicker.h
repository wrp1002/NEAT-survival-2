#pragma once

#include <memory>
#include <iostream>

#include <Box2D/Box2D.h>

using namespace std;

class Object;

namespace MousePicker {

	std::shared_ptr<Object> GetObjectAt(b2Vec2 worldPos);

}
