#pragma once

#include "../Objects/UserData/ObjectUserData.h"
#include <Box2D/Box2D.h>

class ObjectUserData;


class MyContactListener : public b2ContactListener {
	public:
		void BeginContact(b2Contact* contact) override;
		void EndContact(b2Contact* contact) override;
		void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override;
		void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;

		bool BeginContactImmediate(b2Contact* contact, uint32 threadIndex) override;
		bool EndContactImmediate(b2Contact* contact, uint32 threadIndex) override;
		bool PreSolveImmediate(b2Contact* contact, const b2Manifold* oldManifold, uint32 threadIndex) override;
		bool PostSolveImmediate(b2Contact* contact, const b2ContactImpulse* impulse, uint32 threadIndex) override;


		void HandleBorderBeginContact(b2Contact *contact);
		void HandleBorderEndContact(ObjectUserData *userData1, ObjectUserData *userData2);

		void HandleMouseBeginContact(ObjectUserData *userData1, ObjectUserData *userData2);

		void HandleMouthBeginContact(ObjectUserData *userData1, ObjectUserData *userData2);
		void HandleMouthEndContact(ObjectUserData *userData1, ObjectUserData *userData2);
};