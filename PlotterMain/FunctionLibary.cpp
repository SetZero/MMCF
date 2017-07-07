/*
 * FunctionLibary.cpp
 *
 * Created: 2017-05-13 21:09:58
 *  Author: Sebastian
 */
#include "Arduino.h"
#include "FunctionLibary.h"
#include <stdint.h>

void FunctionLibary::pause(long ms) {
  delay(ms / 1000);
  delayMicroseconds((ms) % 1000); // delayMicroseconds over 16k will crash
}

int16_t FunctionLibary::findCharPos(char *cString, char find) {
  size_t pos = 0;
  while (pos < strlen(cString)) {
    if (cString[pos] == find) {
      return pos;
    }
    pos++;
  }
  return -1;
}

PlotterNumberValue FunctionLibary::findNumber(char *cString, size_t strSize,
                                              char find, bool convertToMM,
                                              uint8_t stepsPerMM) {
  uint8_t isNegative = 0;
  uint8_t isFract = 0;
  float value = 0;
  int c;
  float fraction = 1.0;
  PlotterNumberValue output;

  int16_t numberpos = findCharPos(cString, find);
  if (numberpos == -1) {
    output.legal = false;
    return output;
  }
  size_t currpos =
      numberpos + 1; // +1 as we have to go one step behind the found char

  // Go through C-Array for as long as we have found either a Digit (0-9),
  // signedness char (only -), which is explicit (as we have do..while) or a
  // Fractal Character (.)
  do {
    c = cString[currpos];
    if (c == '-')
      isNegative = 1;
    else if (c == '.') {
      isFract = 1;
    } else if (c >= '0' && c <= '9') {
      value = value * 10 + c - '0';
      if (isFract)
        fraction *= 0.1;
    }
    currpos++;
    c = cString[currpos];
  } while ((((c >= '0' && c <= '9') || (c == '.' && !isFract)) &&
            currpos <= strSize));

  // If we have found a sign char (-) change the number to negative
  if (isNegative)
    value = -value;

  // If we found a fract, change the number by 10^-n and while we are on it we
  // can change the found amount by stepsPerMM
  if (isFract) {
    if (convertToMM)
      output.number = value * fraction * stepsPerMM;
    else
      output.number = value * fraction;
    output.legal = true;
    return output;
  }
  if (convertToMM)
    output.number = value * stepsPerMM;
  else
    output.number = value;
  output.legal = true;
  return output;
}
