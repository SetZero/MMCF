/*
 * ZAxisController.h
 *
 * Created: 2017-06-10 01:26:00
 *  Author: Sebastian
 */
#include "IZAxisController.h"
#include <stdint.h>

#ifndef ZAXISCONTROLLERI2C_H_
#define ZAXISCONTROLLERI2C_H_

/*
 * I²C Protocol for Plotter Heads:
 * 1. Master requests head type (see ZAxisControllerTypes)
 * 2. Slave will answer with 1 byte of data (-> max 255 possible Plotter Heads)
 *
 * Coordinates Communication:
 * 1. Master will send a RequestMoveZAxis, which tells the slave that the next 4
 *    bytes are one float with the ZAxis Coordinates
 * 2. Master will split one float for the coordinate communication in 4 bytes
 * 3. Slave will wait for 4 bytes to recive after he got the first request type
 *    bytes
 *
 * A total of 5 bytes will be send over I²C for a ZAxis move
 */
class ZAxisControllerI2C : public IZAxisController {
private:
  static const int ZAXISCONTROLLERADDR = 8;
  float realZPosition = 0.0;
  ZAxisControllerTypes controllerType = UnknownZType;

public:
  ZAxisControllerI2C();
  void sendZPosition(float zPosition);
  ZAxisStates sendZPositionWithAck(float zPosition);
};
#endif
