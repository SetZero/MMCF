#include "settings.h"

const uint8_t MICROSTEPS = 16;
const uint8_t STEPS_PER_MM = MICROSTEPS * 5;

const int STEP_PIN_X = 26;
const int DIR_PIN_X = 27;
const int STOP_PIN_X = 32;
const int HOME_PIN_X = 13;
const StepperDirections HOME_DIR_X = BACKWARD;

const int STEP_PIN_Y = 33;
const int DIR_PIN_Y = 25;
const int STOP_PIN_Y = 32;
const int HOME_PIN_Y = 4;
const StepperDirections HOME_DIR_Y = FORWARD;
