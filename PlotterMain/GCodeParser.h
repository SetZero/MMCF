/*
 * GCodeParser.h
 *
 * Created: 2017-05-13 20:25:43
 *  Author: Sebastian
 */

#ifndef GCODEPARSER_H_
#define GCODEPARSER_H_

#include "FunctionLibary.h"
#include "LibStepper.h"
#include "ZAxisControllerCAN.h"
#include "ZAxisControllerI2C.h"
#include "settings.h"
#include <stdint.h>

enum PlotterDistanceModes { RELATIVE, ABSOLUTE };
enum PlotterUnits { MM, INCH };
enum PlotterCurrentPrintState { IN_PROGRESS, FINISHED, SLEEP };

/*
 * Saves Information about the executed commands and their resulting values
 */
struct PlotterMovmentInformation {
  uint8_t currentMovement = 0; // G0, G1, G2, G3, ...
  PlotterUnits unit = MM;      // G20(in), G21(cm) -> Inch not implemented!
  PlotterDistanceModes distance =
      ABSOLUTE; // G90, G91 / Relative(0) or Absolute(1) Mode
  PlotterCurrentPrintState currentState = FINISHED; // Printer ready?
  uint16_t maxXCoordinates = 0;
};

/*
 * Saves the current state of the Plotter, like the x,y,z position and the feed
 * reate with it's resulting machine readable step delay
 */
struct PlotterState {
  float x = 0;    // current x position
  float y = 0;    // current y position
  float z = 0;    // curent z position
  float feed;     // feed rate (arbitrary value)
  long stepDelay; // machine readable feed rate (arbitrary value)
};

/*
 * Saves the position coordinates to be done and the current line which is saved
 */
struct PlotterValues {
  float ijk[3];             // I, J, K axis
  float xyz[3] = {0, 0, 0}; // X, Y, Z Axis
  int32_t lineNumber;       // current line
  float r;                  // arc radius
};

template <typename T> class PlotterInterpreter {
private:
  const static uint8_t MAX_BUF = 128; // max size of the buffer
  char gcodeBuffer[MAX_BUF];          // UART rec char buffer
  int gcodeCurrentPosition = 0;       // Position in UART rec char buffer
  PlotterState myState;
  PlotterValues myValues;
  PlotterMovmentInformation myMovement;
  uint8_t microstep = MICROSTEPS;    // 1/n Step
  uint8_t stepsPerMM = STEPS_PER_MM; // Steps needed for one MM

  LibStepper stepperY{'Y',        STEP_PIN_Y, DIR_PIN_Y,  STOP_PIN_Y,
                      HOME_PIN_Y, HOME_DIR_Y, stepsPerMM, microstep};
  LibStepper stepperX{'X',        STEP_PIN_X, DIR_PIN_X,  STOP_PIN_X,
                      HOME_PIN_X, HOME_DIR_X, stepsPerMM, microstep};

  T zController;

  /*
   * Executes all movement commands saved in the current Movement variables.
   * Will only be called if a real movement command is read in readLine()
   */
  void executeMovement();
  /*
   * Use this after the execution of a Movement Command!
   */
  void cleanupMovement(bool forceAll = false);
  /*
   * Four quadrant inverse tangent
   */
  float atan3(float dy, float dx);
  /*
   * Lese alle Informationen über aktuellen GCode in den GCode Speicher
   * Überprüft ob aktuelles Kommando ein valides ist und wenn es eine Bewegungs-
   * aktion benötigt setze das Flag dafür.
   * Alles Codes nach http://reprap.org/wiki/G-code
   */
  void processCommand();
  /* Calculates a Movement Modifier which prevents instant stops and makes
   * movement more precise:
   * Speed^
   * 100% | .~´""""""""\
   * 75%  |             \
   * 50%  |
   * 25%  +---------------->
   *    0%              100% Distance
   */
  float calcMovementMod(long i, long dx);

public:
  /*
   * Constructor sets the feedrate to a given value and moves to the home
   * position
   */
  PlotterInterpreter();
  ~PlotterInterpreter();
  /*
   *  Custom Help Message with the current version
   */
  void displayHelp();
  /*
   * Shows a ack for the last commands
   */
  void gcodeReady();
  /*
   * Prints out the current Position Information
   */
  void currentInfo();
  /*
   * Lese aktuelle Zeile welche im UART Buffer steht ein und wenn es ein Enter
   * ist
   * führe Auswertung aus. Wenn der Speicher voll ist sende einen Fehler.
   */
  void readLine();
  /*
  * Wrapper function of FunctionLibary::findNumber()
  */
  PlotterNumberValue findNumber(char find, bool convertToMM = false);
  /*
   * Calculates the Feedrate
   */
  void setFeedrate(float speed);
  // As in https://de.wikipedia.org/wiki/Bresenham-Algorithmus
  void drawLine(float newx, float newy);
  /*
   * Source: https://github.com/MarginallyClever/GcodeCNCDemo
   * Source 2:
   * https://github.com/gnea/grbl/blob/master/grbl/motion_control.c#L87
   * utility routine for "drawing" a circle
   */
  void drawArc(float cx, float cy, float x, float y, StepperDirections dir);
};

#endif /* GCODEPARSER_H_ */
