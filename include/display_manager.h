#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <string>
#include <vector>
#include "battery_manager.h" // For BatteryData

void display_init();
void display_clear();
void display_draw_text(const std::string& text, int x, int y, uint16_t color, uint16_t bg_color = 0x0000);
void display_update_data(const BatteryData& data);
void display_draw_menu(const std::vector<std::string>& items, int selected_index);

#endif // DISPLAY_MANAGER_H
