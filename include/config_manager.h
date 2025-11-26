#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <cstdint>

#define CONFIG_MAGIC 0xDEADBEEF

enum DisplayType {
    ST7789_240x240,
    ST7735_128x160
};

struct AppConfig {
    uint32_t magic;
    DisplayType display_type;
    bool use_additional_conditions;
    uint16_t charge_term_voltage;
    uint16_t charge_term_current;
    uint16_t discharge_term_voltage;
    uint16_t discharge_term_current;
    uint32_t pre_charge_wait_ms;
    uint32_t charge_wait_ms;
    uint32_t discharge_wait_ms;
};

void config_init();
void config_load();
void config_save();
AppConfig* config_get();

#endif // CONFIG_MANAGER_H
