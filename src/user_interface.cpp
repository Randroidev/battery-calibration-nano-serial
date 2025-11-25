#include "user_interface.h"
#include "display_manager.h"
#include "input_manager.h"
#include <vector>
#include <string>

static ProcessController* p_controller;
static int menu_index = 0;
static std::vector<std::string> main_menu = {"Start Charge", "Start Discharge", "Start Calibration", "Settings"};

static void draw_main_menu() {
    display_clear();
    for (int i = 0; i < main_menu.size(); i++) {
        std::string item = (i == menu_index ? "> " : "  ") + main_menu[i];
        display_print_message(item);
    }
}

void ui_init(ProcessController& controller) {
    p_controller = &controller;
    input_init();
    draw_main_menu();
}

void ui_update() {
    if (p_controller->is_busy()) {
        // While a process is running, UI is blocked
        // and only shows battery data.
        // The process controller update will handle this.
        return;
    }

    UserAction action = input_get_action();
    switch (action) {
        case UserAction::ENCODER_UP:
            menu_index = (menu_index + 1) % main_menu.size();
            draw_main_menu();
            break;
        case UserAction::ENCODER_DOWN:
            menu_index = (menu_index - 1 + main_menu.size()) % main_menu.size();
            draw_main_menu();
            break;
        case UserAction::ENCODER_PRESS:
            if (main_menu[menu_index] == "Start Charge") {
                p_controller->start_charge();
            } else if (main_menu[menu_index] == "Start Discharge") {
                p_controller->start_discharge();
            } else if (main_menu[menu_index] == "Start Calibration") {
                p_controller->start_calibration(1); // 1 cycle for now
            } else if (main_menu[menu_index] == "Settings") {
                // Settings menu not implemented yet
            }
            break;
        case UserAction::BACK_PRESS:
            // In main menu, back does nothing
            break;
        case UserAction::NONE:
            break;
    }
}
