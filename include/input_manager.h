#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

enum class UserAction {
    NONE,
    ENCODER_UP,
    ENCODER_DOWN,
    ENCODER_PRESS,
    BACK_PRESS
};

void input_init();
UserAction input_get_action();

#endif // INPUT_MANAGER_H
