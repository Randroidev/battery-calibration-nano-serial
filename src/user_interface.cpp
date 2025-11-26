#include "user_interface.h"
#include "ui_manager.h"
#include "input_manager.h"
#include "config_manager.h"
#include "hardware/watchdog.h"
#include <vector>
#include <string>
#include <stdio.h>

enum class UIState {
    MAIN_MENU,
    SETTINGS_MENU,
    EDIT_SETTING
};

static ProcessController* p_controller;
static UIState current_state = UIState::MAIN_MENU;
static int menu_index = 0;
static int settings_menu_index = 0;

static std::vector<std::string> main_menu_items = {"Start Charge", "Start Discharge", "Start Calibration", "Calibration Demo", "Settings"};
static std::vector<std::string> settings_menu_items;

static std::string format_time(uint32_t ms) {
    uint32_t seconds = ms / 1000;
    uint32_t hours = seconds / 3600;
    seconds %= 3600;
    uint32_t minutes = seconds / 60;
    seconds %= 60;
    char buffer[20];
    sprintf(buffer, "%luh %lum %lus", hours, minutes, seconds);
    return std::string(buffer);
}

static void populate_settings_menu() {
    settings_menu_items.clear();
    AppConfig* config = config_get();
    char buffer[50];

    sprintf(buffer, "Use Add. Cond: %s", config->use_additional_conditions ? "ON" : "OFF");
    settings_menu_items.push_back(buffer);
    sprintf(buffer, "Charge Term V: %d", config->charge_term_voltage);
    settings_menu_items.push_back(buffer);
    sprintf(buffer, "Charge Term A: %d", config->charge_term_current);
    settings_menu_items.push_back(buffer);
    sprintf(buffer, "Discharge Term V: %d", config->discharge_term_voltage);
    settings_menu_items.push_back(buffer);
    sprintf(buffer, "Discharge Term A: %d", config->discharge_term_current);
    settings_menu_items.push_back(buffer);
    settings_menu_items.push_back("Pre-Charge Wait: " + format_time(config->pre_charge_wait_ms));
    settings_menu_items.push_back("Charge Wait: " + format_time(config->charge_wait_ms));
    settings_menu_items.push_back("Discharge Wait: " + format_time(config->discharge_wait_ms));
    sprintf(buffer, "Calibration Cycles: %d", config->calibration_cycles);
    settings_menu_items.push_back(buffer);
    settings_menu_items.push_back("Demo Stage Time: " + format_time(config->demo_stage_duration_ms));
}

static void draw_menu() {
    if (current_state == UIState::MAIN_MENU) {
        ui_manager_draw_menu(main_menu_items, menu_index, "Main Menu");
    } else if (current_state == UIState::SETTINGS_MENU || current_state == UIState::EDIT_SETTING) {
        populate_settings_menu();
        ui_manager_draw_menu(settings_menu_items, settings_menu_index, "Settings");
    }
}

void ui_init(ProcessController& controller) {
    p_controller = &controller;
    input_init();
    draw_menu();
}

void ui_update_main_menu(UserAction action) {
    switch (action) {
        case UserAction::ENCODER_UP:
            menu_index = (menu_index + 1) % main_menu_items.size();
            draw_menu();
            break;
        case UserAction::ENCODER_DOWN:
            menu_index = (menu_index - 1 + main_menu_items.size()) % main_menu_items.size();
            draw_menu();
            break;
        case UserAction::ENCODER_PRESS:
            if (main_menu_items[menu_index] == "Start Charge") p_controller->start_charge();
            else if (main_menu_items[menu_index] == "Start Discharge") p_controller->start_discharge();
            else if (main_menu_items[menu_index] == "Start Calibration") p_controller->start_calibration(1);
            else if (main_menu_items[menu_index] == "Settings") {
                current_state = UIState::SETTINGS_MENU;
                settings_menu_index = 0;
                draw_menu();
            }
            break;
        case UserAction::BACK_PRESS: // Escape in main menu
            watchdog_reboot(0, 0, 100);
            break;
        default: break;
    }
}

void ui_update_edit_setting(UserAction action) {
    AppConfig* config = config_get();
    int int_step = 100;
    long long_step = 60000;

    switch (settings_menu_index) {
        case 0: // use_additional_conditions
            if (action == UserAction::ENCODER_UP || action == UserAction::ENCODER_DOWN)
                config->use_additional_conditions = !config->use_additional_conditions;
            break;
        case 1: // charge_term_voltage
            if (action == UserAction::ENCODER_UP) config->charge_term_voltage += int_step;
            if (action == UserAction::ENCODER_DOWN) config->charge_term_voltage -= int_step;
            break;
        case 2: // charge_term_current
            if (action == UserAction::ENCODER_UP) config->charge_term_current += 10;
            if (action == UserAction::ENCODER_DOWN) config->charge_term_current -= 10;
            break;
        case 3: // discharge_term_voltage
            if (action == UserAction::ENCODER_UP) config->discharge_term_voltage += int_step;
            if (action == UserAction::ENCODER_DOWN) config->discharge_term_voltage -= int_step;
            break;
        case 4: // discharge_term_current
            if (action == UserAction::ENCODER_UP) config->discharge_term_current += 10;
            if (action == UserAction::ENCODER_DOWN) config->discharge_term_current -= 10;
            break;
        case 5: // pre_charge_wait_ms
            if (action == UserAction::ENCODER_UP) config->pre_charge_wait_ms += long_step;
            if (action == UserAction::ENCODER_DOWN) config->pre_charge_wait_ms -= long_step;
            break;
        case 6: // charge_wait_ms
            if (action == UserAction::ENCODER_UP) config->charge_wait_ms += long_step;
            if (action == UserAction::ENCODER_DOWN) config->charge_wait_ms -= long_step;
            break;
        case 7: // discharge_wait_ms
            if (action == UserAction::ENCODER_UP) config->discharge_wait_ms += long_step;
            if (action == UserAction::ENCODER_DOWN) config->discharge_wait_ms -= long_step;
            break;
        case 8: // calibration_cycles
            if (action == UserAction::ENCODER_UP) config->calibration_cycles++;
            if (action == UserAction::ENCODER_DOWN) config->calibration_cycles--;
            break;
        case 9: // demo_stage_duration_ms
            if (action == UserAction::ENCODER_UP) config->demo_stage_duration_ms += 1000;
            if (action == UserAction::ENCODER_DOWN) config->demo_stage_duration_ms -= 1000;
            break;
    }

    if (action != UserAction::NONE) {
        draw_menu();
    }

    if (action == UserAction::ENCODER_PRESS || action == UserAction::BACK_PRESS) {
        current_state = UIState::SETTINGS_MENU;
        draw_menu();
    }
}

void ui_update_settings_menu(UserAction action) {
    switch (action) {
        case UserAction::ENCODER_UP:
            settings_menu_index = (settings_menu_index + 1) % settings_menu_items.size();
            draw_menu();
            break;
        case UserAction::ENCODER_DOWN:
            settings_menu_index = (settings_menu_index - 1 + settings_menu_items.size()) % settings_menu_items.size();
            draw_menu();
            break;
        case UserAction::ENCODER_PRESS:
            current_state = UIState::EDIT_SETTING;
            break;
        case UserAction::BACK_PRESS:
            current_state = UIState::MAIN_MENU;
            config_save();
            draw_menu();
            break;
        default: break;
    }
}

void ui_update() {
    if (p_controller->is_busy()) return;

    UserAction action = input_get_action();
    if (action == UserAction::NONE) return;

    switch (current_state) {
        case UIState::MAIN_MENU:
            ui_update_main_menu(action);
            break;
        case UIState::SETTINGS_MENU:
            ui_update_settings_menu(action);
            break;
        case UIState::EDIT_SETTING:
            ui_update_edit_setting(action);
            break;
    }
}
