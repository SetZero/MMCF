/*
 * Plotter.cpp
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

PlotterInterpreter *PlotterI;

void setup() {
  // Initialize serial and wait for port to open:
  Serial.begin(115200);

  Serial.print("\n\r\n\n+--------------------------------+\r\n");
  Serial.print("|                                |\r\n");
  Serial.print("|     Welcome to my Printer!     |\r\n");
#if COMMUNICATION_INTERFACE == I2C_BUS
  Serial.print("|          I2C VERSION           |\r\n");
#elif COMMUNICATION_INTERFACE == CAN_BUS
  Serial.print("|          CAN VERSION           |\r\n");
#endif
  Serial.print("|                                |\r\n");
  Serial.print("+--------------------------------+\r\n");

#if COMMUNICATION_INTERFACE == I2C_BUS
  Wire.begin();
  DS3231 rtc;
  rtc.synchonizeTime();

  std::tm t;
  t = rtc.getTime();
  Serial.printf("Time: %02d:%02d:%02d, %02d.%02d.%04d\n\r", t.tm_hour, t.tm_min,
                t.tm_sec, t.tm_mday, t.tm_mon, t.tm_year);

#endif

  PlotterI = new PlotterInterpreter();
}

void loop() { PlotterI->readLine(); }
