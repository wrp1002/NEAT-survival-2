#pragma once

#include <string>

using namespace std;

namespace Globals {
    const int SCREEN_WIDTH = 1600;
    const int SCREEN_HEIGHT = 1200;

    const float scaling = 100; // some number. Probably something like 100 will do for Box2D.
    const float FPS = 1.0 / 60.0;

    const string resourcesDir = "Resources/";

}