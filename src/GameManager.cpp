#include "GameManager.h"

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>

#include <box2d/b2_body.h>
#include <box2d/b2_circle_shape.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_math.h>

#include <box2d/b2_world.h>
#include <cstdint>
#include <iostream>
#include <fmt/format.h>
#include <memory>

#include "Creature/BodyPart.h"
#include "Globals.h"
#include "UserInput.h"
#include "Util.h"
#include "Camera.h"
#include "ObjectUserData.h"

#include "UI/Toolbar.h"

using namespace std;

class MyContactListener : public b2ContactListener {
	public:
		void BeginContact(b2Contact* contact) {

			if (contact->GetFixtureA()->GetBody() == GameManager::worldBorder || contact->GetFixtureB()->GetBody() == GameManager::worldBorder) {
				weak_ptr<Object> obj;
				if (contact->GetFixtureA()->GetBody() == GameManager::worldBorder) {
					uintptr_t ptr = contact->GetFixtureB()->GetBody()->GetUserData().pointer;
					ObjectUserData *userData = reinterpret_cast<ObjectUserData *>(ptr);
					if (userData)
						obj = userData->parentObject;
				}
				else if (contact->GetFixtureB()->GetBody() == GameManager::worldBorder) {
					uintptr_t ptr = contact->GetFixtureA()->GetBody()->GetUserData().pointer;
					ObjectUserData *userData = reinterpret_cast<ObjectUserData *>(ptr);
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
			else {
				ObjectUserData *userData1 = reinterpret_cast<ObjectUserData *>(contact->GetFixtureA()->GetBody()->GetUserData().pointer);
				ObjectUserData *userData2 = reinterpret_cast<ObjectUserData *>(contact->GetFixtureB()->GetBody()->GetUserData().pointer);

				ObjectUserData *mouseData = nullptr;
				ObjectUserData *otherObj = nullptr;

				if (userData1 && userData1->objectType == "mouse") {
					mouseData = userData1;
					otherObj = userData2;
				}
				else if (userData2 && userData2->objectType == "mouse") {
					mouseData = userData2;
					otherObj = userData1;
				}

				if (mouseData && otherObj) {
					cout << "Mouse hover begin! " << otherObj->objectType << endl;
					UserInput::SetHoveredObject(otherObj->parentObject);
				}

			}

		}

		void EndContact(b2Contact* contact) {
			b2Body *bodyA = contact->GetFixtureA()->GetBody();
			b2Body *bodyB = contact->GetFixtureB()->GetBody();
			ObjectUserData *userDataA = reinterpret_cast<ObjectUserData *>(bodyA->GetUserData().pointer);
			ObjectUserData *userDataB = reinterpret_cast<ObjectUserData *>(bodyB->GetUserData().pointer);
			ObjectUserData *userData;

			if (!userDataA || !userDataB) {
				cout << "missing user data" << endl;
				return;
			}

			if (userDataA->objectType == "mouse" || userDataB->objectType == "mouse")
				UserInput::ClearHoveredObject();

			if (userDataA->objectType == "border" || userDataB->objectType == "border") {
				cout << "border end collision" << endl;
				if (userDataA->objectType == "border")
					userData = userDataB;
				else if (userDataB->objectType == "border")
					userData = userDataA;

				if (userData)
					GameManager::objectsOutsideBorder.push_back(userData->parentObject);
				else
					cout << "No user data!" << endl;
			}
		}

		void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) {

		}

		void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {

		}
};


namespace GameManager {
	int speed;
	bool paused;
	double simStartTime;
	double simTicks;
	int32 velocityIterations;
	int32 positionIterations;

	ALLEGRO_DISPLAY *display;
	ALLEGRO_EVENT_QUEUE *event_queue;
	ALLEGRO_TIMER *timer;

	b2Vec2 gravity(0.0, 0.0);
	b2World world(gravity);
	b2Body *worldBorder;

	vector<shared_ptr<Creature>> agents;
	vector<shared_ptr<Object>> looseObjects;
	vector<weak_ptr<Object>> objectsOutsideBorder;

	void Init() {
		InitAllegro();

		velocityIterations = 6;
		positionIterations = 2;
		speed = 1;
		paused = false;

		MyContactListener *contactListener = new MyContactListener;

		world.SetContactListener(contactListener);

		worldBorder = CreateWorldBorder();

		simTicks = 0;
		simStartTime = al_get_time();
	}

	void InitAllegro() {
		al_init();
		al_install_keyboard();
		al_install_mouse();
		al_init_font_addon();
		al_init_ttf_addon();
		al_init_primitives_addon();
		al_init_native_dialog_addon();

		al_set_new_display_flags(ALLEGRO_GTK_TOPLEVEL);
		display = al_create_display(Globals::SCREEN_WIDTH, Globals::SCREEN_HEIGHT);
		event_queue = al_create_event_queue();
		timer = al_create_timer(Globals::FPS);

		al_register_event_source(event_queue, al_get_timer_event_source(timer));
		al_register_event_source(event_queue, al_get_display_event_source(display));
		al_register_event_source(event_queue, al_get_keyboard_event_source());
		al_register_event_source(event_queue, al_get_mouse_event_source());
		al_register_event_source(event_queue, al_get_default_menu_event_source());
	}

	void Shutdown() {
		al_destroy_display(display);
		al_destroy_event_queue(event_queue);
		al_destroy_timer(timer);
		al_uninstall_mouse();
		al_uninstall_keyboard();
	}

	b2Body *CreateWorldBorder() {
		ObjectUserData *objectUserData = new ObjectUserData();
		objectUserData->objectType = "border";

		b2BodyDef bodyDef;
		bodyDef.position = b2Vec2(0, 0);
		bodyDef.type = b2_staticBody;
		bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(objectUserData);

		b2Body *body = world.CreateBody(&bodyDef);

		b2CircleShape shapeDef;
		shapeDef.m_radius = Util::pixelsToMeters(Globals::WORLD_SIZE_PX);

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &shapeDef;
		fixtureDef.isSensor = true;
		fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(objectUserData);

		body->CreateFixture(&fixtureDef);

		return body;
	}

	void Update() {
		if (paused)
			return;

		for (int i = 0; i < speed; i++) {
			GameManager::world.Step(Globals::FPS, velocityIterations, positionIterations);

			for (int i = agents.size() - 1; i >= 0; i--) {
				agents[i]->Update();

				if (!agents[i]->IsAlive()) {
					agents[i]->DestroyAllJoints();

					vector<shared_ptr<BodyPart>> agentParts = agents[i]->GetAllParts();
					looseObjects.insert(looseObjects.end(), agentParts.begin(), agentParts.end());

					agents.erase(agents.begin() + i);

				}
			}

			for (auto object : looseObjects)
				object->Update();


			for (int i = objectsOutsideBorder.size() - 1; i >= 0; i--) {
				if (shared_ptr<Object> object = objectsOutsideBorder[i].lock()) {
					b2Vec2 pos = object->GetPos();
					float diff = pos.Normalize() - Util::pixelsToMeters(Globals::WORLD_SIZE_PX);
					object->ApplyForce(-diff * pos);
				}
				else {
					objectsOutsideBorder.erase(objectsOutsideBorder.begin() + i);
					continue;
				}
			}

			simTicks++;
		}
	}

	void Draw() {
		for (auto object : looseObjects)
			object->Draw();

		for (auto agent : agents)
			agent->Draw();

		bool drawWorldBorder = true;

		if (drawWorldBorder) {
			b2Vec2 origin(0, 0);

			ALLEGRO_TRANSFORM t;
			al_identity_transform(&t);
			al_translate_transform(&t, origin.x, origin.y);
			al_compose_transform(&t, &Camera::transform);

			al_use_transform(&t);

			al_draw_circle(0, 0, Globals::WORLD_SIZE_PX, al_map_rgb(255, 255, 255), 2);
		}
	}

	shared_ptr<Creature> CreateAgent(string genes, b2Vec2 pos) {
		shared_ptr<Creature> creature = make_shared<Creature>(Creature(genes, pos));
		creature->Init();
		agents.push_back(creature);
		cout << agents.size() << endl;
		return creature;
	}

	void ClearAgents() {
		agents.clear();
	}

	void AddObject(shared_ptr<Object> newObject) {
		looseObjects.push_back(newObject);
	}


	double GetSimTime() {
		return al_get_time() - simStartTime;
	}

	string GetSimTimeStr() {
		double simTime = GetSimTime();
		string simTimeLabel = "sec";

		if (simTime > 3600) {
			simTime /= 3600;
			simTimeLabel = "hr";
		}
		else if (simTime > 60) {
			simTime /= 60;
			simTimeLabel = "min";
		}

		return fmt::format("{:.2f} {}", simTime, simTimeLabel);
	}

	string GetSimTicksStr() {
		double simTime = simTicks / 30;
		string simTimeLabel = "sec";

		if (simTime > 3600) {
			simTime /= 3600;
			simTimeLabel = "hr";
		}
		else if (simTime > 60) {
			simTime /= 60;
			simTimeLabel = "min";
		}

		return fmt::format("{:.2f} {}", simTime, simTimeLabel);
	}


	void ResetSpeed() {
		speed = 1;
		Toolbar::UpdateSpeedDisplay();
	}

	void IncreaseSpeed() {
		speed++;
		Toolbar::UpdateSpeedDisplay();
	}

	void DecreaseSpeed() {
		if (speed > 1)
			speed--;
		Toolbar::UpdateSpeedDisplay();
	}

	int GetSpeed() {
		return speed;
	}


	void TogglePaused() {
		paused = !paused;
	}

	bool IsPaused() {
		return paused;
	}

}