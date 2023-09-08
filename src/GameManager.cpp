#include "GameManager.h"

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>

#include <box2d/b2_math.h>

#include <iostream>
#include <fmt/format.h>

#include "Globals.h"
#include "Util.h"
#include "Camera.h"

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
	vector<shared_ptr<Creature>> agents;

	void Init() {
		velocityIterations = 6;
		positionIterations = 2;
		speed = 1;
		paused = false;
		InitAllegro();

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

	void Update() {
		if (paused)
			return;

		GameManager::world.Step(Globals::FPS, velocityIterations, positionIterations);

		for (auto agent: agents)
			agent.get()->Update();

		simTicks++;
	}

	void Draw() {
		for (auto agent: agents) {
			agent.get()->Draw();
		}

		bool drawWorldBorder = true;

		if (drawWorldBorder) {
			b2Vec2 origin(Globals::SCREEN_WIDTH / 2.0, Globals::SCREEN_HEIGHT / 2.0);

			ALLEGRO_TRANSFORM t;
			al_identity_transform(&t);
			al_translate_transform(&t, origin.x, origin.y);
			al_compose_transform(&t, &Camera::transform);

			al_use_transform(&t);

			al_draw_circle(0, 0, Globals::WORLD_SIZE_PX, al_map_rgb(255, 255, 255), 2);
		}
	}

	void CreateAgent(string genes, b2Vec2 pos) {
		shared_ptr<Creature> creature = make_shared<Creature>(Creature(genes, pos));
		creature->Init();
		agents.push_back(creature);
		cout << agents.size() << endl;
	}

	void ClearAgents() {
		agents.clear();
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
	}

	void IncreaseSpeed() {
		speed++;
	}

	void DecreaseSpeed() {
		if (speed > 1)
			speed--;
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