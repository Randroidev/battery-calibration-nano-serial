#include "display_manager.h"
#include "config_manager.h"
#include "PicoGraphics.hpp"
#include "ST7789.hpp"
#include "ST7735.hpp"

// For simplicity, we'll assume the library provides classes like these
// with methods like init(), clear(), drawText(), etc.
static PicoGraphics* graphics = nullptr;
static ST7789 display_st7789;
static ST7735 display_st7735;

void display_init() {
    if (config_get()->display_type == ST7789_240x240) {
        // graphics = &display_st7789; // Something like this
    } else {
        // graphics = &display_st7735;
    }
    // graphics->init();
}

void display_clear() {
    // if (graphics) graphics->clear();
}

void display_draw_text(const std::string& text, int x, int y, uint16_t color, uint16_t bg_color) {
    // if (graphics) graphics->drawText(text, x, y, color, bg_color);
}

void display_update_data(const BatteryData& data) {
    // ... use graphics->drawText() to display data ...
}

void display_draw_menu(const std::vector<std::string>& items, int selected_index) {
    // ... use graphics->drawText() to draw the menu ...
}
