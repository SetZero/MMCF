/*
 * LibStepper.h
 *
 * Created: 2017-05-13 21:31:13
 *  Author: Sebastian
 */

#ifndef LIBSTEPPER_H_
#define LIBSTEPPER_H_

#include "SwitchControl.h"
#include <memory>
#include <stdint.h>

enum StepperState { OFF, ON };
enum StepperDirections { FORWARD, BACKWARD };

struct PlotterCoordinates {
  float x = 0.0;
  float y = 0.0;
  float z = 0.0;
};

class LibStepper {
private:
  const char AXIS_CHAR;
  const int STEP_PIN;
  const int DIR_PIN;
  const int STOP_PIN;
  const StepperDirections HOME_DIR;
  uint8_t stepsPerMM = 40;
  uint8_t microStep = 16;
  long stepDelay = (long)0.0001;
  StepperState state = OFF;
  std::unique_ptr<SwitchControl> scontrol;

public:
  /*
   * Initiates the Stepper Driver Chip pins and deactivates the hold (break)
   * state in which the driver is by default
   */
  LibStepper(const char axisChar, const int stepPin, const int dirPin,
             const int stopPin, const int homePin,
             const StepperDirections HomeDir, uint8_t mmStep = 40,
             uint8_t microstep = 16);
  /* Do one step on the Y axis */
  void mStep(StepperDirections dir);
  /* Move one millimeter on the X axis */
  void doMMStep(StepperDirections dir);
  /* Go to the Home Position on the X axis */
  uint16_t goHome();
  /* Go to the Home Position on the Y axis */
  void goMax();
  /* mutator for stepDelay */
  void setStepDelay(long delay);
  /* Stops all current flow to the steppers (disables driver chip)*/
  void stopSteppers();
  /*
   * Sets the stepper motors implicitly to hold (break) state and activates the
   * driver chip for further commands
   */
  void startSteppers();
  /* Get the state of the steppers (are they on or off?) */
  StepperState currentState();
};

#endif /* LIBSTEPPER_H_ */
