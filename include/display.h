#ifndef DISPLAY_H
#define DISPLAY_H

#include <string>
#include <vector>
#include "battery_manager.h"

void display_init();
void display_clear();
void display_draw_text(const std::string& text, int x, int y, uint16_t color);
void display_update();
void display_update_data(const BatteryData& data);
void display_draw_menu(const std::vector<std::string>& items, int selected_index, const std::string& title);

#endif // DISPLAY_H
