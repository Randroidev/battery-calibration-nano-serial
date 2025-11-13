#include "battery_reporter.h"
#include "user_interface.h"

static String parse_manufacture_date(uint16_t date_word);
static void print_detailed_status_flags(uint16_t status_word);

void reporter_print_data(const BatteryData& data, bool full_report) {
  if (full_report) {
    ui_print_message(F("  --- SMART BATTERY DATA (Full Report) ---"));
    ui_print_param(F("Manufacturer Name          "), data.manufacturer_name);
    ui_print_param(F("Device Name                "), data.device_name);
    ui_print_param(F("Chemistry                  "), data.chemistry);
    ui_print_param(F("Design Capacity (mAh)      "), String(data.design_capacity));
    ui_print_param(F("Design Voltage (mV)        "), String(data.design_voltage));
    ui_print_param(F("Manufacture Date (Y-M-D)   "), parse_manufacture_date(data.manufacture_date));
    ui_print_param(F("Serial Number              "), String(data.serial_number));
    ui_print_param(F("Specification Info         "), String(data.specification_info));
    ui_print_param(F("Cycle Count                "), String(data.cycle_count));
    ui_print_param(F("Full Charge Capacity (mAh) "), String(data.full_charge_capacity));
    ui_print_param(F("Remaining Capacity (mAh)   "), String(data.remaining_capacity));
    ui_print_param(F("Relative Charge (%)        "), String(data.relative_state_of_charge));
    ui_print_param(F("Absolute Charge (%)        "), String(data.absolute_state_of_charge));
    ui_print_param(F("State of Health (%)        "), String(data.state_of_health));
    ui_print_param(F("Cell 1 Voltage (mV)        "), String(data.cell_voltage_1));
    ui_print_param(F("Cell 2 Voltage (mV)        "), String(data.cell_voltage_2));
    ui_print_param(F("Cell 3 Voltage (mV)        "), String(data.cell_voltage_3));
    ui_print_param(F("Cell 4 Voltage (mV)        "), String(data.cell_voltage_4));
    ui_print_param(F("Charging Current (mA)      "), String(data.charging_current));
    ui_print_param(F("Charging Voltage (mV)      "), String(data.charging_voltage));
    ui_print_param(F("Temp (C)                   "), String(data.temperature / 10.0 - 273.15, 2));
    ui_print_param(F("Voltage (mV)               "), String(data.voltage));
    ui_print_param(F("Current (mA)               "), String(data.current));
  } else {
    ui_print_message(F("  --- SMART BATTERY DATA (Live) ---"));
    ui_print_param(F("Remaining Capacity (mAh)   "), String(data.remaining_capacity));
    ui_print_param(F("Relative Charge (%)        "), String(data.relative_state_of_charge));
    ui_print_param(F("Absolute Charge (%)        "), String(data.absolute_state_of_charge));
    ui_print_param(F("State of Health (%)        "), String(data.state_of_health));
    ui_print_param(F("Temp (C)                   "), String(data.temperature / 10.0 - 273.15, 2));
    ui_print_param(F("Voltage (mV)               "), String(data.voltage));
    ui_print_param(F("Current (mA)               "), String(data.current));
  }
  
  print_detailed_status_flags(data.battery_status_word);
}

static String parse_manufacture_date(uint16_t date_word) {
  int day = date_word & 0x1F;
  int month = (date_word >> 5) & 0x0F;
  int year = ((date_word >> 9) & 0x7F) + 1980;
  return String(year) + "-" + String(month) + "-" + String(day);
}

static void print_detailed_status_flags(uint16_t status_word) {
  ui_print_message(F("  --- Battery Status Flags ---"));
  ui_print_param(F("Fully Charged (FC)                 "), (status_word & 0x0020) ? "SET" : "CLEAR");
  ui_print_param(F("Fully Discharged (FD)              "), (status_word & 0x0010) ? "SET" : "CLEAR");
  ui_print_param(F("Is Discharging                     "), (status_word & 0x0040) ? "YES" : "NO");
  ui_print_param(F("Initialized                        "), (status_word & 0x0080) ? "YES" : "NO");
  ui_print_message(F("  --- Battery Alarm Flags ---"));
  ui_print_param(F("Over Charged Alarm (OCA)           "), (status_word & 0x8000) ? "SET" : "CLEAR");
  ui_print_param(F("Terminate Charge Alarm (TCA)       "), (status_word & 0x4000) ? "SET" : "CLEAR");
  ui_print_param(F("Over Temp Alarm (OTA)              "), (status_word & 0x1000) ? "SET" : "CLEAR");
  ui_print_param(F("Terminate Discharge Alarm (TDA)    "), (status_word & 0x0800) ? "SET" : "CLEAR");
  ui_print_param(F("Remaining Capacity Alarm (RCA)     "), (status_word & 0x0200) ? "SET" : "CLEAR");
  ui_print_param(F("Remaining Time Alarm (RTA)         "), (status_word & 0x0100) ? "SET" : "CLEAR");
  ui_print_message(F("  =================================="));
}
