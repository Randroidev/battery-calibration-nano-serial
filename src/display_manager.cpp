#include "display_manager.h"
#include "config_manager.h"

// Forward declarations for the driver-specific functions
void st7789_init();
void st7789_clear();
void st7789_draw_text(const std::string& text, int x, int y, uint16_t color, uint16_t bg_color);
void st7789_update_data(const BatteryData& data);
void st7789_draw_menu(const std::vector<std::string>& items, int selected_index);

void st7735_init();
void st7735_clear();
void st7735_draw_text(const std::string& text, int x, int y, uint16_t color, uint16_t bg_color);
void st7735_update_data(const BatteryData& data);
void st7735_draw_menu(const std::vector<std::string>& items, int selected_index);


void display_init() {
    if (config_get()->display_type == ST7789_240x240) {
        st7789_init();
    } else {
        st7735_init();
    }
}

void display_clear() {
    if (config_get()->display_type == ST7789_240x240) {
        st7789_clear();
    } else {
        st7735_clear();
    }
}

void display_draw_text(const std::string& text, int x, int y, uint16_t color, uint16_t bg_color) {
    if (config_get()->display_type == ST7789_240x240) {
        st7789_draw_text(text, x, y, color, bg_color);
    } else {
        st7735_draw_text(text, x, y, color, bg_color);
    }
}

void display_update_data(const BatteryData& data) {
    if (config_get()->display_type == ST7789_240x240) {
        st7789_update_data(data);
    } else {
        st7735_update_data(data);
    }
}

void display_draw_menu(const std::vector<std::string>& items, int selected_index) {
    if (config_get()->display_type == ST7789_240x240) {
        st7789_draw_menu(items, selected_index);
    } else {
        st7735_draw_menu(items, selected_index);
    }
}
