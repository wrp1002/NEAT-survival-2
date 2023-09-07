#include "GameManager.h"

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>

#include <box2d/b2_math.h>

#include <iostream>

#include "Globals.h"

using namespace std;

namespace GameManager {
	ALLEGRO_DISPLAY *display;
	ALLEGRO_EVENT_QUEUE *event_queue;
	ALLEGRO_TIMER *timer;

	b2Vec2 gravity(0.0, 10.0);
	b2World world(gravity);
	vector<shared_ptr<Creature>> agents;

	void Init() {
		InitAllegro();
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
		for (auto agent: agents)
			agent.get()->Update();
	}

	void Draw() {
		for (auto agent: agents)
			agent.get()->Draw();
	}

	void CreateAgent(string genes, b2Vec2 pos) {
		shared_ptr<Creature> creature = make_shared<Creature>(Creature(genes, b2Vec2(Globals::SCREEN_WIDTH / 2.0, Globals::SCREEN_HEIGHT / 2.0)));
		creature->Init();
		agents.push_back(creature);
		cout << agents.size() << endl;
	}

	void ClearAgents() {
		agents.clear();
	}
}