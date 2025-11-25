#include "ui_manager.h"
#include "display_manager.h"
#include "pico/stdio.h"
#include <iostream>

void ui_manager_init() {
    display_init();
}

void ui_manager_clear() {
    display_clear();
    // ANSI escape code to clear terminal screen
    printf("\033[2J\033[H");
    fflush(stdout);
}

void ui_manager_draw_menu(const std::vector<std::string>& items, int selected_index, const std::string& title) {
    display_clear();
    printf("--- %s ---\n", title.c_str());

    int y = 10;
    for (int i = 0; i < items.size(); ++i) {
        std::string line = items[i];
        if (i == selected_index) {
            display_draw_text(line, 10, y, 0x0000, 0xFFE0); // Black on yellow
            printf("> %s\n", line.c_str());
        } else {
            display_draw_text(line, 10, y, 0xFFFF, 0x0000); // White on black
            printf("  %s\n", line.c_str());
        }
        y += 12;
    }

    // Instructions for terminal users
    printf("\nUse 'w'/'s' to navigate, 'e' to select, 'q' to go back.\n");
    fflush(stdout);
}

void ui_manager_update_data(const BatteryData& data) {
    display_update_data(data); // The display function already has formatting

    // Clear terminal and print data
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
    display_draw_text(message, 10, 10, 0xFFFF);

    printf("\n--- Message ---\n");
    printf("%s\n", message.c_str());
    fflush(stdout);
}
