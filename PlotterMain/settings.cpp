#include "settings.h"

// The Type of communication to use for the information exchange of the Z-Axis
const CommunicationInterfaces COMMUNICATION_INTERFACE = CAN_BUS;

// The Amount of Microsteps selected in the driver chip
const uint8_t MICROSTEPS = 16;
// the amount of steps needed to move one MM, use MICROSTEPS and the base value
// without microstepping for a better overview
const uint8_t STEPS_PER_MM = MICROSTEPS * 5;

const int STEP_PIN_X = 26; // The X-Axis Pin for STEP
const int DIR_PIN_X = 27;  // The X-Axis Pin for DIR
const int STOP_PIN_X = 32; // The X-Axis Pin Connected to ENABLE
const int HOME_PIN_X = 13; // The X-Axis Pin connected to the HOME Switch
// The X-Axis Direction to move to to reach the Home coordinates
const StepperDirections HOME_DIR_X = BACKWARD;

const int STEP_PIN_Y = 33; // The X-Axis Pin for STEP
const int DIR_PIN_Y = 25;  // The X-Axis Pin for DIR
const int STOP_PIN_Y = 32; // The X-Axis Pin Connected to ENABLE
const int HOME_PIN_Y = 4;  // The X-Axis Pin connected to the HOME Switch
// The X-Axis Direction to move to to reach the Home coordinates
const StepperDirections HOME_DIR_Y = FORWARD;
