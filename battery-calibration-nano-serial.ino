#include "config.h"
#include "led_indicator.h"
#include "user_interface.h"
#include "battery_manager.h"
#include "process_controller.h"
#include "battery_reporter.h"

BatteryManager battery;
ProcessController controller(battery);

void handle_menu_input();
int get_cycle_count();

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  while (!Serial);
  
  ui_print_message(F("--- Battery Calibrator Initializing ---"));

  led_init();
  controller.init();
  
  bool battery_found = false;
  for (int i = 0; i < BATTERY_CONNECT_RETRIES; i++) {
    ui_print_message(String(F("Attempting to connect to battery... (Attempt ")) + (i + 1) + ")", true);
    if (battery.connect()) {
      battery_found = true;
      break;
    }
    delay(1000);
  }

  if (battery_found) {
    ui_print_message(F("\nReading initial battery data..."));
    if (battery.read_data()) {
      reporter_print_data(battery.get_data(), true);
    } else {
      ui_print_message(F("Failed to read initial battery data."));
    }
  } else {
    ui_print_message(F("\nCould not connect to battery. Continuing without battery data."));
    ui_print_message(F("Warning: Only DEMO mode is recommended."));
  }

  ui_show_main_menu();
}

void loop() {
  controller.update();

  if (!controller.is_busy()) {
    handle_menu_input();
  }
}

void handle_menu_input() {
  if (Serial.available() > 0) {
    int choice = ui_read_integer();

    switch (choice) {
      case 1:
      case 5: {
        int cycles = get_cycle_count();
        if (cycles > 0) {
          if (choice == 1) controller.start_calibration(cycles);
          else controller.start_demo(cycles);
        } else {
          ui_show_main_menu();
        }
        break;
      }
      
      case 2:
        ui_print_message(F("\nReading battery data..."));
        if (battery.read_data()) {
          reporter_print_data(battery.get_data(), true);
        } else {
          ui_print_message(F("Failed to read battery data."));
        }
        ui_show_main_menu();
        break;
        
      case 3:
        controller.start_charge();
        break;
        
      case 4:
        controller.start_discharge();
        break;
        
      default:
        ui_print_message(F("Invalid choice. Please try again."));
        ui_show_main_menu();
        break;
    }
  }
}

int get_cycle_count() {
  int cycles = -1;
  while (cycles < 0 || cycles > 5) {
    ui_prompt_for_cycles();
    cycles = ui_read_integer();
    if (cycles < 0 || cycles > 5) {
      ui_prompt_invalid_cycles();
    }
  }
  return cycles;
}
