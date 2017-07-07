# MMCF
MMCF (short for Modern Motion Control Firmware) is a 2D/3D Printing Libary for the ESP32 written in C++14 (This might change later on to C++17).
We currently Plan on adding more features over the time, but here is a list of currently supported Gcodes:

```
 G0 / G1 (linear movement)
 G2 / G3 (arc movement)
 G4 (pause)
 G20 / G21 (INCH / CM) Inch is currently untested
 G28 (Homeing to X, Y, Z)
 G90 / G91 (relative / absolute mode)
 
 M2 / M18 / M84 (currently only stops all Stepper Motors
 M100 (custom help text)
 M112 (same as M2)
 M114 (prints x,y,z coordinates)
 
 X, Y, Z, F, I, J (E will be added later on, F is currently WIP, but should be working ok)
```
