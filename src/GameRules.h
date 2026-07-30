#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <array>
#include <vector>

using namespace std;

namespace GameRules {
	enum RuleName {
		FOLLOW_RANDOM_AGENT,
		WASTE_DAMAGE,
		EYE_MOVEMENT,
		MOUTH_CONTROL,
		FORCE_MIN_POPULATION,

		COUNT
	};
	inline array<bool, COUNT> rules;

	bool IsInt(string input);

	void PrintPrompt(vector<string> keys);
	void GameRulePrompt();

	bool IsRuleEnabled(RuleName key);
	void SetRuleEnabled(RuleName key, bool value);
	void ToggleRule(RuleName key);
};

