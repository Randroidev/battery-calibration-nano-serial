#include "battery_reporter.h"
#include "display_manager.h"

void reporter_print_data(const BatteryData& data) {
    display_update_data(data);
}
