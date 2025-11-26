#ifndef PROCESS_CONTROLLER_H
#define PROCESS_CONTROLLER_H

#include "battery_manager.h"

enum class Process {
    IDLE,
    CHARGE,
    DISCHARGE,
    CALIBRATION,
    CALIBRATION_DEMO
};

enum class CalibrationStep {
    PRE_CALIB_CHARGING,
    PRE_CALIB_WAITING,
    START_DISCHARGE,
    DISCHARGING,
    POST_DISCHARGE_WAIT,
    START_CHARGE,
    CHARGING,
    POST_CHARGE_WAIT
};

class ProcessController {
public:
    ProcessController(BatteryManager& bat_manager);
    void init();
    bool is_busy() const;

    void start_charge();
    void start_discharge();
    void start_calibration(int cycles);
    void start_calibration_demo();
    void stop_process();
    void update();

private:
    void update_charge();
    void update_discharge();
    void update_calibration();
    void periodic_battery_check();
    void control_relays(bool charge, bool discharge);
    bool should_stop_charging();
    bool should_stop_discharging();

    BatteryManager& battery;
    Process current_process;
    CalibrationStep calib_step;

    int total_cycles;
    int current_cycle;

    absolute_time_t step_start_time;
    absolute_time_t process_start_time;
    absolute_time_t last_battery_read;

    int consecutive_read_errors;
};

#endif // PROCESS_CONTROLLER_H
