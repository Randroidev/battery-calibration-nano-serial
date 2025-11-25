#include "pico/stdlib.h"
#include "battery_manager.h"
#include "process_controller.h"
#include "led_indicator.h"
#include "display_manager.h"
#include "user_interface.h"
#include "config_manager.h"

int main() {
    stdio_init_all();

    config_init();
    led_init();
    display_init();

    BatteryManager battery;
    if (!battery.connect()) {
        display_draw_text("Battery not found!", 10, 10, 0xFFFF, 0x0000);
        while (true) tight_loop_contents();
    }

    ProcessController controller(battery);
    controller.init();

    ui_init(controller);

    while (true) {
        controller.update();
        ui_update();

        // Reporting data to display
        if (controller.is_busy()) {
            static absolute_time_t last_report_time = {0};
            if (time_us_64() - to_us_since_boot(last_report_time) > 1000 * 1000) {
                 display_update_data(battery.get_data());
                 last_report_time = get_absolute_time();
            }
        }

        sleep_ms(10);
    }

    return 0;
}
