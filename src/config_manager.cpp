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

void config_init() {
    // Load default values
    current_config.use_additional_conditions = true;
    current_config.charge_term_voltage = 12600;
    current_config.charge_term_current = 10;
    current_config.discharge_term_voltage = 7500;
    current_config.discharge_term_current = 10;
    current_config.pre_charge_wait_ms = 1800000;
    current_config.charge_wait_ms = 3600000;
    current_config.discharge_wait_ms = 18000000;

    // Try to load from flash
    config_load();
}

void config_load() {
    const uint8_t* flash_addr = get_flash_target_addr();
    memcpy(&current_config, flash_addr, sizeof(AppConfig));
    // A simple validity check could be added here (e.g., magic number)
}

void config_save() {
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
