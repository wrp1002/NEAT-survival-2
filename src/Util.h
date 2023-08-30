#pragma once

#include <box2d/box2d.h>

namespace Util {
    b2Vec2 metersToPixels(float xMeters, float yMeters);
    b2Vec2 metersToPixels(b2Vec2 pos);
    float metersToPixels(float val);
    b2Vec2 pixelsToMeters(float xPixels, float yPixels);
    b2Vec2 pixelsToMeters(b2Vec2 pos);

    float pixelsToMeters(float val);

    float mapVal(float input, float input_start, float input_end, float output_start, float output_end);

    float DegreesToRadians(int degrees);
    int RadiansToDegrees(float radians);
}