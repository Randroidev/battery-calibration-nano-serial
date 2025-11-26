#include "display_manager.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "font.h"
#include <string>
#include <vector>

// Dummy implementations for ST7735
void st7735_init() { /* ST7735 initialization logic */ }
void st7735_clear() { /* ST7735 clear logic */ }
void st7735_draw_text(const std::string& text, int x, int y, uint16_t color, uint16_t bg_color) { /* ST7735 text logic */ }
void st7735_update_data(const BatteryData& data) { /* ST7735 data logic */ }
void st7735_draw_menu(const std::vector<std::string>& items, int selected_index) { /* ST7735 menu logic */ }
