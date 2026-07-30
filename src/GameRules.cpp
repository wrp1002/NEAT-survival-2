#include "GameRules.h"

namespace GameRules {

	bool IsInt(string input) {
		try {
			stoi(input);
		}
		catch (exception e) {
			return false;
		}
		return true;
	}

	void PrintPrompt(vector<string> keys) {
		// for (unsigned i = 0; i < keys.size(); i++) {
		// 	string key = keys[i];
		// 	cout << i + 1 << ". " << keys[i] << ": " << (rules[key] ? "Enabled" : "Disabled") << endl;
		// }
		// cout << keys.size() + 1 << ". return" << endl;
	}

	void GameRulePrompt() {
		//SetFocus(GetConsoleWindow());

		// int input = -1;
		// string inputStr = "";

		// vector<string> keys;

		// for (auto val : rules) {
		// 	keys.push_back(val.first);
		// }


		// while (input != keys.size()) {
		// 	PrintPrompt(keys);

		// 	cin >> inputStr;
		// 	if (!IsInt(inputStr))
		// 		continue;
		// 	input = stoi(inputStr);
		// 	input--;

		// 	if (input < 0 || input >= keys.size())
		// 		continue;

		// 	string key = keys[input];
		// 	rules[key] = !rules[key];
		// }
	}

	bool IsRuleEnabled(RuleName key) {
		return rules[key];
	}

	void SetRuleEnabled(RuleName key, bool value) {
		rules[key] = value;
	}

	void ToggleRule(RuleName key) {
		rules[key] = !rules[key];
	}
}