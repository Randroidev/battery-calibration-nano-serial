#include "config_manager.h"
#include "hardware/flash.h"
#include "hardware/sync.h"
#include <cstring>

#define FLASH_TARGET_OFFSET (PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE)

static AppConfig current_config;

// Helper function to get the flash address
static uint8_t* get_flash_target_addr() {
    return (uint8_t*)(XIP_BASE + FLASH_TARGET_OFFSET);
}

static void set_defaults() {
    current_config.magic = CONFIG_MAGIC;
    current_config.display_type = ST7789_240x240;
    current_config.use_additional_conditions = true;
    current_config.charge_term_voltage = 12600;
    current_config.charge_term_current = 10;
    current_config.discharge_term_voltage = 7500;
    current_config.discharge_term_current = 10;
    current_config.pre_charge_wait_ms = 1800000;
    current_config.charge_wait_ms = 3600000;
    current_config.discharge_wait_ms = 18000000;
}

void config_init() {
    config_load();
    if (current_config.magic != CONFIG_MAGIC) {
        set_defaults();
        config_save();
    }
}

void config_load() {
    const uint8_t* flash_addr = get_flash_target_addr();
    memcpy(&current_config, flash_addr, sizeof(AppConfig));
}

void config_save() {
    current_config.magic = CONFIG_MAGIC;
    uint8_t buffer[FLASH_PAGE_SIZE];
    memset(buffer, 0xFF, FLASH_PAGE_SIZE);
    memcpy(buffer, &current_config, sizeof(AppConfig));

    uint32_t ints = save_and_disable_interrupts();
    flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
    flash_range_program(FLASH_TARGET_OFFSET, buffer, FLASH_PAGE_SIZE);
    restore_interrupts(ints);
}

AppConfig* config_get() {
    return &current_config;
}
