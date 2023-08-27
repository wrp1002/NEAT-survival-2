#pragma once
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include <unordered_map>
#include <string>

using namespace std;

namespace Font {
	extern unordered_map<string, ALLEGRO_FONT*> fonts;

	ALLEGRO_FONT* GetFont(string fontName, int fontSize);
    void DrawText(string fontName, int fontSize, string text, int x, int y);
};

