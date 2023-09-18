#pragma once

#include "ObjectUserData.h"
#include <box2d/b2_contact.h>
#include <box2d/b2_world_callbacks.h>

class ObjectUserData;


class MyContactListener : public b2ContactListener {
	public:
		void BeginContact(b2Contact* contact);
		void EndContact(b2Contact* contact);
		void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
		void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);

		void HandleBorderBeginContact(b2Contact *contact);
		void HandleBorderEndContact(ObjectUserData *userData1, ObjectUserData *userData2);
		void HandleMouseBeginContact(ObjectUserData *userData1, ObjectUserData *userData2);
};