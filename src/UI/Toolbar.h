#pragma once

#include <allegro5/allegro5.h>
#include <allegro5/allegro_native_dialog.h>

#include <memory>
#include <string>

using namespace std;

namespace Toolbar {
	extern ALLEGRO_MENU *menu;

	namespace BUTTON_IDS {
		enum IDS {
			EXIT,
			TOGGLE_INFO_DISPLAY,
			RESET_SIM,

			SEARCH_RANDOM,
			SEARCH_HIGHEST_KILLS,
			SEARCH_HIGHEST_DAMAGED_INPUTS,
			SEARCH_HIGHEST_AGE,
			SEARCH_HIGHEST_ENERGY,
			SEARCH_HIGHEST_HEALTH,
			SEARCH_LOWEST_KILLS,
			SEARCH_LOWEST_DAMAGED_INPUTS,
			SEARCH_LOWEST_AGE,
			SEARCH_LOWEST_ENERGY,
			SEARCH_LOWEST_HEALTH,

			FOLLOW_RANDOM_AGENT,
			FORCE_MIN_POPULATION,

			SPEED_DISPLAY,
			SPEED_INCREASE,
			SPEED_DECREASE,
			SPEED_RESET,
			SPEED_AUTO_INCREASE,

			PLAY_START,
			PLAY_DAMAGE_INPUT,
			PLAY_REPAIR_INPUT,
			PLAY_MUTATE_MENU,
			PLAY_MUTATE_RANDOM,
			PLAY_MUTATE_ADD_NODE,
			PLAY_MUTATE_REMOVE_NODE,
			PLAY_MUTATE_ADD_CONNECTION,

			SELECTED_OBJECT_DESTROY,
			SELECTED_OBJECT_MAKE_EGG,
		};
	}

	void Init(ALLEGRO_DISPLAY *display);
	void HandleEvent(ALLEGRO_EVENT ev);
	void SetMenuCaption(int id, string text);
	void UpdateSpeedDisplay();
	void SetCheckboxEnabled(BUTTON_IDS::IDS buttonID, bool enabled);
	void ToggleCheckbox(BUTTON_IDS::IDS buttonID);

	//template <class searchType>
	//void AgentSearch(bool highest, searchType(Agent::*funcPtr)(void));
}