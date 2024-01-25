#include "GameManager.h"

#include <Box2D/Common/b2Math.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>

#include <Box2D/Box2D.h>

#include <cstdint>
#include <iostream>
#include <fmt/format.h>
#include <memory>

#include "SimStats.h"
#include "Objects/Creature/BodyPart.h"
#include "Globals.h"
#include "Util.h"
#include "UI/Camera.h"
#include "Objects/UserData/ObjectUserData.h"
#include "Objects/Egg.h"
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
	b2ThreadPoolTaskExecutor executor;

	vector<shared_ptr<Creature>> agents;
	vector<shared_ptr<Egg>> eggs;
	vector<shared_ptr<Object>> looseObjects;
	vector<weak_ptr<Object>> objectsOutsideBorder;

	double extraEnergy;


	void Init() {
		InitAllegro();

		velocityIterations = 6;
		positionIterations = 2;
		speed = 1;
		paused = false;
		extraEnergy = 0;

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

	void Reset() {
		cout << "Resetting sim..." << endl;
		simTicks = 0;
		simStartTime = al_get_time();
		SimStats::Init();

		for (int i = agents.size() - 1; i >= 0; i--) {
			agents[i]->DestroyAllJoints();
			vector<shared_ptr<BodyPart>> agentParts = agents[i]->GetAllParts();
			looseObjects.insert(looseObjects.end(), agentParts.begin(), agentParts.end());
			agents.erase(agents.begin() + i);
		}

		for (int i = looseObjects.size() - 1; i >= 0; i--) {
			looseObjects[i]->Destroy();
		}
		looseObjects.clear();

		for (int i = eggs.size() - 1; i >= 0; i--) {
			eggs[i]->Destroy();
		}
		eggs.clear();

		objectsOutsideBorder.clear();


		// Create new eggs
		for (int i = 0; i < 400; i++) ObjectFactory::CreateEgg();
	}

	b2Body *CreateWorldBorder() {
		ObjectUserData *objectUserData = new ObjectUserData();
		objectUserData->objectType = "border";

		b2BodyDef bodyDef;
		bodyDef.position = b2Vec2(0, 0);
		bodyDef.type = b2_staticBody;
		//bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(objectUserData);
		bodyDef.userData = (void*)objectUserData;

		b2Body *body = world.CreateBody(&bodyDef);

		b2CircleShape shapeDef;
		shapeDef.m_radius = Util::pixelsToMeters(Globals::WORLD_SIZE_PX);

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &shapeDef;
		fixtureDef.isSensor = true;
		//fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(objectUserData);
		bodyDef.userData = (void*)objectUserData;

		body->CreateFixture(&fixtureDef);

		return body;
	}

	void Update() {
		if (paused)
			return;

		for (int i = 0; i < speed; i++) {
			SimStats::StartPhysicsTimer();
			GameManager::world.Step(Globals::FPS, velocityIterations, positionIterations, GameManager::executor);
			SimStats::EndPhysicsTimer();

			SimStats::StartUpdateTimer();

			for (int i = agents.size() - 1; i >= 0; i--) {
				agents[i]->Update();

				if (!agents[i]->IsAlive()) {
					extraEnergy += agents[i]->GetTotalEnergy();

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

				double eggEnergy = 5;
				if (extraEnergy > eggEnergy && rand() % 10 == 0) {
					egg->AddEnergy(eggEnergy);
					extraEnergy -= eggEnergy;
				}

				if (!egg->IsAlive()) {
					egg->Destroy();
					eggs.erase(eggs.begin() + i);
					continue;
				}

				if (egg->ShouldHatch()) {
					ObjectFactory::CreateAgent(egg->GetGenes(), Util::metersToPixels(egg->GetPos()), egg->GetNN(), egg->GetEnergy());
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


			double foodValue = 100.0;
			if (rand() % 2 == 0 && extraEnergy > foodValue) {
				AddFood(foodValue);
				extraEnergy -= foodValue;
			}

			SimStats::EndUpdateTimer();

			simTicks++;
		}

		if (agents.size() + eggs.size() == 0)
			Reset();
	}

	void Draw() {
		SimStats::ResetDrawnObjectCount();

		for (auto object : looseObjects) {
			if (Camera::ShouldDrawObject(object->GetPosPX())) {
				object->Draw();
				SimStats::IncrementDrawnObjectCount();
			}
		}

		for (auto agent : agents) {
			b2Vec2 pos = agent->GetHeadPosPX();
			if (Camera::ShouldDrawObject(pos)) {
				agent->Draw();
				SimStats::IncrementDrawnObjectCount();
			}
		}

		for (auto egg : eggs) {
			if (Camera::ShouldDrawObject(egg->GetPosPX())) {
				egg->Draw();
				SimStats::IncrementDrawnObjectCount();
			}
		}

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
		//cout << agents.size() << endl;
		return creature;
	}

	void ClearAgents() {
		agents.clear();
	}

	void AddObject(shared_ptr<Object> newObject) {
		looseObjects.push_back(newObject);
	}

	void AddFood(double value) {
		ObjectFactory::CreateFood(value);
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

	unsigned int GetTotalEnergy() {
		int total = 0;

		total += extraEnergy;

		for (auto agent : agents)
			total += agent->GetTotalEnergy();

		for (auto egg : eggs) {
			total += egg->GetEnergy();
		}

		for (auto obj : looseObjects) {
			if (shared_ptr<BodyPart> bodyPart = dynamic_pointer_cast<BodyPart>(obj)) {
				total += bodyPart->GetHealth();
			}
		}

		return total;
	}

}