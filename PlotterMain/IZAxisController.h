
#ifndef IZAXISCONTROLLER_H_
#define IZAXISCONTROLLER_H_

enum ZAxisControllerTypes {
  UnknownZType = 0,
  ZAxisDummy = 10,    // wtf?! The first entry will cause 0 over I2C
  PlotterDrawer = 11, // weird, everything < 10 is 0 over I2C?!
  LaserEngraver = 12,
  Printer3D = 13
};

enum ZAxisStates { UnknownZState = 0, ZStateFinished = 1, ZStateFailed = 2 };
enum RequestTypes { RequestControllerType = 1, RequestMoveZAxis = 2 };

class IZAxisController {
public:
  virtual void sendZPosition(float zPosition) = 0;
  virtual ZAxisStates sendZPositionWithAck(float zPosition) = 0;
};

#endif
