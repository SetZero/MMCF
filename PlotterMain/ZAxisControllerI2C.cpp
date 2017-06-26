/*
 * ZAxisController.cpp
 *
 * Created: 2017-06-10 01:26:00
 *  Author: Sebastian
 */
#include "IZAxisController.h"
#include "ZAxisControllerI2C.h"
#include <Arduino.h>
#include <Wire.h>

ZAxisControllerI2C::ZAxisControllerI2C() {
  Serial.println("Waiting for Plotter Head (I2C) to answer... ");
  // First contact to our controller
  Wire.beginTransmission(ZAXISCONTROLLERADDR);
  Wire.write(static_cast<int>(RequestControllerType));
  Wire.endTransmission();
  delay(10);
  Wire.requestFrom(ZAXISCONTROLLERADDR,
                   1); // request 6 bytes from slave device #8

  if (Wire.available()) {
    int c = Wire.read(); // receive a byte as character
    Serial.print(c);     // print the character
    controllerType = static_cast<ZAxisControllerTypes>(c);
    Serial.print("\nI found the following controller for our Z-Axis:");
  }
  // What type of controller have we contacted right now?

  switch (controllerType) {
  case UnknownZType:
    Serial.println("Unknown");
    break;
  case PlotterDrawer:
    Serial.println("A Pen Drawer!");
    break;
  case LaserEngraver:
    Serial.println("A Laser Engraver!");
    break;
  case Printer3D:
    Serial.println("A 3D Printer Head!");
    break;
  default:
    Serial.println("None");
  }
}

void ZAxisControllerI2C::sendZPosition(float zPosition) {
  Serial.printf("Sending %f over  I2C\n\r", zPosition);
  uint8_t *wirePosition = reinterpret_cast<uint8_t *>(&zPosition);

  Wire.beginTransmission(ZAXISCONTROLLERADDR);
  Wire.write(static_cast<int>(RequestMoveZAxis));
  Wire.write(wirePosition, sizeof(float));
  Wire.endTransmission();
}
ZAxisStates ZAxisControllerI2C::sendZPositionWithAck(float zPosition) {
  sendZPosition(zPosition);
  delay(5);
  Wire.requestFrom(ZAXISCONTROLLERADDR, 1);
  ZAxisStates state = UnknownZState;
  while (Wire.available()) {
    state = static_cast<ZAxisStates>(Wire.read());
    Serial.printf("Answer: %d\n", (int)state);
  }
  return state;
}
