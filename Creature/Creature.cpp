#include "Creature.h"

#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>

#include <box2d/b2_world.h>
#include <box2d/box2d.h>

#include "BodySegment.h"
#include "../Globals.h"

Creature::Creature(b2World &world, string genes) {
    BodySegment *head = new BodySegment(world, pixelsToMeters(Globals::SCREEN_WIDTH / 2, Globals::SCREEN_HEIGHT / 2), 50, al_map_rgb(r, g, b));
    BodySegment *prevPart = head;

    for (int i = 0; i < 10; i ++) {
        BodySegment *newPart = new BodySegment(world, *prevPart, -45, 0, 50, al_map_rgb(255, 0, 255));
        prevPart->AddChild(newPart);
        prevPart = newPart;
    }
}