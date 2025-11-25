#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include <string>
#include <vector>
#include "battery_manager.h"

// Initializes the UI manager and its underlying output devices
void ui_manager_init();

// Clears all UI surfaces (display and terminal)
void ui_manager_clear();

// Displays a menu with a selected item highlighted
void ui_manager_draw_menu(const std::vector<std::string>& items, int selected_index, const std::string& title);

// Displays the battery data screen
void ui_manager_update_data(const BatteryData& data);

// Displays a simple message to the user
void ui_manager_print_message(const std::string& message);

#endif // UI_MANAGER_H
