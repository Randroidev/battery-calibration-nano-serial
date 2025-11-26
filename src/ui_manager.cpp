#include "ui_manager.h"
#include "display.h"
#include "pico/stdio.h"
#include <iostream>

void ui_manager_init() {
    display_init();
}

void ui_manager_clear() {
    display_clear();
    printf("\033[2J\033[H");
    fflush(stdout);
}

void ui_manager_draw_menu(const std::vector<std::string>& items, int selected_index, const std::string& title) {
    display_clear();
    display_draw_menu(items, selected_index, title);
    display_update();

    printf("--- %s ---\n", title.c_str());
    for (int i = 0; i < items.size(); ++i) {
        printf("%s %s\n", (i == selected_index ? ">" : " "), items[i].c_str());
    }
    printf("\nUse arrows to navigate/change, Enter to select, Esc to go back.\n");
    fflush(stdout);
}

void ui_manager_update_data(const BatteryData& data) {
    display_clear();
    display_update_data(data);
    display_update();

    printf("\033[2J\033[H");
    printf("--- Battery Status ---\n");
    printf("Voltage: %d mV\n", data.voltage);
    printf("Current: %d mA\n", data.current);
    printf("SoC: %d%%\n", data.relative_state_of_charge);
    printf("SoH: %d%%\n", data.state_of_health);
    printf("Cycle Count: %d\n", data.cycle_count);
    printf("Temperature: %.1f C\n", (data.temperature / 10.0) - 273.15);
    fflush(stdout);
}

void ui_manager_print_message(const std::string& message) {
    display_clear();
    display_draw_text(message, 5, 5, 0xFFFF);
    display_update();

    printf("\n--- Message ---\n");
    printf("%s\n", message.c_str());
    fflush(stdout);
}
