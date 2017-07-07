/*
 * FunctionLibary.h
 *
 * Created: 2017-05-13 21:08:49
 *  Author: Sebastian
 */

#ifndef FUNCTIONLIBARY_H_
#define FUNCTIONLIBARY_H_

#include <stdint.h>
#include <stdlib.h>

/*
 * Saves a Number which can be check whether it's a valid value or not,
 * see http://en.cppreference.com/w/cpp/utility/optional
 */
struct PlotterNumberValue {
  float number = 0.0;
  bool legal = false;
};

class FunctionLibary {
public:
  static void pause(long ms);
  /*
   * Finde die Position eines Characters und gebe diese zurück wenn der char
   * gefunden wurde
   */
  static int16_t findCharPos(char *cString, char find);
  /*
   * Finde die Nummer nach einem Char.
   * zB. G02 liefert 2, wenn findNumber(... 'G');
   * übergeben wurde.
   * Übergeben wird eine überprüfte Zahl, wenn keine Zahl gefunden wird
   * so wird die false flag der Zahl gesetzt.
   */
  static PlotterNumberValue findNumber(char *cString, size_t strSize, char find,
                                       bool convertToMM = false,
                                       uint8_t stepsPerMM = 255);
};

#endif /* FUNCTIONLIBARY_H_ */
