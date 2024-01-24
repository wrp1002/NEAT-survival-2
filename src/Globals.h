#pragma once

#include <Box2D/Box2D.h>
#include <string>

using namespace std;

namespace Globals {
	const int SCREEN_WIDTH = 1600;
	const int SCREEN_HEIGHT = 1200;

	const int WORLD_SIZE_PX = 20000;

	const float scaling = 100; // some number. Probably something like 100 will do for Box2D.
	const float FPS = 1.0 / 30.0;

	const string resourcesDir = "Resources/";

	const int GENE_LENGTH = 16;
	const float GENE_MUTATE_CHANCE = 0.01;
	const float GENE_CREATE_CHANCE = 0.001;
	const float GENE_DELETE_CHANCE = 0.001;

}