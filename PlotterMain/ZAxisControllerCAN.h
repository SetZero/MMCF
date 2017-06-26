/*
 * ZAxisControllerCAN.h
 *
 * Created: 2017-06-10 01:26:00
 *  Author: Sebastian
 */
#include "IZAxisController.h"
#include <mcp_can.h>
#include <stdint.h>

#ifndef ZAXISCONTROLLERCAN_H_
#define ZAXISCONTROLLERCAN_H_

/*
 * *Who, What, How Protocol
 *   ID      1   2  3  4   5   6   7   8
 * +------------------------------------+
 * | Who | What | How                   |
 * +------------------------------------+
 * Who, Address From Sender, 0x01 = Master, 0x02 = Slave
 * What, Command (See Header of Interface)
 * How, Data bytes
 * 
 * CAN Protocol for Plotter Heads:
 * 1. Master (Sender with ID 0x01) requests head type (see ZAxisControllerTypes)
 * 2. Slave (sender with id != 0x01) will answer with its ID. Data bytes will be 0x00
 *
 * Coordinates Communication:
 * 1. Master will send a RequestMoveZAxis, which tells the slave that the next 4
 *    bytes are one float with the ZAxis Coordinates, the remaining 3 bytes are 0x00
 * 2. Master will split one float for the coordinate communication in 4 bytes
 * 3. Slave will read the bytes 2-5 (4bytes) to regain the float value
 *
 * A total of 5 data bytes and 3 stuff bytes will be send over CAN for a ZAxis move
 */
class ZAxisControllerCAN : public IZAxisController {
private:
  long unsigned int rxId;
  unsigned char len = 0;
  unsigned char rxBuf[8];
  MCP_CAN CAN0{2};                // CS on pin 10
  const static int CAN0_INT = 15; // Interrupt Pin on Pin 10
  const static int MASTER_ID = 0x01;
  float realZPosition = 0.0;
  ZAxisControllerTypes controllerType = UnknownZType;

public:
  ZAxisControllerCAN();
  /*
   * Send the Z-Axis Position
   */
  void sendZPosition(float zPosition);
  /*
   * We don't really need a function with ACK, as CAN is controlling the data flow, but
   * this function is needed for other Communications types which don't support this
   * kind of data control
   */
  ZAxisStates sendZPositionWithAck(float zPosition);
};
#endif
