#ifndef BATTERY_MANAGER_H
#define BATTERY_MANAGER_H

#include <Arduino.h>
#include <Wire.h>

struct BatteryData {
  String manufacturer_name;
  String device_name;
  String chemistry;
  uint16_t design_capacity;
  uint16_t design_voltage;
  uint16_t manufacture_date;
  uint16_t serial_number;
  uint16_t specification_info;
  uint16_t cell_voltage_1;
  uint16_t cell_voltage_2;
  uint16_t cell_voltage_3;
  uint16_t cell_voltage_4;
  uint16_t charging_current;
  uint16_t charging_voltage;
  uint16_t voltage;
  int16_t current;
  uint16_t relative_state_of_charge;
  uint16_t absolute_state_of_charge;
  uint16_t remaining_capacity;
  uint16_t full_charge_capacity;
  uint16_t temperature;
  uint16_t cycle_count;
  uint16_t battery_status_word;
  uint16_t state_of_health;
  bool fully_discharged;
  bool fully_charged;
  bool discharge_fet_closed;
  bool charge_fet_closed;
  bool error_condition;
};

class BatteryManager {
public:
  BatteryManager();
  bool connect();
  bool read_data();
  void generate_demo_data(int state);
  const BatteryData& get_data() const;
  bool is_fully_charged() const;
  bool is_fully_discharged() const;
  bool is_charge_inhibited() const;
  bool is_discharge_inhibited() const;
  bool has_error() const;

private:
  BatteryData data;
  uint16_t read_smbus_word(byte command);
  String read_smbus_string(byte command);
  void parse_status_flags(uint16_t status_word);
};

#endif // BATTERY_MANAGER_H
