#include "display_manager.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include <string>
#include <vector>

// Display SPI settings
#define SPI_PORT spi0
#define SPI_MOSI_PIN 19
#define SPI_SCK_PIN  18
#define SPI_CS_PIN   17
#define SPI_MISO_PIN 16 // Not used for display
#define SPI_RESET_PIN 14
#define SPI_DC_PIN   15

#define ST7789_NOP     0x00
#define ST7789_SWRESET 0x01
#define ST7789_SLPOUT  0x11
#define ST7789_NORON   0x13
#define ST7789_INVOFF  0x20
#define ST7789_INVON   0x21
#define ST7789_DISPON  0x29
#define ST7789_CASET   0x2A
#define ST7789_RASET   0x2B
#define ST7789_RAMWR   0x2C
#define ST7789_COLMOD  0x3A
#define ST7789_MADCTL  0x36

#define DISPLAY_WIDTH 240
#define DISPLAY_HEIGHT 240

static void write_command(uint8_t cmd) {
    gpio_put(SPI_DC_PIN, 0);
    gpio_put(SPI_CS_PIN, 0);
    spi_write_blocking(SPI_PORT, &cmd, 1);
    gpio_put(SPI_CS_PIN, 1);
}

static void write_data(const uint8_t* data, size_t len) {
    gpio_put(SPI_DC_PIN, 1);
    gpio_put(SPI_CS_PIN, 0);
    spi_write_blocking(SPI_PORT, data, len);
    gpio_put(SPI_CS_PIN, 1);
}

void st7789_init() {
    spi_init(SPI_PORT, 1000 * 1000 * 20); // 20 MHz
    gpio_set_function(SPI_MOSI_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI_SCK_PIN, GPIO_FUNC_SPI);

    gpio_init(SPI_CS_PIN);
    gpio_set_dir(SPI_CS_PIN, GPIO_OUT);
    gpio_put(SPI_CS_PIN, 1);

    gpio_init(SPI_RESET_PIN);
    gpio_set_dir(SPI_RESET_PIN, GPIO_OUT);

    gpio_init(SPI_DC_PIN);
    gpio_set_dir(SPI_DC_PIN, GPIO_OUT);

    // Reset the display
    gpio_put(SPI_RESET_PIN, 0);
    sleep_ms(10);
    gpio_put(SPI_RESET_PIN, 1);
    sleep_ms(10);

    write_command(ST7789_SWRESET);
    sleep_ms(150);
    write_command(ST7789_SLPOUT);
    sleep_ms(255);

    // Memory Data Access Control
    uint8_t madctl_data = 0x04; // Row/Col addr order, BGR color
    write_command(ST7789_MADCTL);
    write_data(&madctl_data, 1);

    // Interface Pixel Format
    uint8_t colmod_data = 0x55; // 16-bit/pixel
    write_command(ST7789_COLMOD);
    write_data(&colmod_data, 1);

    write_command(ST7789_NORON);
    sleep_ms(10);

    write_command(ST7789_DISPON);
    sleep_ms(255);

    st7789_clear();
}

void st7789_clear() {
    uint16_t black = 0x0000;
    std::vector<uint16_t> buffer(DISPLAY_WIDTH * DISPLAY_HEIGHT, black);

    uint8_t caset_data[] = {0, 0, 0, DISPLAY_WIDTH - 1};
    write_command(ST7789_CASET);
    write_data(caset_data, 4);

    uint8_t raset_data[] = {0, 0, 0, DISPLAY_HEIGHT - 1};
    write_command(ST7789_RASET);
    write_data(raset_data, 4);

    write_command(ST7789_RAMWR);
    write_data((uint8_t*)buffer.data(), buffer.size() * 2);
}

#include "font.h"

static uint16_t framebuffer[DISPLAY_WIDTH * DISPLAY_HEIGHT];

static void set_pixel(int x, int y, uint16_t color) {
    if (x >= 0 && x < DISPLAY_WIDTH && y >= 0 && y < DISPLAY_HEIGHT) {
        framebuffer[y * DISPLAY_WIDTH + x] = color;
    }
}

void st7789_draw_text(const std::string& text, int x, int y, uint16_t color, uint16_t bg_color) {
    int start_x = x;
    for (char c : text) {
        if (c == '\n') {
            y += 8;
            x = start_x;
            continue;
        }
        if (c < 32 || c > 127) c = '?';
        const uint8_t* glyph = font[(int)c];
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                set_pixel(x + j, y + i, ((glyph[i] >> j) & 1) ? color : bg_color);
            }
        }
        x += 8;
    }
}

static void render_framebuffer() {
    uint8_t caset_data[] = {0, 0, 0, DISPLAY_WIDTH - 1};
    write_command(ST7789_CASET);
    write_data(caset_data, 4);

    uint8_t raset_data[] = {0, 0, 0, DISPLAY_HEIGHT - 1};
    write_command(ST7789_RASET);
    write_data(raset_data, 4);

    write_command(ST7789_RAMWR);
    write_data((uint8_t*)framebuffer, sizeof(framebuffer));
}

void st7789_update_data(const BatteryData& data) {
    char buffer[50];
    sprintf(buffer, "Voltage: %d mV", data.voltage);
    display_draw_text(buffer, 10, 10, 0xFFFF);
    sprintf(buffer, "Current: %d mA", data.current);
    display_draw_text(buffer, 10, 20, 0xFFFF);
    sprintf(buffer, "SOC: %d %%", data.relative_state_of_charge);
    display_draw_text(buffer, 10, 30, 0xFFFF);
    render_framebuffer();
}

void st7789_draw_menu(const std::vector<std::string>& items, int selected_index) {
    int y = 10;
    for (int i = 0; i < items.size(); i++) {
        if (i == selected_index) {
            display_draw_text(items[i], 10, y, 0x0000, 0xFFE0); // Black on yellow
        } else {
            display_draw_text(items[i], 10, y, 0xFFFF, 0x0000); // White on black
        }
        y += 10;
    }
    render_framebuffer();
}
