#include "MousePicker.h"

#include "../Objects/Object.h"
#include "../Objects/UserData/ObjectUserData.h"
#include "../GameManager.h"


class MouseQueryCallback : public b2QueryCallback {
public:

	b2Vec2 point;
	std::shared_ptr<Object> foundObject;

	MouseQueryCallback(b2Vec2 point) : point(point) {}

	bool ReportFixture(b2Fixture* fixture) override {
		if (!fixture->TestPoint(point)) {
			return true;
		}

		auto userData = reinterpret_cast<ObjectUserData*>(fixture->GetUserData());

		if (!userData)
			return true;

		foundObject = userData->parentObject.lock();

		// Stop searching once we found something
		return false;
	}
};


std::shared_ptr<Object> MousePicker::GetObjectAt(b2Vec2 worldPos) {
	MouseQueryCallback callback(worldPos);

	b2AABB aabb;
	aabb.lowerBound = worldPos - b2Vec2(0.001f, 0.001f);
	aabb.upperBound = worldPos + b2Vec2(0.001f, 0.001f);

	GameManager::world.QueryAABB(&callback, aabb);

	return callback.foundObject;
}
