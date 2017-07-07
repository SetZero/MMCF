/*
 * LibStepper.cpp
 *
 * Created: 2017-05-13 21:34:31
 *  Author: Sebastian
 */
#include "FunctionLibary.h"
#include "LibStepper.h"
#include <Arduino.h>

LibStepper::LibStepper(const char axisChar, const int stepPin, const int dirPin,
                       const int stopPin, const int homePin,
                       const StepperDirections homeDir, uint8_t mmStep,
                       uint8_t microstep)
    : AXIS_CHAR(axisChar), STEP_PIN(stepPin), DIR_PIN(dirPin),
      STOP_PIN(stopPin), HOME_DIR(homeDir), stepsPerMM(mmStep),
      microStep(microstep) {
  scontrol =
      std::unique_ptr<SwitchControl>(new SwitchControl(axisChar, homePin));
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(STOP_PIN, OUTPUT);

  Serial.printf("Called Setup Routine for Stepper %c...\n\r", axisChar);
  stopSteppers();
};

void LibStepper::mStep(StepperDirections dir) {
  if (state == OFF)
    startSteppers();
  digitalWrite(STEP_PIN, HIGH);
  if (dir == FORWARD)
    digitalWrite(DIR_PIN, HIGH);
  else
    digitalWrite(DIR_PIN, LOW);
  digitalWrite(STEP_PIN, LOW);
}

void LibStepper::doMMStep(StepperDirections dir) {
  for (uint8_t i = 0; i < (stepsPerMM); i++) {
    mStep(dir);
    FunctionLibary::pause(stepDelay / microStep);
  }
}

uint16_t LibStepper::goHome() {
  Serial.printf("Homeing %c...", AXIS_CHAR);
  scontrol->reachedHome = HOMEING;
  scontrol->enableInterruptSwitch();
  startSteppers();
  uint16_t distance = 0;
  while (scontrol->reachedHome != HOME) {
    mStep(HOME_DIR);
    distance++;
    FunctionLibary::pause(stepDelay / microStep);
  }
  stopSteppers();
  return distance;
}

void LibStepper::goMax() {
  scontrol->reachedHome = MAXING;
  scontrol->enableInterruptSwitch();
  startSteppers();
  while (scontrol->reachedHome != MAX) {
    if (HOME_DIR == FORWARD)
      mStep(BACKWARD);
    else
      mStep(FORWARD);
    FunctionLibary::pause(stepDelay / microStep);
  }
  stopSteppers();
}

void LibStepper::setStepDelay(long delay) { stepDelay = delay; }

void LibStepper::stopSteppers() {
  digitalWrite(STOP_PIN, HIGH);
  state = OFF;
}

void LibStepper::startSteppers() {
  digitalWrite(STOP_PIN, LOW);
  state = ON;
  delay(1); // Documentation recommends a short wait time after the stepers are
            // started. Sadly, "a short delay" hasn't been specified, so I'll go
            // for 1 ms
}

StepperState LibStepper::currentState() { return state; }
