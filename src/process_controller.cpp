#include "process_controller.h"
#include "led_indicator.h"
#include "config_manager.h" // Will be created later
#include "display_manager.h" // Will be created later

#define RELAY_PIN_CHARGE 6
#define RELAY_PIN_DISCHARGE 7

#define BATTERY_READ_INTERVAL_MS 15000

ProcessController::ProcessController(BatteryManager& bat_manager) : battery(bat_manager) {
    current_process = Process::IDLE;
    consecutive_read_errors = 0;
}

void ProcessController::init() {
    gpio_init(RELAY_PIN_CHARGE);
    gpio_set_dir(RELAY_PIN_CHARGE, GPIO_OUT);
    gpio_put(RELAY_PIN_CHARGE, 0);

    gpio_init(RELAY_PIN_DISCHARGE);
    gpio_set_dir(RELAY_PIN_DISCHARGE, GPIO_OUT);
    gpio_put(RELAY_PIN_DISCHARGE, 0);
}

bool ProcessController::is_busy() const {
    return current_process != Process::IDLE;
}

void ProcessController::stop_process() {
    display_print_message("\n# Process finished.");
    control_relays(false, false);
    led_turn_off_all();
    current_process = Process::IDLE;
    consecutive_read_errors = 0;
    // Here we should return to main menu in UI
}

void ProcessController::start_charge() {
    display_print_message("\n# Starting Charge Process...");
    current_process = Process::CHARGE;
    consecutive_read_errors = 0;
    step_start_time = get_absolute_time();
    last_battery_read = get_absolute_time();
    control_relays(true, false);
    led_indicate_charge();
}

void ProcessController::start_discharge() {
    display_print_message("\n# Starting Discharge Process...");
    current_process = Process::DISCHARGE;
    consecutive_read_errors = 0;
    step_start_time = get_absolute_time();
    last_battery_read = get_absolute_time();
    control_relays(false, true);
    led_indicate_discharge();
}

void ProcessController::start_calibration(int cycles) {
    display_print_message("\n# Starting Calibration Process...");
    current_process = Process::CALIBRATION;
    consecutive_read_errors = 0;
    total_cycles = cycles;
    current_cycle = 1;
    process_start_time = get_absolute_time();
    step_start_time = get_absolute_time();
    last_battery_read = get_absolute_time();
    calib_step = CalibrationStep::PRE_CALIB_CHARGING;
}

void ProcessController::update() {
    if (!is_busy()) return;

    switch (current_process) {
        case Process::CHARGE:      update_charge(); break;
        case Process::DISCHARGE:   update_discharge(); break;
        case Process::CALIBRATION: update_calibration(); break;
        case Process::IDLE: break;
    }
}

void ProcessController::update_charge() {
    periodic_battery_check();

    bool stop_charging = battery.is_fully_charged() || battery.has_error();
    if (config_get()->use_additional_conditions) {
        const BatteryData& data = battery.get_data();
        stop_charging = stop_charging || (data.voltage >= config_get()->charge_term_voltage || abs(data.current) < config_get()->charge_term_current);
    }

    if (stop_charging) {
        if (battery.has_error()) {
            display_print_message("## Stopping charge due to battery error.");
        } else {
            display_print_message("## Charge finished.");
        }
        stop_process();
    }
}

void ProcessController::update_discharge() {
    periodic_battery_check();

    bool stop_discharging = battery.is_fully_discharged() || battery.has_error();
    if (config_get()->use_additional_conditions) {
        const BatteryData& data = battery.get_data();
        stop_discharging = stop_discharging || (data.voltage <= config_get()->discharge_term_voltage || abs(data.current) < config_get()->discharge_term_current);
    }

    if (stop_discharging) {
        if (battery.has_error()) {
            display_print_message("## Stopping discharge due to battery error.");
        } else {
            display_print_message("## Discharge finished.");
        }
        stop_process();
    }
}

void ProcessController::update_calibration() {
    periodic_battery_check();

    int64_t wait_time_ms = 0;

    switch (calib_step) {
        case CalibrationStep::PRE_CALIB_CHARGING:
            control_relays(true, false);
            led_indicate_charge();
            if (battery.is_fully_charged() || battery.has_error()) {
                display_print_message("## Initial charge complete. Waiting...");
                led_indicate_charge_done();
                step_start_time = get_absolute_time();
                calib_step = CalibrationStep::PRE_CALIB_WAITING;
            }
            break;

        case CalibrationStep::PRE_CALIB_WAITING:
            wait_time_ms = config_get()->pre_charge_wait_ms;
            if (absolute_time_diff_us(step_start_time, get_absolute_time()) / 1000 > wait_time_ms) {
                calib_step = CalibrationStep::START_DISCHARGE;
            }
            break;

        case CalibrationStep::START_DISCHARGE:
            display_print_message("## Starting Discharge Phase...");
            control_relays(false, true);
            led_indicate_discharge();
            step_start_time = get_absolute_time();
            calib_step = CalibrationStep::DISCHARGING;
            break;

        case CalibrationStep::DISCHARGING:
            if (battery.is_fully_discharged() || battery.has_error()) {
                display_print_message("## Discharge complete. Waiting...");
                control_relays(false, false);
                led_indicate_waiting();
                step_start_time = get_absolute_time();
                calib_step = CalibrationStep::POST_DISCHARGE_WAIT;
            }
            break;

        case CalibrationStep::POST_DISCHARGE_WAIT:
            wait_time_ms = config_get()->discharge_wait_ms;
            if (absolute_time_diff_us(step_start_time, get_absolute_time()) / 1000 > wait_time_ms) {
                calib_step = CalibrationStep::START_CHARGE;
            }
            break;

        case CalibrationStep::START_CHARGE:
            display_print_message("## Starting Charge Phase...");
            control_relays(true, false);
            led_indicate_charge();
            step_start_time = get_absolute_time();
            calib_step = CalibrationStep::CHARGING;
            break;

        case CalibrationStep::CHARGING:
            if (battery.is_fully_charged() || battery.has_error()) {
                display_print_message("## Charge complete. Waiting...");
                led_indicate_charge_done();
                step_start_time = get_absolute_time();
                calib_step = CalibrationStep::POST_CHARGE_WAIT;
            }
            break;

        case CalibrationStep::POST_CHARGE_WAIT:
            wait_time_ms = config_get()->charge_wait_ms;
            if (absolute_time_diff_us(step_start_time, get_absolute_time()) / 1000 > wait_time_ms) {
                if (current_cycle < total_cycles) {
                    current_cycle++;
                    calib_step = CalibrationStep::START_DISCHARGE;
                } else {
                    display_print_message("\n## All cycles complete.");
                    stop_process();
                }
            }
            break;
    }
}

void ProcessController::periodic_battery_check() {
    if (absolute_time_diff_us(last_battery_read, get_absolute_time()) / 1000 >= BATTERY_READ_INTERVAL_MS) {
        last_battery_read = get_absolute_time();

        if (!battery.read_data()) {
            consecutive_read_errors++;
            if (consecutive_read_errors >= 3) {
                display_print_message("## Aborting due to read errors.");
                stop_process();
            }
            return;
        } else {
            consecutive_read_errors = 0;
        }

        // Reporting data to display will be handled by UI module
    }
}

void ProcessController::control_relays(bool charge, bool discharge) {
    gpio_put(RELAY_PIN_CHARGE, charge);
    gpio_put(RELAY_PIN_DISCHARGE, discharge);
}
