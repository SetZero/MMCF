
#ifndef IZAXISCONTROLLER_H_
#define IZAXISCONTROLLER_H_

enum ZAxisControllerTypes {
  UnknownZType = 0, // error prevention for some Communication Interfaces
  // Keep this clear for further error prevention
  ZAxisDummy = 10,    // Test Head for Developement
  PlotterDrawer = 11, // A drawer Head with a Pencil
  LaserEngraver = 12, // A Laser Engraver with a Laser Head (not dangerous)
  Printer3D = 13      // A 3D Printer Head with an extruder and a heater
};

// Z Control States, currently only needed if Communicating over I2C
enum ZAxisStates { UnknownZState = 0, ZStateFinished = 1, ZStateFailed = 2 };
// Request Protocol, this will be send out to the head
enum RequestTypes { RequestControllerType = 1, RequestMoveZAxis = 2 };

class IZAxisController {
public:
  /*
   * This will send the RequestType "RequestMoveZAxis" and a float for the
   * position to move to
   */
  virtual void sendZPosition(float zPosition) = 0;
  /*
  * This will force the Head to send an Ack
  */
  virtual ZAxisStates sendZPositionWithAck(float zPosition) = 0;
  virtual ~IZAxisController() {}
};

#endif
