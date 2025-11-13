#include "battery_manager.h"
#include "user_interface.h"

#define SBS_CMD_CHARGING_CURRENT         0x14
#define SBS_CMD_CHARGING_VOLTAGE         0x15
#define SBS_CMD_BATTERY_STATUS           0x16
#define SBS_CMD_CYCLE_COUNT              0x17
#define SBS_CMD_DESIGN_CAPACITY          0x18
#define SBS_CMD_DESIGN_VOLTAGE           0x19
#define SBS_CMD_SPECIFICATION_INFO       0x1A
#define SBS_CMD_MANUFACTURE_DATE         0x1B
#define SBS_CMD_SERIAL_NUMBER            0x1C
#define SBS_CMD_MANUFACTURER_NAME        0x20
#define SBS_CMD_DEVICE_NAME              0x21
#define SBS_CMD_CHEMISTRY                0x22
#define SBS_CMD_CELL_VOLTAGE_4           0x3C
#define SBS_CMD_CELL_VOLTAGE_3           0x3D
#define SBS_CMD_CELL_VOLTAGE_2           0x3E
#define SBS_CMD_CELL_VOLTAGE_1           0x3F
#define SBS_CMD_TEMPERATURE              0x08
#define SBS_CMD_VOLTAGE                  0x09
#define SBS_CMD_CURRENT                  0x0A
#define SBS_CMD_RELATIVE_SOC             0x0D
#define SBS_CMD_ABSOLUTE_SOC             0x0E
#define SBS_CMD_REMAINING_CAPACITY       0x0F
#define SBS_CMD_FULL_CHARGE_CAPACITY     0x10

const byte SMBUS_ADDRESS = 0x0B;

BatteryManager::BatteryManager() {
  memset(&data, 0, sizeof(BatteryData));
}

bool BatteryManager::connect() {
  Wire.begin();
  Wire.beginTransmission(SMBUS_ADDRESS);
  byte error = Wire.endTransmission();
  if (error != 0) {
    ui_print_message(String(F("Connection error: ")) + error);
  }
  return (error == 0);
}

bool BatteryManager::read_data() {
  data.voltage = read_smbus_word(SBS_CMD_VOLTAGE);
  data.current = (int16_t)read_smbus_word(SBS_CMD_CURRENT);
  data.relative_state_of_charge = read_smbus_word(SBS_CMD_RELATIVE_SOC);
  data.absolute_state_of_charge = read_smbus_word(SBS_CMD_ABSOLUTE_SOC);
  data.remaining_capacity = read_smbus_word(SBS_CMD_REMAINING_CAPACITY);
  data.full_charge_capacity = read_smbus_word(SBS_CMD_FULL_CHARGE_CAPACITY);
  data.temperature = read_smbus_word(SBS_CMD_TEMPERATURE);
  data.cycle_count = read_smbus_word(SBS_CMD_CYCLE_COUNT);
  data.battery_status_word = read_smbus_word(SBS_CMD_BATTERY_STATUS);
  data.manufacturer_name = read_smbus_string(SBS_CMD_MANUFACTURER_NAME);
  data.device_name = read_smbus_string(SBS_CMD_DEVICE_NAME);
  data.chemistry = read_smbus_string(SBS_CMD_CHEMISTRY);
  data.design_capacity = read_smbus_word(SBS_CMD_DESIGN_CAPACITY);
  data.design_voltage = read_smbus_word(SBS_CMD_DESIGN_VOLTAGE);
  data.manufacture_date = read_smbus_word(SBS_CMD_MANUFACTURE_DATE);
  data.serial_number = read_smbus_word(SBS_CMD_SERIAL_NUMBER);
  data.specification_info = read_smbus_word(SBS_CMD_SPECIFICATION_INFO);
  data.cell_voltage_1 = read_smbus_word(SBS_CMD_CELL_VOLTAGE_1);
  data.cell_voltage_2 = read_smbus_word(SBS_CMD_CELL_VOLTAGE_2);
  data.cell_voltage_3 = read_smbus_word(SBS_CMD_CELL_VOLTAGE_3);
  data.cell_voltage_4 = read_smbus_word(SBS_CMD_CELL_VOLTAGE_4);
  data.charging_current = read_smbus_word(SBS_CMD_CHARGING_CURRENT);
  data.charging_voltage = read_smbus_word(SBS_CMD_CHARGING_VOLTAGE);

  if (data.battery_status_word == 0xFFFF) {
    data.error_condition = true;
    return false;
  }
  
  parse_status_flags(data.battery_status_word);
  
  if (data.design_capacity > 0) {
    data.state_of_health = (uint16_t)((data.full_charge_capacity * 100L) / data.design_capacity);
  } else {
    data.state_of_health = 0;
  }
  
  return true;
}

void BatteryManager::generate_demo_data(int state) {
  data.manufacturer_name = "DEMO INC.";
  data.device_name = "DEMO-BATT";
  data.chemistry = "LION";
  data.design_capacity = 6000;
  data.design_voltage = 11100;
  data.manufacture_date = (2023-1980)*512 + 10*32 + 26;
  data.serial_number = 12345;
  data.specification_info = 33;
  data.cycle_count = 10;
  data.charging_current = 1500;
  data.charging_voltage = 12600;
  data.error_condition = false;
  data.fully_charged = false;
  data.fully_discharged = false;

  if (state == 0) {
    data.relative_state_of_charge = max(0, data.relative_state_of_charge - 5);
    data.current = -1500;
    if (data.relative_state_of_charge <= 5) data.fully_discharged = true;
  } else if (state == 1) {
    data.relative_state_of_charge = min(100, data.relative_state_of_charge + 5);
    data.current = 2000;
    if (data.relative_state_of_charge >= 99) data.fully_charged = true;
  } else {
    data.current = 0;
  }
  
  data.absolute_state_of_charge = data.relative_state_of_charge;
  data.remaining_capacity = (uint16_t)(5000L * data.relative_state_of_charge / 100);
  data.full_charge_capacity = 5000;
  data.voltage = 10000 + (25 * data.relative_state_of_charge);
  data.temperature = 2982 + random(0, 50);
  data.cell_voltage_1 = data.voltage/3;
  data.cell_voltage_2 = data.voltage/3;
  data.cell_voltage_3 = data.voltage/3;
  data.cell_voltage_4 = 0;
  data.state_of_health = 95;
}

const BatteryData& BatteryManager::get_data() const {
  return data;
}

bool BatteryManager::is_fully_charged() const { return data.fully_charged; }
bool BatteryManager::is_fully_discharged() const { return data.fully_discharged; }
bool BatteryManager::is_charge_inhibited() const { return data.charge_fet_closed; }
bool BatteryManager::is_discharge_inhibited() const { return data.discharge_fet_closed; }
bool BatteryManager::has_error() const { return data.error_condition; }

uint16_t BatteryManager::read_smbus_word(byte command) {
  uint16_t result = 0xFFFF;
  Wire.beginTransmission(SMBUS_ADDRESS);
  Wire.write(command);
  if (Wire.endTransmission(false) != 0) return result;
  if (Wire.requestFrom(SMBUS_ADDRESS, (byte)2) == 2) {
    byte lowByte = Wire.read();
    byte highByte = Wire.read();
    result = (uint16_t)highByte << 8 | lowByte;
  }
  return result;
}

String BatteryManager::read_smbus_string(byte command) {
  char buffer[32];
  memset(buffer, 0, sizeof(buffer));
  
  Wire.beginTransmission(SMBUS_ADDRESS);
  Wire.write(command);
  if (Wire.endTransmission(false) != 0) return "READ_ERR";

  byte len = 0;
  if (Wire.requestFrom(SMBUS_ADDRESS, (byte)32, (byte)true) > 0) {
    len = Wire.read();
    if (len > 31) len = 31;
    for (byte i = 0; i < len; i++) {
      buffer[i] = Wire.read();
    }
  }
  return String(buffer);
}

void BatteryManager::parse_status_flags(uint16_t status_word) {
  data.error_condition = (status_word & 0x8000) != 0;
  data.fully_charged = (status_word & 0x0020) != 0;
  data.fully_discharged = (status_word & 0x0010) != 0;
  data.charge_fet_closed = data.fully_charged;
  data.discharge_fet_closed = data.fully_discharged;
}
