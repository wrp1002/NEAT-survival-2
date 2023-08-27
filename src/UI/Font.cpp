#include "Font.h"

#include "../Globals.h"
#include <allegro5/allegro_font.h>
#include <allegro5/color.h>

namespace Font {
    unordered_map<string, ALLEGRO_FONT*> fonts;

    ALLEGRO_FONT *GetFont(string fontName, int fontSize) {
        string key = fontName + to_string(fontSize);
        if (fonts[key] == nullptr) {
            string fontPath = Globals::resourcesDir + fontName;
            ALLEGRO_FONT *font = al_load_font(fontPath.c_str(), fontSize, 0);
            fonts[key] = font ? font : al_create_builtin_font();
        }
        return fonts[key];
    }


    void DrawText(string fontName, int fontSize, string text, int x, int y) {
        al_draw_textf(GetFont(fontName, fontSize), al_map_rgb(255, 255, 255), x, y, 0, "%s", text.c_str());
    }
}
