#ifndef BATTERY_REPORTER_H
#define BATTERY_REPORTER_H

#include "battery_manager.h"

void reporter_print_data(const BatteryData& data, bool full_report);

#endif // BATTERY_REPORTER_H
