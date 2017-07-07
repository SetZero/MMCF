/*
 * Plotter.ino
 *
 * Created: 2017-05-13 20:24:48
 * Author : Sebastian
 */
#include "DS3231.h"
#include "GCodeParser.h"
#include "LibWiFi.h"
#include "settings.h"
#include <Arduino.h>
#include <Wire.h>

// Create an unique pointer to PlotterInterpreter
std::unique_ptr<PlotterInterpreter<ZAxisControllerCAN>> PlotterI;

void setup() {
  // Initialize serial
  Serial.begin(115200);

  // Fancy Output Stuff
  Serial.print("\n\r\n\n+--------------------------------+\r\n");
  Serial.print("|                                |\r\n");
  Serial.print("|     Welcome to my Printer!     |\r\n");
  if (COMMUNICATION_INTERFACE == I2C_BUS)
    Serial.print("|          I2C VERSION           |\r\n");
  else if (COMMUNICATION_INTERFACE == CAN_BUS)
    Serial.print("|          CAN VERSION           |\r\n");
  Serial.print("|                                |\r\n");
  Serial.print("+--------------------------------+\r\n");

  // If we are Communicating over I2C we can use our RTC
  if (COMMUNICATION_INTERFACE == I2C_BUS) {
    Wire.begin();
    DS3231 rtc;
    rtc.synchonizeTime(); // We want to be sure that we are using the right time

    std::tm t;
    t = rtc.getTime();
    // Fancy output
    Serial.printf("Time: %02d:%02d:%02d, %02d.%02d.%04d\n\r", t.tm_hour,
                  t.tm_min, t.tm_sec, t.tm_mday, t.tm_mon, t.tm_year);

    // Create Instance of PlotterInterpreter with a I2C Bus
    // PlotterI = new PlotterInterpreter<ZAxisControllerI2C>();

  } else if (COMMUNICATION_INTERFACE == CAN_BUS) {
    // Create an Instance of Plotter Interpreter with a CAN Bus
    PlotterI = std::make_unique<PlotterInterpreter<ZAxisControllerCAN>>();
  }
}

// Start by reading a line, this will also manage the rest
void loop() { PlotterI->readLine(); }
