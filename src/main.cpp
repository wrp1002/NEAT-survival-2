#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/color.h>
#include <allegro5/display.h>
#include <allegro5/drawing.h>
#include <allegro5/events.h>
#include <allegro5/keyboard.h>
#include <allegro5/keycodes.h>
#include <allegro5/mouse.h>
#include <allegro5/timer.h>
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
#include <functional>
#include <iostream>
#include <vector>
#include <map>
#include <memory>


#include "Camera.h"
#include "Globals.h"
#include "UserInput.h"
#include "Util.h"
#include "UI/Font.h"
#include "Creature/Creature.h"

#include "RectObject.h"

using namespace std;

const int boxSize = 25;
const int groundWidth = 600;



int main() {
    srand(time(0));

    int32 velocityIterations = 6;
    int32 positionIterations = 2;

    bool done = false;
    bool redraw = true;
    float count;
    map<int, bool> keys;


    Camera::Init();
    UserInput::Init();

    // ============= allegro init =============
    al_init();
    al_install_keyboard();
    al_install_mouse();
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_primitives_addon();

    ALLEGRO_DISPLAY *display = al_create_display(Globals::SCREEN_WIDTH, Globals::SCREEN_HEIGHT);
    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
    ALLEGRO_TIMER *timer = al_create_timer(Globals::FPS);

    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_mouse_event_source());



    // ============= end allegro =============


    // tmp stuff
    b2Vec2 gravity(0.0f, 10.0f);
    b2World world(gravity);

    b2BodyDef groundBodyDef;
    groundBodyDef.position = Util::pixelsToMeters(Globals::SCREEN_WIDTH / 2.0, Globals::SCREEN_HEIGHT - 50);
    b2Body* groundBody = world.CreateBody(&groundBodyDef);
    b2PolygonShape groundBox;
    groundBox.SetAsBox(groundWidth / Globals::scaling, 10.0f / Globals::scaling);
    groundBody->CreateFixture(&groundBox, 0.0f);


    string genes = "";

    // first 3 are instruction type
    // 100==shape, 0==rect, 1==circle, 2==?, 3 digits for w&h
    genes += "1001100100000000";
    genes += "1255100200000000";
    genes += "0000000000000000";
    genes += "0000000000000000";


    shared_ptr<Creature> creature = make_shared<Creature>(Creature(genes));
    creature->Init(world);

    //shared_ptr<RectObject> obj = make_shared<RectObject>(RectObject(world, b2Vec2(Globals::SCREEN_WIDTH / 2, Globals::SCREEN_HEIGHT / 2), b2Vec2(50, 75), 0));
    //shared_ptr<RectObject> obj2 = make_shared<RectObject>(RectObject(world, obj, M_PI + 1, b2Vec2(75, 10), M_PI_4 + M_PI));


    al_start_timer(timer);

    while (!done) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            done = true;
        else if (ev.type == ALLEGRO_EVENT_TIMER) {
            redraw = true;

            /*
            if (keys[ALLEGRO_KEY_UP])
                head->body->ApplyForce(b2Vec2(0, -100), head->body->GetWorldCenter(), true);

            head->body->ApplyForce(b2Vec2(50 * (keys[ALLEGRO_KEY_RIGHT] - keys[ALLEGRO_KEY_LEFT]), 0), head->body->GetWorldCenter(), true);
            */

            world.Step(Globals::FPS, velocityIterations, positionIterations);
        }
        else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            keys[ev.keyboard.keycode] = true;

            if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                done = true;
        }
        else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
            keys[ev.keyboard.keycode] = false;
        }

        else if (ev.type == ALLEGRO_EVENT_MOUSE_AXES) {
            b2Vec2 mousePos(ev.mouse.x, ev.mouse.y);
			if (ev.mouse.display == display) {
				UserInput::SetMousePos(mousePos);
				int wheelDiff = ev.mouse.z - UserInput::mouseWheel;
				Camera::UpdateZoom(wheelDiff);
				UserInput::mouseWheel = ev.mouse.z;
			}
            /*
			else if (ev.mouse.display == InfoDisplay::display) {
				InfoDisplay::mousePos = mousePos;
			}
            */
        }
        else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            UserInput::StartDragging(b2Vec2(ev.mouse.x, ev.mouse.y));
        }
        else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
            if (ev.mouse.display == display) {
				if (ev.mouse.button == 2) {
					Camera::pos = Camera::CalculatePos();
					UserInput::StopDragging();
				}
			}
        }



        if (redraw && al_is_event_queue_empty(event_queue)) {
            redraw = false;

            Camera::UpdateTransform();

            al_set_target_backbuffer(display);
            al_clear_to_color(al_map_rgb(0,0,0));

            //al_draw_filled_circle(100, 100, 50, al_map_rgb(255, 0, 255));

            b2Vec2 groundPos = Util::metersToPixels(groundBody->GetPosition().x, groundBody->GetPosition().y);
            al_draw_filled_rectangle(Globals::SCREEN_WIDTH / 2 - groundWidth, Globals::SCREEN_HEIGHT - 50 - 10, Globals::SCREEN_WIDTH / 2 + groundWidth, Globals::SCREEN_HEIGHT - 50 + 10, al_map_rgb(100, 100, 100));

            creature.get()->Draw();
            //obj->Draw();
            //obj2->Draw();


            ALLEGRO_TRANSFORM identityTransform;
            al_identity_transform(&identityTransform);
            Font::DrawText("arial.ttf", 16, "string text", 10, 10);

            al_flip_display();
        }


    }


}