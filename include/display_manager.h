#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <string>
#include "battery_manager.h" // For BatteryData

void display_init();
void display_clear();
void display_print_message(const std::string& message);
void display_update_data(const BatteryData& data);

#endif // DISPLAY_MANAGER_H
