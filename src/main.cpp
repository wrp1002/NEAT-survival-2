#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>

#include <allegro5/keycodes.h>
#include <allegro5/transformations.h>
#include <box2d/b2_block_allocator.h>
#include <box2d/b2_body.h>
#include <box2d/b2_circle_shape.h>
#include <box2d/b2_common.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_joint.h>
#include <box2d/b2_math.h>
#include <box2d/b2_revolute_joint.h>
#include <box2d/b2_weld_joint.h>
#include <box2d/b2_world.h>
#include <box2d/box2d.h>
#include <box2d/b2_polygon_shape.h>

#include <cmath>
#include <cstdint>
#include <functional>
#include <iostream>
#include <sys/types.h>
#include <vector>
#include <map>
#include <memory>
#include <string>


#include "GameManager.h"
#include "ObjectFactory.h"
#include "Objects/Object.h"
#include "UI/Camera.h"
#include "UserInput.h"
#include "Util.h"
#include "UI/Font.h"
#include "UI/Toolbar.h"
#include "UI/InfoDisplay.h"

using namespace std;

const int boxSize = 25;
const int groundWidth = 600;

string GetRandomNums(int amount) {
	string val = "";
	for (int i = 0; i < amount; i++)
		val += to_string(rand() % 10);
	return val;
}

int main() {
	int seed = 0;
	if (seed)
		srand(seed);
	else
		srand(time(0));

	bool done = false;
	bool redraw = true;
	float lastRedrawTime = 0;
	float maxRedrawTime = 2.0;
	float count;
	float nextTime = 5;


	GameManager::Init();
	Camera::Init();
	UserInput::Init();
	Toolbar::Init(GameManager::display);
	InfoDisplay::Init(GameManager::event_queue);
	ObjectFactory::Init();

	GameManager::Reset();


	string genes = "";
	// gene length == 16
	// first 3 are instruction type
	// 000==shape, 0==rect, 1==circle, 2==?, 3 digits for w&h, 3 digits for angle on parent & angle offset
	//genes += string() + "000" + "1" + "050" + "150" + "000" + "000";
	// 001==color, 3 digits for r, g, b |  4 unused
	//genes += string() + "001" + "255" + "000" + "255" + "0000";
	genes += string() + "000" + "1959950000000";	// shape
	genes += string() + "002" + "0000000000000";	// create head
	//genes += string() + "000" + "0200900000000";	// shape
	genes += string() + "003" + "3500000000000";	// angle
	genes += string() + "002" + "0000000000000";	// create

	genes += string() + "003" + "4300500000000";	// angle
	genes += string() + "002" + "0000000000000";	// create

	genes = "33270047261965839371800403010815334330276373056903205545858050585139156705001893134688365460630163290170692983317905754510576612132261432742294986781133382963409845980375604215883196636945485369179119891514013324199784424983010243332603006359868958302012343588133393993530636423485909155503516877186631604469939529434906";
	//genes = "16046997046216714658134496517170987696391034827212228987731668768449291514999730184992767833821765885966658741761350547442934122822528206270568794760405890224616980720547603472040110892846409018310365127694190121203609469493224250985654265570612193041280522960188643291041024244760980952440488265894190292718315505511957";
	// bubbles
	//genes = "03478659525045150460440913291052501387536035562528875886308423873033986281950122905609332306684209597437724736257317563793381922093751659225942805552023373795195492377502296498963377705276885556704556777536432789772209000757447015684368923311381521621808564586246692606138588631925007083451077757128054802868952774472049346132976071674070078267707101367973380109568998194925913845971890149381408247058492783289779170195245490434331100400918907115153870331575900320389672693606113901156316826850118296557017648050378101703601632763538232918934281003195657721119462587798584102424934459493514650800591572984437822294434505262465734881282725629878313985423699362861191365027026077868100799627126445593207121849705548617791704340819339273378240004887557827474475601740187921923601379291209676130403433146";
	// little guy
	//genes = "46049855229059940961270970232456052269004109223319652749965120857273294836758301279663652596571410050633210341680578175524173538532596049175435614421173682136699161995024587248792819998613294403512216684510600883823894135878149561145702762743047548162761598444779741117403738681592788037801570664785425918978425616541421879837325067880820612586350594874789512230215298279759586738127715788112355277206191360135969763433364799239951692948153779845100538100353446526924951228334145310433481724897006259340211613132628831664190852897337129610530801604962592697006731653036700902082180852213133927084459041731651019107540864456167722845972250361573048224861597481686773090256305949964372644682761648860185737321219866153518907041127157830565808717622693783417720455330308081172834205793056828062718860698706101521092697";

	//genes += string() + "00" + "2999999000000";	// angle
	//genes = "";
	//genes += string() + "02" + "6739657396839";	// create head

	//genes += string() + "02" + "9739657396839";	// create cilium
	//genes += string() + "02" + "9657384651297";	// create cilium

	//genes += string() + "003" + "2500000000000";	// angle
	//genes += string() + "000" + "1200500000000";	// shape
	//genes += string() + "002" + "0000000000000";	// create

	genes = "";
	genes += string() + "0039999990000000";			// garbage
	genes += string() + "0039999990000000";			// garbage
	genes += string() + "00" + "39999990000000";	// Set shape
	genes += string() + "02" + "30000000000000";	// Create head
	genes += string() + "03" + "59999990000000";	// Set angle to back angle
	genes += string() + "02" + "9" + GetRandomNums(13);	// Create cilium
	genes += string() + "04" + "00000000000000";	// Set parent ID to head
	genes += string() + "03" + "29999990000000";	// Set angle to back
	genes += string() + "02" + "9" + GetRandomNums(13);	// Create cilium
	genes += string() + "04" + "00000000000000";	// Set parent ID to head
	genes += string() + "03" + "30000000000000";	// Set angle to front
	genes += string() + "02" + "7" + GetRandomNums(13);	// Create eyes


	ObjectFactory::CreateEgg(genes, b2Vec2(0, 0), ObjectFactory::RandomNN(), 100);
	shared_ptr<Creature> playerCreature = GameManager::CreateAgent(genes, b2Vec2(0, 0));
	playerCreature->SetAsPlayer(true);

	//cout << genes << endl;

	//for (int i = 0; i < 50; i++) ObjectFactory::CreateEgg(genes, Util::RandomWorldPosPX(), ObjectFactory::RandomNN(), 100);

	al_start_timer(GameManager::timer);

	while (!done) {
		ALLEGRO_EVENT ev;
		al_wait_for_event(GameManager::event_queue, &ev);

		if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			if (ev.display.source == GameManager::display)
				done = true;
			else if (InfoDisplay::IsVisible() && InfoDisplay::display == ev.display.source) {
				InfoDisplay::Hide();
				Toolbar::SetMenuCaption(Toolbar::BUTTON_IDS::TOGGLE_INFO_DISPLAY, "Show Info Display");
			}
		}
		else if (ev.type == ALLEGRO_EVENT_MENU_CLICK) {
			Toolbar::HandleEvent(ev);
		}
		else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			UserInput::SetPressed(ev.keyboard.keycode, true);

			if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
				done = true;
			else if (ev.keyboard.keycode == ALLEGRO_KEY_I)
				InfoDisplay::Toggle();
			else if (ev.keyboard.keycode == ALLEGRO_KEY_SPACE)
				GameManager::TogglePaused();
		}
		else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
			UserInput::SetPressed(ev.keyboard.keycode, false);
		}

		else if (ev.type == ALLEGRO_EVENT_MOUSE_AXES) {
			b2Vec2 mousePos(ev.mouse.x, ev.mouse.y);
			if (ev.mouse.display == GameManager::display) {
				UserInput::SetMousePos(mousePos);

				int wheelDiff = ev.mouse.z - UserInput::mouseWheel;
				Camera::UpdateZoom(wheelDiff);
				UserInput::mouseWheel = ev.mouse.z;
			}
			else if (ev.mouse.display == InfoDisplay::display) {
				InfoDisplay::mousePos = mousePos;
			}
		}
		else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
			if (ev.mouse.button == 1)
				InfoDisplay::SelectObject(UserInput::hoveredObject);
			else if (ev.mouse.button == 2)
				UserInput::StartDragging(b2Vec2(ev.mouse.x, ev.mouse.y));
		}
		else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
			if (ev.mouse.display == GameManager::display) {
				if (ev.mouse.button == 2) {
					Camera::pos = Camera::CalculatePos();
					UserInput::StopDragging();
				}
			}
		}
		else if (ev.type == ALLEGRO_EVENT_TIMER) {
			redraw = true;

			/*
			if (keys[ALLEGRO_KEY_UP])
				head->body->ApplyForce(b2Vec2(0, -100), head->body->GetWorldCenter(), true);

			head->body->ApplyForce(b2Vec2(50 * (keys[ALLEGRO_KEY_RIGHT] - keys[ALLEGRO_KEY_LEFT]), 0), head->body->GetWorldCenter(), true);
			*/

			if (al_get_time() - lastRedrawTime > maxRedrawTime) {
				cout << "Last redraw was more than " << maxRedrawTime << " seconds ago" << endl;
				cout << "Clearing event queue and lowering speed to " << (GameManager::speed > 1 ? GameManager::speed - 1 : 1) << endl;

				if (GameManager::speed == 1) {
					GameManager::TogglePaused();
					cout << "MAJOR LAG DETECTED. paused sim" << endl;
				}

				GameManager::DecreaseSpeed();
				al_flush_event_queue(GameManager::event_queue);
			}
			else {
				GameManager::Update();

				/*
				if (Camera::followObject.expired() && GameRules::IsRuleEnabled("FollowRandomAgent")) {
					shared_ptr<Object> followObj = GameManager::GetRandomAgent();
					Camera::FollowObject(followObj);
					InfoDisplay::SelectObject(followObj);
				}
				*/
			}


			playerCreature->ApplyForce(b2Vec2(
				(UserInput::IsPressed(ALLEGRO_KEY_RIGHT) - UserInput::IsPressed(ALLEGRO_KEY_LEFT)) * 5,
				(UserInput::IsPressed(ALLEGRO_KEY_DOWN) - UserInput::IsPressed(ALLEGRO_KEY_UP)) * 5)
			);

			playerCreature->SetBiting(UserInput::IsPressed(ALLEGRO_KEY_B));

		}


		if (redraw && al_is_event_queue_empty(GameManager::event_queue)) {
			redraw = false;
			lastRedrawTime = al_get_time();

			InfoDisplay::Draw();

			Camera::UpdateTransform();

			al_set_target_backbuffer(GameManager::display);
			al_clear_to_color(al_map_rgb(0, 0, 0));

			GameManager::Draw();

			Util::ResetTransform();
			Font::DrawText("arial.ttf", 16, "string text", 10, 10);

			al_flip_display();
		}


	}

	InfoDisplay::Shutdown();
	GameManager::Shutdown();
}