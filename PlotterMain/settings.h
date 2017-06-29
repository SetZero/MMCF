#ifndef SETTINGS_H_
#define SETTINGS_H_
#include "LibStepper.h"
#include <stdint.h>

enum CommunicationInterfaces { CAN_BUS, I2C_BUS };
extern const CommunicationInterfaces COMMUNICATION_INTERFACE;

extern const uint8_t STEPS_PER_MM;
extern const uint8_t MICROSTEPS;
extern const uint8_t GCODE_BUFFER_SIZE;

extern const int STEP_PIN_X;
extern const int DIR_PIN_X;
extern const int STOP_PIN_X;
extern const int HOME_PIN_X;
extern const StepperDirections HOME_DIR_X;

extern const int STEP_PIN_Y;
extern const int DIR_PIN_Y;
extern const int STOP_PIN_Y;
extern const int HOME_PIN_Y;
extern const StepperDirections HOME_DIR_Y;

#endif
