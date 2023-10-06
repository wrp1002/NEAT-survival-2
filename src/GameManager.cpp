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
#include "Util.h"
#include "UI/Camera.h"
#include "ObjectUserData.h"
#include "NEAT/NEAT.h"
#include "Egg.h"
#include "ObjectFactory.h"

#include "Event_Listeners/ContactListener.h"
#include "Event_Listeners/JointDestructionListener.h"

#include "UI/Toolbar.h"

using namespace std;


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
	vector<shared_ptr<Egg>> eggs;
	vector<shared_ptr<Object>> looseObjects;
	vector<weak_ptr<Object>> objectsOutsideBorder;

	void Init() {
		InitAllegro();

		velocityIterations = 6;
		positionIterations = 2;
		speed = 1;
		paused = false;

		MyContactListener *contactListener = new MyContactListener;
		JointDestructionListener *jointDestructionListener = new JointDestructionListener;


		world.SetContactListener(contactListener);
		world.SetDestructionListener(jointDestructionListener);

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

			for (int i = looseObjects.size() - 1; i >= 0; i--) {
				auto object = looseObjects[i];

				object->Update();

				if (!object->IsAlive()) {
					object->Destroy();
					looseObjects.erase(looseObjects.begin() + i);
				}
			}

			for (int i = eggs.size() - 1; i >= 0; i--) {
				auto egg = eggs[i];

				egg->Update();

				if (!egg->IsAlive()) {
					egg->Destroy();
					eggs.erase(eggs.begin() + i);
					continue;
				}

				if (egg->ShouldHatch()) {
					ObjectFactory::CreateAgent(egg->GetGenes(), Util::metersToPixels(egg->GetPos()), egg->GetNN());
					egg->Destroy();
					eggs.erase(eggs.begin() + i);
					continue;
				}
			}


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

		for (auto egg : eggs)
			egg->Draw();

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