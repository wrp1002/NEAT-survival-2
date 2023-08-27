#pragma once

#include <box2d/box2d.h>

#include "Globals.h"

b2Vec2 metersToPixels(float xMeters, float yMeters) {
    return b2Vec2(xMeters * scaling, yMeters * scaling);
}

b2Vec2 metersToPixels(b2Vec2 pos) {
    return metersToPixels(pos.x, pos.y);
}

float metersToPixels(float val) {
    return val * scaling;
}

b2Vec2 pixelsToMeters(float xPixels, float yPixels) {
    return b2Vec2(xPixels / scaling, yPixels / scaling);
}

b2Vec2 pixelsToMeters(b2Vec2 pos) {
    return pixelsToMeters(pos.x, pos.y);
}

float pixelsToMeters(float val) {
    return val / scaling;
}

float mapVal(float input, float input_start, float input_end, float output_start, float output_end) {
    float slope = (output_end - output_start) / (input_end - input_start);
    float output = output_start + slope * (input - input_start);
    return output;
}