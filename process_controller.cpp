#include "process_controller.h"
#include "led_indicator.h"
#include "user_interface.h"
#include "config.h"
#include "battery_reporter.h"

ProcessController::ProcessController(BatteryManager& bat_manager) : battery(bat_manager) {
  current_process = Process::IDLE;
  consecutive_read_errors = 0;
}

void ProcessController::init() {
  pinMode(RELAY_PIN_CHARGE, OUTPUT);
  pinMode(RELAY_PIN_DISCHARGE, OUTPUT);
  digitalWrite(RELAY_PIN_CHARGE, RELAY_OFF);
  digitalWrite(RELAY_PIN_DISCHARGE, RELAY_OFF);
}

bool ProcessController::is_busy() const {
  return current_process != Process::IDLE;
}

void ProcessController::stop_process() {
  ui_print_message(F("\n# Process finished. Returning to menu."));
  control_relays(false, false);
  led_turn_off_all();
  current_process = Process::IDLE;
  consecutive_read_errors = 0;
  ui_show_main_menu();
}

void ProcessController::start_charge() {
  ui_print_message(F("\n# Starting Charge Process..."));
  current_process = Process::CHARGE;
  consecutive_read_errors = 0;
  step_start_time = millis();
  last_battery_read = 0;
  control_relays(true, false);
  led_indicate_charge();
}

void ProcessController::start_discharge() {
  ui_print_message(F("\n# Starting Discharge Process..."));
  current_process = Process::DISCHARGE;
  consecutive_read_errors = 0;
  step_start_time = millis();
  last_battery_read = 0;
  control_relays(false, true);
  led_indicate_discharge();
}

void ProcessController::start_calibration(int cycles) {
  ui_print_message(F("\n# Starting Calibration Process..."));
  current_process = Process::CALIBRATION;
  consecutive_read_errors = 0;
  total_cycles = cycles;
  current_cycle = 1;
  process_start_time = millis();
  step_start_time = millis(); // Initialize for the first step
  last_battery_read = 0;
  calib_step = CalibrationStep::PRE_CALIB_CHARGING;
}

void ProcessController::start_demo(int cycles) {
  ui_print_message(F("\n# Starting DEMO Process..."));
  current_process = Process::DEMO;
  consecutive_read_errors = 0;
  total_cycles = cycles;
  current_cycle = 1;
  process_start_time = millis();
  step_start_time = millis(); // Initialize for the first step
  last_battery_read = 0;
  calib_step = CalibrationStep::PRE_CALIB_CHARGING;
}

void ProcessController::update() {
  if (!is_busy()) return;

  switch (current_process) {
    case Process::CHARGE:      update_charge(); break;
    case Process::DISCHARGE:   update_discharge(); break;
    case Process::CALIBRATION: update_calibration_or_demo(false); break;
    case Process::DEMO:        update_calibration_or_demo(true); break;
    case Process::IDLE: break;
  }
}

void ProcessController::update_charge() {
  periodic_battery_check(true);
  
  if (battery.is_fully_charged() || battery.is_charge_inhibited() || battery.has_error()) {
    if (battery.has_error()) ui_print_message(F("## Stopping charge due to battery error."));
    else if (battery.is_charge_inhibited()) ui_print_message(F("## Stopping charge because FET was closed by battery."));
    else ui_print_message(F("## Battery is fully charged."));
    stop_process();
  }
}

void ProcessController::update_discharge() {
  periodic_battery_check(true);

  if (battery.is_fully_discharged() || battery.is_discharge_inhibited() || battery.has_error()) {
     if (battery.has_error()) ui_print_message(F("## Stopping discharge due to battery error."));
    else if (battery.is_discharge_inhibited()) ui_print_message(F("## Stopping discharge because FET was closed by battery."));
    else ui_print_message(F("## Battery is fully discharged."));
    stop_process();
  }
}

void ProcessController::update_calibration_or_demo(bool is_demo) {
  periodic_battery_check(false, is_demo);
  
  unsigned long wait_time = 0;
  unsigned long process_time = is_demo ? DEMO_PROCESS_DURATION_MS : 0;
  
  switch (calib_step) {
    case CalibrationStep::PRE_CALIB_CHARGING:
        control_relays(true, false); // Turn on charger
        led_indicate_charge();
        if ( (is_demo && millis() - step_start_time > process_time) || battery.is_fully_charged() || battery.is_charge_inhibited() || battery.has_error() ) {
            ui_print_message(F("## Initial charge complete. Starting 30-minute wait..."));
            led_indicate_charge_done();
            step_start_time = millis();
            calib_step = CalibrationStep::PRE_CALIB_WAITING;
        }
        break;
        
    case CalibrationStep::PRE_CALIB_WAITING:
      wait_time = is_demo ? DEMO_WAIT_DURATION_MS : CALIBRATION_PRE_CHARGE_WAIT_MS;
      if (millis() - step_start_time > wait_time) {
        calib_step = CalibrationStep::START_DISCHARGE;
      }
      break;
        
    case CalibrationStep::START_DISCHARGE:
        ui_print_message(String(F("\n## --- Cycle ")) + current_cycle + "/" + total_cycles + F(": Starting Discharge Phase ---"));
        control_relays(false, true);
        led_indicate_discharge();
        step_start_time = millis();
        calib_step = CalibrationStep::DISCHARGING;
      break;

    case CalibrationStep::DISCHARGING:
      if ( (is_demo && millis() - step_start_time > process_time) || battery.is_fully_discharged() || battery.is_discharge_inhibited() || battery.has_error() ) {
        ui_print_message(F("## Discharge phase complete. Starting 5-hour wait."));
        control_relays(false, false);
        led_indicate_waiting();
        step_start_time = millis();
        calib_step = CalibrationStep::POST_DISCHARGE_WAIT;
      }
      break;

    case CalibrationStep::POST_DISCHARGE_WAIT:
      wait_time = is_demo ? DEMO_WAIT_DURATION_MS : CALIBRATION_DISCHARGE_WAIT_MS;
      if (millis() - step_start_time > wait_time) {
        calib_step = CalibrationStep::START_CHARGE;
      }
      break;

    case CalibrationStep::START_CHARGE:
      ui_print_message(String(F("\n## --- Cycle ")) + current_cycle + "/" + total_cycles + F(": Starting Charge Phase ---"));
      control_relays(true, false);
      led_indicate_charge();
      step_start_time = millis();
      calib_step = CalibrationStep::CHARGING;
      break;

    case CalibrationStep::CHARGING:
      if ( (is_demo && millis() - step_start_time > process_time) || battery.is_fully_charged() || battery.is_charge_inhibited() || battery.has_error() ) {
        ui_print_message(F("## Charging phase complete. Starting 1-hour wait."));
        led_indicate_charge_done();
        step_start_time = millis();
        calib_step = CalibrationStep::POST_CHARGE_WAIT;
      }
      break;

    case CalibrationStep::POST_CHARGE_WAIT:
      wait_time = is_demo ? DEMO_WAIT_DURATION_MS : CALIBRATION_CHARGE_WAIT_MS;
      if (millis() - step_start_time > wait_time) {
        if (current_cycle < total_cycles) {
          current_cycle++;
          calib_step = CalibrationStep::START_DISCHARGE;
        } else {
          ui_print_message(F("\n## All calibration cycles complete."));
          stop_process();
        }
      }
      break;
  }
}

void ProcessController::periodic_battery_check(bool full_report, bool is_demo) {
  if (millis() - last_battery_read >= BATTERY_READ_INTERVAL_MS) {
    last_battery_read = millis();
    
    if (is_demo) {
      int state = 0;
      if (calib_step == CalibrationStep::CHARGING || calib_step == CalibrationStep::PRE_CALIB_CHARGING) state = 1;
      else if (calib_step != CalibrationStep::DISCHARGING) state = 2;
      battery.generate_demo_data(state);
    } else {
      if (!battery.read_data()) {
        consecutive_read_errors++;
        ui_print_message(String(F("## Error reading battery data (Attempt ")) + consecutive_read_errors + "/3)");
        if (consecutive_read_errors >= 3) {
          ui_print_message(F("## Aborting process due to too many read errors."));
          stop_process();
        }
        return;
      } else {
        consecutive_read_errors = 0;
      }
    }
    
    if(current_process == Process::CALIBRATION || current_process == Process::DEMO) {
        unsigned long elapsed = (millis() - process_start_time) / 1000;
        unsigned long hours = elapsed / 3600;
        unsigned long mins = (elapsed % 3600) / 60;
        unsigned long secs = elapsed % 60;
        
        String time_str = String(hours) + "h " + String(mins) + "m " + String(secs) + "s";

        ui_print_message(F("\n  ===== BATTERY TRAINER STATUS ====="));
        ui_print_param(F("Calibration Cycle"), String(current_cycle) + " / " + String(total_cycles));
        ui_print_param(F("Elapsed Time"), time_str);
    }

    reporter_print_data(battery.get_data(), full_report);
  }
}
void ProcessController::control_relays(bool charge, bool discharge) {
    digitalWrite(RELAY_PIN_CHARGE, charge ? RELAY_ON : RELAY_OFF);
    digitalWrite(RELAY_PIN_DISCHARGE, discharge ? RELAY_ON : RELAY_OFF);
}
