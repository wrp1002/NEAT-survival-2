#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/color.h>
#include <allegro5/display.h>
#include <allegro5/drawing.h>
#include <allegro5/events.h>
#include <allegro5/keyboard.h>
#include <allegro5/keycodes.h>
#include <allegro5/timer.h>
#include <allegro5/transformations.h>

#include <box2d/b2_block_allocator.h>
#include <box2d/b2_body.h>
#include <box2d/b2_circle_shape.h>
#include <box2d/b2_common.h>
#include <box2d/b2_joint.h>
#include <box2d/b2_math.h>
#include <box2d/b2_revolute_joint.h>
#include <box2d/b2_weld_joint.h>
#include <box2d/b2_world.h>
#include <box2d/box2d.h>
#include <box2d/b2_polygon_shape.h>

#include <functional>
#include <iostream>
#include <vector>
#include <map>
#include <memory>

#include "Creature/Creature.h"


using namespace std;

const int boxSize = 25;
const int groundWidth = 600;

vector<b2RevoluteJoint*> joints;





void DrawCircle(b2Vec2 pos, float angle, float r, ALLEGRO_COLOR color) {
    ALLEGRO_TRANSFORM t;
    al_identity_transform(&t);

    al_rotate_transform(&t, angle);
    al_translate_transform(&t, pos.x, pos.y);
    al_use_transform(&t);

    al_draw_filled_circle(0, 0, r, color);

    al_identity_transform(&t);
    al_use_transform(&t);
}


void DrawRect(b2Vec2 pos, float angle, float width, float height, ALLEGRO_COLOR color) {
    ALLEGRO_TRANSFORM t;
    al_identity_transform(&t);

    al_rotate_transform(&t, angle);
    al_translate_transform(&t, pos.x, pos.y);
    al_use_transform(&t);

    al_draw_filled_rectangle(-width, -height, width, height, color);
    al_draw_filled_circle(0, 0, 10, al_map_rgb(255, 0, 255));

    al_identity_transform(&t);
    al_use_transform(&t);
}

b2RevoluteJoint *CreateJoint(b2World &world, b2Body *body1, b2Body *body2, b2Vec2 pos) {
    b2RevoluteJointDef jointDef;

    b2Vec2 pos1 = body1->GetPosition();
    b2Vec2 pos2 = body2->GetPosition();

    jointDef.Initialize(body1, body2, pos);
    jointDef.lowerAngle = -0.5f * b2_pi; // -90 degrees
    jointDef.upperAngle = 0.25f * b2_pi; // 45 degrees
    jointDef.enableLimit = false;
    jointDef.maxMotorTorque = 2.0f;
    jointDef.motorSpeed = 0.0f;
    jointDef.enableMotor = true;
    jointDef.collideConnected = false;

    b2RevoluteJoint *joint = (b2RevoluteJoint *)world.CreateJoint(&jointDef);
    return joint;
}


class Box {
    private:

    public:
        b2Body *body;
        int width, height;

        Box(b2World &world, b2Vec2 pos, int width, int height) {
            this->width = width;
            this->height = height;

            b2BodyDef bodyDef;
            bodyDef.type = b2_dynamicBody;
            bodyDef.position = pos;
            body = world.CreateBody(&bodyDef);

            b2PolygonShape dynamicBox;
            dynamicBox.SetAsBox(width / scaling, height / scaling);
            b2CircleShape s;

            //b2CircleShape circle;
            //circle.m_radius = 1;

            b2FixtureDef fixtureDef;
            fixtureDef.shape = &dynamicBox;
            fixtureDef.density = 1.0f;
            fixtureDef.friction = 0.3f;
            fixtureDef.restitution = 0.5f;

            body->CreateFixture(&fixtureDef);

            //body->ApplyTorque(rand() % 10 * 100, true);
        }

        b2Body *getBody() {
            return body;
        }
};








float decimalFromSubstring(string str, int wholeDigits, int decimalDigits) {
    assert(str.size() >= wholeDigits + decimalDigits);
    str = str.substr(0, wholeDigits + decimalDigits);
    str.insert(wholeDigits, ".");
    return stof(str);
}



int main() {
    srand(time(0));
    cout << decimalFromSubstring("201232465", 3, 3) << endl;

    int32 velocityIterations = 6;
    int32 positionIterations = 2;

    bool done = false;
    bool redraw = true;
    map<int, bool> keys;

    vector<Box> boxes;


    b2Vec2 gravity(0.0f, 10.0f);
    b2World world(gravity);

    b2BodyDef groundBodyDef;
    groundBodyDef.position = pixelsToMeters(SCREEN_WIDTH / 2.0, SCREEN_HEIGHT - 50);
    b2Body* groundBody = world.CreateBody(&groundBodyDef);
    b2PolygonShape groundBox;
    groundBox.SetAsBox(groundWidth / scaling, 10.0f / scaling);
    groundBody->CreateFixture(&groundBox, 0.0f);


    string genes = "";
    int geneLength = 16;

    // first 3 are instruction type
    // 100==shape, 0==rect, 1==circle, 2==?, 3 digits for w&h
    genes += "1001100100000000";
    genes += "1255100200000000";
    genes += "0000000000000000";
    genes += "0000000000000000";

    int section = 0;
    string colorGene = genes.substr(section * geneLength, section * geneLength + geneLength);
    cout << colorGene << endl;
    int first = colorGene[0];
    int r = decimalFromSubstring(colorGene.substr(1, 3), 3, 0);
    int g = decimalFromSubstring(colorGene.substr(4, 3), 3, 0);
    int b = decimalFromSubstring(colorGene.substr(7, 3), 3, 0);
    cout << r << " " << g << " " << b << endl;

    /*

    */


    // ============= allegro init =============
    al_init();
    al_install_keyboard();
    al_init_primitives_addon();

    ALLEGRO_DISPLAY *display = al_create_display(SCREEN_WIDTH, SCREEN_HEIGHT);
    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
    ALLEGRO_TIMER *timer = al_create_timer(FPS);

    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_keyboard_event_source());

    al_start_timer(timer);

    // ============= end allegro =============

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

            world.Step(FPS, velocityIterations, positionIterations);
        }
        else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            keys[ev.keyboard.keycode] = true;
        }
        else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
            keys[ev.keyboard.keycode] = false;
        }



        if (redraw && al_is_event_queue_empty(event_queue)) {
            redraw = false;

            al_clear_to_color(al_map_rgb(0,0,0));

            b2Vec2 groundPos = metersToPixels(groundBody->GetPosition().x, groundBody->GetPosition().y);
            DrawRect(groundPos, 0, groundWidth, 10, al_map_rgb(100, 100, 100));

            for (auto box : boxes) {
                DrawRect(metersToPixels(box.getBody()->GetPosition().x, box.getBody()->GetPosition().y), box.getBody()->GetAngle(), box.width, box.height, al_map_rgb(255, 100, 100));
            }

            //head->Draw();

            for (auto joint : joints) {
                b2Vec2 pos;
                pos = metersToPixels(joint->GetAnchorA());
                al_draw_filled_circle(pos.x, pos.y, 2, al_map_rgb(0, 255, 0));

                pos = metersToPixels(joint->GetAnchorB());
                al_draw_filled_circle(pos.x, pos.y, 2, al_map_rgb(0, 255, 0));
            }



            al_flip_display();
        }


    }


}