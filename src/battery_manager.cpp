#include "battery_manager.h"
#include "hardware/i2c.h"
#include <cstring>
#include <vector>

// I2C settings
#define I2C_PORT i2c0
#define I2C_SDA_PIN 4
#define I2C_SCL_PIN 5
#define SMBUS_ADDRESS 0x0B

// SBS Commands
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

BatteryManager::BatteryManager() {
    memset(&data, 0, sizeof(BatteryData));
}

bool BatteryManager::connect() {
    i2c_init(I2C_PORT, 100 * 1000);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);

    // Check for device presence
    int ret = i2c_write_blocking(I2C_PORT, SMBUS_ADDRESS, nullptr, 0, false);
    return (ret >= 0);
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

const BatteryData& BatteryManager::get_data() const {
    return data;
}

bool BatteryManager::is_fully_charged() const { return data.fully_charged; }
bool BatteryManager::is_fully_discharged() const { return data.fully_discharged; }
bool BatteryManager::has_error() const { return data.error_condition; }

uint16_t BatteryManager::read_smbus_word(uint8_t command) {
    uint8_t buffer[2];
    i2c_write_blocking(I2C_PORT, SMBUS_ADDRESS, &command, 1, true);
    int read_count = i2c_read_blocking(I2C_PORT, SMBUS_ADDRESS, buffer, 2, false);
    if (read_count != 2) return 0xFFFF;
    return (uint16_t)buffer[1] << 8 | buffer[0];
}

std::string BatteryManager::read_smbus_string(uint8_t command) {
    i2c_write_blocking(I2C_PORT, SMBUS_ADDRESS, &command, 1, true);

    uint8_t len;
    int read_count = i2c_read_blocking(I2C_PORT, SMBUS_ADDRESS, &len, 1, false);
    if (read_count != 1 || len > 31) return "READ_ERR";

    std::vector<char> buffer(len + 1, 0);
    read_count = i2c_read_blocking(I2C_PORT, SMBUS_ADDRESS, (uint8_t*)buffer.data(), len, false);

    if (read_count != len) return "READ_ERR";

    return std::string(buffer.data(), len);
}

void BatteryManager::parse_status_flags(uint16_t status_word) {
    data.error_condition = (status_word & 0x8000) != 0;
    data.fully_charged = (status_word & 0x0020) != 0;
    data.fully_discharged = (status_word & 0x0010) != 0;
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

  if (state == 0) { // Discharging
    data.relative_state_of_charge = std::max(0, data.relative_state_of_charge - 20);
    data.current = -1500;
    if (data.relative_state_of_charge <= 5) data.fully_discharged = true;
  } else if (state == 1) { // Charging
    data.relative_state_of_charge = std::min(100, data.relative_state_of_charge + 20);
    data.current = 2000;
    if (data.relative_state_of_charge >= 99) data.fully_charged = true;
  } else { // Idle
    data.current = 0;
  }

  data.absolute_state_of_charge = data.relative_state_of_charge;
  data.remaining_capacity = (uint16_t)(5000L * data.relative_state_of_charge / 100);
  data.full_charge_capacity = 5000;
  data.voltage = 10000 + (25 * data.relative_state_of_charge);
  data.temperature = 2982;
  data.cell_voltage_1 = data.voltage/3;
  data.cell_voltage_2 = data.voltage/3;
  data.cell_voltage_3 = data.voltage/3;
  data.cell_voltage_4 = 0;
  data.state_of_health = 95;
}
