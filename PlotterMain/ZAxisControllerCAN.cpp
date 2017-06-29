/*
 * ZAxisController.cpp
 *
 * Created: 2017-06-10 01:26:00
 *  Author: Sebastian
 */
#include "IZAxisController.h"
#include "ZAxisControllerCAN.h"
#include <Arduino.h>
#include <mcp_can.h>

ZAxisControllerCAN::ZAxisControllerCAN() {
  Serial.println("Waiting for Plotter Head (CAN) to answer... ");
  // Controller with 8Mhz crital, 100KBPS rate
  if (CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK)
    Serial.println("MCP2515 Initialized Successfully!");
  else
    Serial.println("Error Initializing MCP2515...");
  CAN0.setMode(MCP_NORMAL);
  pinMode(CAN0_INT, INPUT);

  byte data[8] = {static_cast<byte>(RequestControllerType),
                  0x00,
                  0x00,
                  0x00,
                  0x00,
                  0x00,
                  0x00,
                  0x00};
  byte sndStat = CAN0.sendMsgBuf(MASTER_ID, 0, 8, data);
  while (digitalRead(CAN0_INT)) {
    byte sndStat = CAN0.sendMsgBuf(MASTER_ID, 0, 8, data);
    if (sndStat == CAN_OK) {
      Serial.println("Successfully waiting for a Plotter Head!");
    } else {
      Serial.println("Error Sending Data!");
    }
    delay(5);
  }
  CAN0.readMsgBuf(&rxId, &len, rxBuf);
  if (len < 3)
    Serial.print("Malformed Package");
  controllerType = static_cast<ZAxisControllerTypes>(rxId);
  Serial.printf(
      "\nI found the following controller for our Z-Axis(0x%.3lX / %d) :", rxId,
      (int)controllerType);
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

void ZAxisControllerCAN::sendZPosition(float zPosition) {
  Serial.printf("Sending %f over  CAN\n\r", zPosition);
  uint8_t *wirePosition = reinterpret_cast<uint8_t *>(&zPosition);
  byte data[8] = {static_cast<byte>(RequestMoveZAxis),
                  wirePosition[0],
                  wirePosition[1],
                  wirePosition[2],
                  wirePosition[3],
                  0x00,
                  0x00,
                  0x00};
  byte sndStat = CAN0.sendMsgBuf(MASTER_ID, 0, 8, data);
  if (sndStat != CAN_OK) {
    delay(5);
    sendZPosition(zPosition);
  }
}
ZAxisStates ZAxisControllerCAN::sendZPositionWithAck(float zPosition) {
  sendZPosition(zPosition);
  delay(5);
  // Wire.requestFrom(ZAXISCONTROLLERADDR, 1);
  ZAxisStates state = UnknownZState;
  /*while (Wire.available()) {
    state = static_cast<ZAxisStates>(Wire.read());
    Serial.printf("Answer: %d\n", (int)state);
  }*/
  return state;
}
