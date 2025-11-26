#include "display.h"
#include "pico_graphics.hpp"
#include "drivers/st7735/st7735.hpp"
#include "common/pimoroni_common.hpp"

using namespace pimoroni;

SPIPins pins = {
    .spi = spi0,
    .cs = 17,
    .sck = 14,
    .mosi = 19,
    .miso = PIN_UNUSED,
    .dc = 15,
    .bl = 18
};
ST7735 st7735(160, 128, pins);
PicoGraphics_PenRGB332 graphics(160, 128, nullptr);

void display_init() {
    st7735.set_backlight(255);
}

void display_clear() {
    graphics.set_pen(0, 0, 0);
    graphics.clear();
}

void display_draw_text(const std::string& text, int x, int y, uint16_t color) {
    // This is still a simplification for color mapping
    if (color == 0xFFFF) { // White
        graphics.set_pen(255, 255, 255);
    } else {
        graphics.set_pen(0, 0, 0);
    }
    graphics.text(text, Point(x, y), -1, 1);
}

void display_update() {
    st7735.update(&graphics);
}

void display_update_data(const BatteryData& data) {
    display_clear();
    char buffer[50];
    sprintf(buffer, "V: %dmV", data.voltage);
    display_draw_text(buffer, 5, 5, 0xFFFF);
    sprintf(buffer, "A: %dmA", data.current);
    display_draw_text(buffer, 5, 15, 0xFFFF);
    sprintf(buffer, "SOC: %d%%", data.relative_state_of_charge);
    display_draw_text(buffer, 5, 25, 0xFFFF);
    display_update();
}

void display_draw_menu(const std::vector<std::string>& items, int selected_index, const std::string& title) {
    display_clear();
    display_draw_text(title, 5, 5, 0xFFFF);
    int y = 15;
    for (int i = 0; i < items.size(); i++) {
        // Invert for selected item
        if (i == selected_index) {
             graphics.set_pen(255, 255, 255);
             graphics.rectangle({0, y-1, 160, 10});
             display_draw_text(items[i], 5, y, 0x0000);
        } else {
            display_draw_text(items[i], 5, y, 0xFFFF);
        }
        y += 10;
    }
    display_update();
}
