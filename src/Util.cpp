#include "Util.h"

#include "Globals.h"

b2Vec2 Util::metersToPixels(float xMeters, float yMeters) {
    return b2Vec2(xMeters * Globals::scaling, yMeters * Globals::scaling);
}

b2Vec2 Util::metersToPixels(b2Vec2 pos) {
    return metersToPixels(pos.x, pos.y);
}

float Util::metersToPixels(float val) {
    return val * Globals::scaling;
}

b2Vec2 Util::pixelsToMeters(float xPixels, float yPixels) {
    return b2Vec2(xPixels / Globals::scaling, yPixels / Globals::scaling);
}

b2Vec2 Util::pixelsToMeters(b2Vec2 pos) {
    return pixelsToMeters(pos.x, pos.y);
}

float Util::pixelsToMeters(float val) {
    return val / Globals::scaling;
}

float Util::mapVal(float input, float input_start, float input_end, float output_start, float output_end) {
    float slope = (output_end - output_start) / (input_end - input_start);
    float output = output_start + slope * (input - input_start);
    return output;
}

float Util::DegreesToRadians(int degrees) {
    return degrees * M_PI / 180;
}

int Util::RadiansToDegrees(float radians) {
    return radians * 180 / M_PI;
}