/*
 * SwitchControl.cpp
 *
 * Created: 2017-05-16 21:41:15
 *  Author: Sebastian
 */
#include "SwitchControl.h"
#include <Arduino.h>

std::map<char, SwitchControl *> mySwitches;

void interruptHandlerHome() {
  Serial.print("Got an Interrupt!\n\r");
  for (auto const &mySwitch : mySwitches) {
    if (mySwitch.second->reachedHome == HOMEING) {
      mySwitch.second->reachedHome = HOME;
      mySwitch.second->clearInterruptSwitch();
      Serial.printf("Reached Home %c!\n", mySwitch.second->axisChar);
    } else if (mySwitch.second->reachedHome == MAXING) {
      mySwitch.second->reachedHome = MAX;
      mySwitch.second->clearInterruptSwitch();
      Serial.print("Reached MAX!\n");
    }
  }
}

SwitchControl::SwitchControl(char axis, const int home)
    : HOME(home), axisChar(axis) {
  mySwitches[axis] = this;
  SwitchControl::enableInterruptSwitch(); // Enable Interrupt 1
}

void SwitchControl::swap(SwitchControl &other) {
  using std::swap;
  swap(axisChar, other.axisChar);
  swap(HOME, other.HOME);
}

SwitchControl &SwitchControl::operator=(SwitchControl &&rhs) {
  swap(rhs);
  return *this;
}
SwitchControl::SwitchControl(SwitchControl &&other) { swap(other); }

SwitchControl::~SwitchControl() {
  delete mySwitches.at(axisChar);
  mySwitches.erase(axisChar);
}

void SwitchControl::enableInterruptSwitch() {
  Serial.printf("Attached an Interrupt to Pin #%d\n\r", HOME);
  // already HIGH ?
  if (digitalRead(HOME)) {
    interruptHandlerHome();
  } else {
    attachInterrupt(HOME, interruptHandlerHome, HIGH);
  }
}

void SwitchControl::clearInterruptSwitch() {
  // EIMSK &= ~(1 << INT1);
  // EIFR = EIFR | 0x01;
  // Even though this shouldn't do anything if we
  // accidentally enter the Interrupt without needing it
  // it is better to disable this for performance reasons
  detachInterrupt(HOME);
}
