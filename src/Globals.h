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

	const int MAX_OBJECT_SIZE = 50;

	const int GENE_LENGTH = 16;
	const int GENE_INSTRUCTION_TYPES = 5;
	const float GENE_MUTATE_CHANCE = 0.01;
	const float GENE_CREATE_CHANCE = 0.001;
	const float GENE_DELETE_CHANCE = 0.001;

	const struct MutationProfile {
		static constexpr float MAX_HEADER_MUTATION_RATE = 0.001f;
		static constexpr float MAX_PARAMETER_MUTATION = 0.02f;      // 1% per parameter
		static constexpr float MAX_PARAMETER_REWRITE = 0.01f;      // 1% per parameter
		static constexpr float MAX_DUPLICATION = 0.002f;            // 0.2% per instruction
		static constexpr float MAX_DELETION = 0.001f;
		static constexpr float MAX_MOVE = 0.001f;
		static constexpr float MAX_SWAP = 0.001f;
		static constexpr float MAX_RANDOM_INSERT = 0.0002f;
	} mutationProfile;

	const unsigned int STARTING_POPULATION = 400;
	const unsigned int POPULATION_MIN_SAFETY_NET = 50;
	const unsigned int STARTING_DNA_GENE_LENGTH = 50;
	const unsigned int HEADER_GENES_COUNT = 2;

}