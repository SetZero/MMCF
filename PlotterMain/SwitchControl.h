/*
 * SwitchControl.h
 *
 * Created: 2017-05-16 21:41:26
 *  Author: Sebastian
 */

#ifndef SWITCHCONTROL_H_
#define SWITCHCONTROL_H_

#include <map>

enum SwitchState { HOME, MAX, UNDEFINED, HOMEING, MAXING };

class SwitchControl {
private:
  int HOME = 13; // Y=4, X=13

public:
  SwitchState reachedHome = {UNDEFINED};
  char axisChar;

  SwitchControl(char axis, const int home);
  SwitchControl(SwitchControl &&other);
  SwitchControl &operator=(SwitchControl &&rhs);
  ~SwitchControl();

  void enableInterruptSwitch();
  void clearInterruptSwitch();
  void swap(SwitchControl &other);

  friend void interruptHandlerHome();
};

extern std::map<char, SwitchControl *> mySwitches;
#endif /* HOMESWITCHCONTROL_H_ */
