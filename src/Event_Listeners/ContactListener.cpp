#include "ContactListener.h"
#include <Box2D/Box2D.h>

#include "../GameManager.h"

#include <iostream>

using namespace std;

void MyContactListener::BeginContact(b2Contact* contact) {

	if (contact->GetFixtureA()->GetBody() == GameManager::worldBorder || contact->GetFixtureB()->GetBody() == GameManager::worldBorder) {
		HandleBorderBeginContact(contact);
	}

}

void MyContactListener::EndContact(b2Contact* contact) {
	b2Body *bodyA = contact->GetFixtureA()->GetBody();
	b2Body *bodyB = contact->GetFixtureB()->GetBody();
	ObjectUserData *userData1 = reinterpret_cast<ObjectUserData *>(bodyA->GetUserData());
	ObjectUserData *userData2 = reinterpret_cast<ObjectUserData *>(bodyB->GetUserData());


	if (!userData1 || !userData2) {
		cout << "missing user data" << endl;
		return;
	}

	else if (userData1->objectType == "border" || userData2->objectType == "border")
		HandleBorderEndContact(userData1, userData2);
}

void MyContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold) {

}

void MyContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {

}

bool MyContactListener::BeginContactImmediate(b2Contact* contact, uint32 threadIndex) {
    // Call BeginContact for every contact.
    return true;
}
bool MyContactListener::EndContactImmediate(b2Contact* contact, uint32 threadIndex) {
    // Call EndContact for every contact.
    return true;
}
bool MyContactListener::PreSolveImmediate(b2Contact* contact, const b2Manifold* oldManifold, uint32 threadIndex) {
    // Never call PreSolve
    return false;
}
bool MyContactListener::PostSolveImmediate(b2Contact* contact, const b2ContactImpulse* impulse, uint32 threadIndex) {
    // Never call PostSolve
    return false;
}





void MyContactListener::HandleBorderBeginContact(b2Contact *contact) {
	weak_ptr<Object> obj;
	if (contact->GetFixtureA()->GetBody() == GameManager::worldBorder) {
		//uintptr_t ptr = contact->GetFixtureB()->GetBody()->GetUserData();
		ObjectUserData *userData = reinterpret_cast<ObjectUserData *>(contact->GetFixtureB()->GetBody()->GetUserData());
		if (userData)
			obj = userData->parentObject;
	}
	else if (contact->GetFixtureB()->GetBody() == GameManager::worldBorder) {
		//uintptr_t ptr = contact->GetFixtureA()->GetBody()->GetUserData();
		ObjectUserData *userData = reinterpret_cast<ObjectUserData *>(contact->GetFixtureA()->GetBody()->GetUserData());
		if (userData)
			obj = userData->parentObject;
	}

	if (!obj.expired()) {
		for (int i = GameManager::objectsOutsideBorder.size() - 1; i >= 0; i--) {
			if (GameManager::objectsOutsideBorder[i].lock() == obj.lock())
				GameManager::objectsOutsideBorder.erase((GameManager::objectsOutsideBorder.begin() + i));
		}
	}
}

void MyContactListener::HandleBorderEndContact(ObjectUserData *userData1, ObjectUserData *userData2) {
	ObjectUserData *userData;

	//cout << "border end collision" << endl;
	if (userData1->objectType == "border")
		userData = userData2;
	else if (userData2->objectType == "border")
		userData = userData1;

	if (userData)
		GameManager::objectsOutsideBorder.push_back(userData->parentObject);
	else
		cout << "No user data!" << endl;
}


