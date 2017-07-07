/*
 * GCodeParser.cpp
 *
 * Created: 2017-05-13 20:25:53
 *  Author: Sebastian
 */

#include "FunctionLibary.h"
#include "GCodeParser.h"
#include "ZAxisControllerCAN.h"
#include <Arduino.h>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <type_traits>

template <typename T> PlotterInterpreter<T>::PlotterInterpreter() {
  static_assert(std::is_base_of<IZAxisController, T>::value,
                "T must be derived from IZAxisController");
  setFeedrate(400);
  Serial.print("Homing...\n\r");
  stepperX.goHome();
  stepperY.goHome();
  Serial.print("Finished setup!\n\r");
}

template <typename T> PlotterInterpreter<T>::~PlotterInterpreter() {
  // Cleanup cStrings
}

template <typename T> void PlotterInterpreter<T>::displayHelp() {
  Serial.print("\n\n\e[31m+--------------------------------+\r\n");
  Serial.print("|                                |\r\n");
  Serial.print("|           CNC PRINTER          |\r\n");
  Serial.print("|             v. 0.3.2           |\r\n");
  Serial.print("+--------------------------------+\r\n");
}

template <typename T> void PlotterInterpreter<T>::gcodeReady() {
  gcodeCurrentPosition = 0;
  Serial.print("ok\n");
}

template <typename T> void PlotterInterpreter<T>::currentInfo() {
  Serial.printf("ok X: %.2f ", myState.x);
  Serial.printf("Y: %.2f ", myState.y);
  Serial.printf("Z: %.2f \r\n", myState.z);
}

template <typename T> void PlotterInterpreter<T>::readLine() {
  if (Serial.available() > 0) {
    char c = Serial.read();
    Serial.print(c);
    if (gcodeCurrentPosition < MAX_BUF)
      gcodeBuffer[gcodeCurrentPosition++] = c;
    else
      Serial.print("\e[91mOut of Memory...\e[39m");
    if (c == 13) {
      gcodeBuffer[gcodeCurrentPosition++] = 0;
      processCommand();
      gcodeReady();
    }
  }
}

template <typename T>
PlotterNumberValue PlotterInterpreter<T>::findNumber(char find,
                                                     bool convertToMM) {
  FunctionLibary::findNumber(gcodeBuffer, MAX_BUF, find, convertToMM,
                             stepsPerMM);
}

template <typename T>
float PlotterInterpreter<T>::calcMovementMod(long i, long dx) {

  float fact;
  if (dx > 0) {
    //<10% of the total distance -> build up speed
    if (i < dx * 0.1) {
      fact = 1.25 - ((double)i / dx) * 2.4; // from 75% to 100, 0.4 = 0.1/0.25
    } else if (i >= dx * 0.1 && i <= dx * 0.9) {
      fact = 1; // keep at fullspeed between >10% and <90%
    } else if (i > dx * 0.9) {
      fact = 1 + (((double)i / dx) - 0.9) * 5; // 5 = 0.5/0.1, mapping .1 to .5
    }
  }
  // prevent stuck movement
  if (fact < 1e-10)
    return 0.1;
  return fact;
}

template <typename T> void PlotterInterpreter<T>::setFeedrate(float speed) {
  if (speed <= 1)
    return; // Feedrate of 1 or smaller is.... slow, don't do it!
  if (myState.feed == speed)
    return; // do nothing if the same
  myState.stepDelay = 1000000.0 / speed;
  myState.feed = speed;
  stepperX.setStepDelay(myState.stepDelay);
  stepperY.setStepDelay(myState.stepDelay);
}

template <typename T> void PlotterInterpreter<T>::processCommand() {
  bool executionNeeded = false;
  PlotterNumberValue linenumber = findNumber('N');
  if (linenumber.legal) {
    myValues.lineNumber = linenumber.number;
  }
  PlotterNumberValue gcode = findNumber('G');

  if (gcode.legal) {
    int cmd = (int)gcode.number;
    switch (cmd) {
    // This one is G0 and G1, linear Movement they don't have any real
    // difference in my Code, normally they should be rapid movement (G0) or
    // Tool Movement (G1), but as modern programms don't care anymore about
    // those I'll just leave them as the same type of command
    // This will save only save the values needed in the according save vars, so
    // that we don't have to care about unchanged Feedrates etc..
    // Accepts X, Y, Z and F
    case 0:
    case 1: {
      myMovement.currentMovement = 1;
      PlotterNumberValue tmpfeed = findNumber('F');
      if (tmpfeed.legal)
        setFeedrate(tmpfeed.number);
      // Set X, Y, Z
      PlotterNumberValue tmpValue = findNumber('X', true);
      if (tmpValue.legal) { // TODO: Add Maximum Value
        if ((myMovement.distance == RELATIVE &&
             (myState.x + tmpValue.number >= 0)) ||
            myMovement.distance == ABSOLUTE)
          myValues.xyz[0] = tmpValue.number;
      }

      tmpValue = findNumber('Y', true);
      if (tmpValue.legal) {
        if ((myMovement.distance == RELATIVE &&
             (myState.y + tmpValue.number >= 0)) ||
            myMovement.distance == ABSOLUTE)
          myValues.xyz[1] = tmpValue.number;
      }

      tmpValue = findNumber('Z');
      if (tmpValue.legal) {
        if ((myMovement.distance == RELATIVE &&
             (myState.z + tmpValue.number >= 0)) ||
            myMovement.distance == ABSOLUTE)
          myValues.xyz[2] = tmpValue.number;
      }
      executionNeeded = true;
      break;
    }
    // Currently not really working is the circular movement, this should be
    // either clockwise (G2) or counter-clockwise (G3). I and J are values added
    // to X and Y
    // Accepts X, Y, Z, I, J and F
    case 2:
    case 3: {
      myMovement.currentMovement = cmd;
      PlotterNumberValue tmpfeed = findNumber('F');
      if (tmpfeed.legal)
        setFeedrate(tmpfeed.number);
      // Set X, Y, Z
      PlotterNumberValue tmpValue = findNumber('X', true);
      if (tmpValue.legal) { // TODO: Add Maximum Value
        if ((myMovement.distance == RELATIVE &&
             (myState.x + tmpValue.number >= 0)) ||
            myMovement.distance == ABSOLUTE)
          myValues.xyz[0] = tmpValue.number;
      }

      tmpValue = findNumber('Y', true);
      if (tmpValue.legal) {
        if ((myMovement.distance == RELATIVE &&
             (myState.y + tmpValue.number >= 0)) ||
            myMovement.distance == ABSOLUTE)
          myValues.xyz[1] = tmpValue.number;
      }

      tmpValue = findNumber('Z');
      if (tmpValue.legal) {
        if ((myMovement.distance == RELATIVE &&
             (myState.z + tmpValue.number >= 0)) ||
            myMovement.distance == ABSOLUTE)
          myValues.xyz[2] = tmpValue.number;
      }

      tmpValue = findNumber('I', true);
      if (tmpValue.legal) {
        myValues.ijk[0] = tmpValue.number;
      }

      tmpValue = findNumber('J', true);
      if (tmpValue.legal) {
        myValues.ijk[1] = tmpValue.number;
      }

      executionNeeded = true;
      break;
    }
    // Pause for a while.
    // Accepts S (Seconds) and P (Milliseconds)
    case 4: {
      uint32_t wait = 0;
      PlotterNumberValue tmpwait = findNumber('P');
      if (tmpwait.legal)
        wait += tmpwait.number;
      tmpwait = findNumber('S');
      if (tmpwait.legal)
        wait += (tmpwait.number * 1000);
      FunctionLibary::pause(wait);
      break;
    }
    // G20 and G21 change the units to either INCH or MM (Inch is not working
    // right now, but who needs inch ;-)
    case 20:
      myMovement.unit = INCH;
      break;
    case 21:
      myMovement.unit = MM;
      break;
    // Homeing command for all Axis, accepts X, Y, Z (but as we are 2D
    // currently only X and Y)
    case 28: {
      setFeedrate(400);
      PlotterNumberValue tmpX = findNumber('X');
      PlotterNumberValue tmpY = findNumber('Y');
      PlotterNumberValue tmpZ = findNumber('Z');
      if (tmpX.legal) {
        stepperX.goHome();
        myState.x = 0;
        cleanupMovement(true);
      } else if (tmpY.legal) {
        stepperY.goHome();
        myState.y = 0;
        cleanupMovement(true);
      } else if (tmpZ.legal) {
        myState.z = 0;
        cleanupMovement(true);
      } else {
        stepperX.goHome();
        stepperY.goHome();
        myState.x = 0;
        myState.z = 0;
        myState.y = 0;
        cleanupMovement(true);
      }
      zController.sendZPositionWithAck(myState.z);
      break;
    }
    // Movement Command for Either relative(G91) or absolute(G90) Movement
    case 90:
      myMovement.distance = ABSOLUTE;
      break;
    case 91:
      myMovement.distance = RELATIVE;
      break;
    default:
      break;
    }
  }

  // miscellaneous commands
  gcode = findNumber('M', false);
  if (gcode.legal) {
    int cmd = (int)gcode.number;
    switch (cmd) {
    // M2 (Programm End) and M18 (Disable all stepper motors)
    case 2:
    case 18:
    case 84:
      stepperX.stopSteppers();
      stepperY.stopSteppers();
      break;
    // M100 custom command for help (Debug only)
    case 100:
      displayHelp();
      break;
    // M112: Emergency Stop. As we can't implement this in an interrupt routine
    // I'll just leave it this way (see M2 and M18 - they do the same)
    case 112:
      stepperX.stopSteppers();
      stepperY.stopSteppers();
      break;
    // M114 should display important informations to the client
    case 114:
      currentInfo();
      break; // prints px, py, fr, and mode.
    default:
      break;
    }
  }

  // Does the G-code we just got need any Movement to execute?
  if (executionNeeded) {
    // If the steppers are stopped start them!
    if (stepperX.currentState() == OFF || stepperY.currentState() == OFF) {
      stepperX.startSteppers();
      stepperY.startSteppers();
    }
    // Set the Flag the we are currently working on something, so that no other
    // progress can interfere
    myMovement.currentState = IN_PROGRESS;
    executeMovement();
  }
}

template <typename T>
void PlotterInterpreter<T>::drawLine(float newx, float newy) {
  long over = 0;

  long dx = (newx - myState.x);                         // Distance X to travel
  long dy = (newy - myState.y);                         // Distance Y to travel
  StepperDirections dirx = dx > 0 ? FORWARD : BACKWARD; // Movement Direction
  StepperDirections diry = dy > 0 ? BACKWARD : FORWARD;
  dx = abs(dx); // Saved direction, so make the Movement absolute
  dy = abs(dy); // same...

  // do we have to do more X or Y steps?
  if (dx > dy) {
    over = dx / 2;
    for (long i = 0; i < dx; ++i) {
      // stepper.doMMStepX(dirx);
      stepperX.mStep(dirx);
      over += dy;
      if (over >= dx) {
        over -= dx;
        stepperY.mStep(diry);
      }

      FunctionLibary::pause((myState.stepDelay / microstep) *
                            calcMovementMod(i, dx));
    }
  } else {
    over = dy / 2;
    for (long i = 0; i < dy; ++i) {
      stepperY.mStep(diry);
      over += dx;
      if (over >= dy) {
        over -= dy;
        stepperX.mStep(dirx);
      }
      FunctionLibary::pause((myState.stepDelay / microstep) *
                            calcMovementMod(i, dy));
    }
  }

  myState.x = newx;
  myState.y = newy;
}

template <typename T> float PlotterInterpreter<T>::atan3(float dy, float dx) {
  float a = std::atan2(dy, dx);
  if (a < 0)
    a = (M_PI * 2.0) + a;
  return a;
}

template <typename T>
void PlotterInterpreter<T>::drawArc(float cx, float cy, float x, float y,
                                    StepperDirections dir) {
  // get radius
  float dx = (myState.x - cx);
  float dy = (myState.y - cy);
  float radius = std::sqrt(dx * dx + dy * dy);

  // find angle of arc (sweep)
  float angle1 = atan3(dy, dx);
  float angle2 = atan3(y - cy, x - cx);
  float theta = angle2 - angle1;

  if (dir == FORWARD && theta < 0) {
    angle2 += 2 * M_PI;
  } else if (dir == BACKWARD && theta > 0) {
    angle1 += 2 * M_PI;
  }

  theta = angle2 - angle1;

  // get length of arc
  // float circ=PI*2.0*radius;
  // float len=theta*circ/(PI*2.0);
  // simplifies to
  // std::abs works for float, fabs needs a cast
  float len = std::abs(theta) * radius;

  int i, segments = std::ceil(len * 0.1);

  float nx, ny, angle3, scale;

  for (i = 0; i < segments; ++i) {
    // interpolate around the arc
    scale = ((float)i) / ((float)segments);

    angle3 = (theta * scale) + angle1;
    nx = cx + std::cos(angle3) * radius;
    ny = cy + std::sin(angle3) * radius;
    // send it to the planner
    drawLine(round(nx), round(ny));
  }

  drawLine(round(x), round(y));
}

template <typename T> void PlotterInterpreter<T>::executeMovement() {
  switch (myMovement.currentMovement) {
  case 0:
  case 1: {
    float xMovement =
        myValues.xyz[0] + (myMovement.distance == RELATIVE ? myState.x : 0);
    float yMovement =
        myValues.xyz[1] + (myMovement.distance == RELATIVE ? myState.y : 0);
    float zMovement =
        myValues.xyz[2] + (myMovement.distance == RELATIVE ? myState.z : 0);
    if (abs(myState.z - zMovement) > 0.001) {
      zController.sendZPositionWithAck(zMovement);
      myState.z = zMovement;
    }
    drawLine(xMovement, yMovement);
    break;
  }
  case 2:
  case 3: {
    float xMovement =
        myValues.xyz[0] + (myMovement.distance == RELATIVE ? myState.x : 0);
    float yMovement =
        myValues.xyz[1] + (myMovement.distance == RELATIVE ? myState.y : 0);
    float zMovement =
        myValues.xyz[2] + (myMovement.distance == RELATIVE ? myState.z : 0);
    float iMovement = myValues.ijk[0] + myState.x;
    float jMovement = myValues.ijk[1] + myState.y;
    StepperDirections dir =
        (myMovement.currentMovement == 2) ? BACKWARD : FORWARD;

    // prevent Bus Spam, only send if there is really a change in the Z State
    if (abs(myState.z - zMovement) > 0.001) {
      zController.sendZPositionWithAck(zMovement);
      myState.z = zMovement;
    }
    drawArc(iMovement, jMovement, xMovement, yMovement, dir);
    break;
  }
  }
  cleanupMovement();
}

template <typename T>
void PlotterInterpreter<T>::cleanupMovement(bool forceAll) {
  // only reset the current coordinates to 0 if you are in relative mode!
  if (myMovement.distance == RELATIVE || forceAll) {
    memset(myValues.xyz, 0, sizeof(myValues.xyz));
  }
  // IJK should be reset as we don't have to save them for later
  memset(myValues.ijk, 0, sizeof(myValues.xyz));
  myValues.r = 0;
  myMovement.currentState = FINISHED;
}

template class PlotterInterpreter<ZAxisControllerCAN>;
template class PlotterInterpreter<ZAxisControllerI2C>;
