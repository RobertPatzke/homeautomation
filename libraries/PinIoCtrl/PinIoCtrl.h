//-----------------------------------------------------------------------------
// Topic:   Decentral home automation / smart devices
// File:    Sonoff.h
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de  / homeautomation.x-api.de
//-----------------------------------------------------------------------------
// Licence: CC-BY-SA  (see Wikipedia: Creative Commons)
//

#ifndef _Sonoff_h
#define _Sonoff_h
//-----------------------------------------------------------------------------

#ifndef smnNoArduinoLibs
  #define smnArduino
#endif

#ifdef smnArduino
  #include "Arduino.h"
#endif

#define InfoLED     13

// ---------------------------------------------------------------------------
// class Sonoff
// ---------------------------------------------------------------------------
//

class Sonoff
{
  // -------------------------------------------------------------------------
  // class specific data types
  // -------------------------------------------------------------------------
  //

private:
  // -------------------------------------------------------------------------
  // local variables
  // -------------------------------------------------------------------------
  //
  int       outPort;
  boolean   outPortSet, outPortON;
  boolean   doFlash, flashed;
  int       flashLen;
  int       currentFreq;
  int       dimmVal, dimmCount;
  boolean   dimmed, simulatedDimm;
  int       ditLen, morseCount;

  // -------------------------------------------------------------------------
  // local functions/methods
  // -------------------------------------------------------------------------
  //

public:
  // -------------------------------------------------------------------------
  // public variables
  // -------------------------------------------------------------------------
  //

  // -------------------------------------------------------------------------
  // constructors and initialisations
  // -------------------------------------------------------------------------
  //
  Sonoff(); Sonoff(int outport);
  int initPerif(); void init(int port);

  // -------------------------------------------------------------------------
  // user functions
  // -------------------------------------------------------------------------
  //
  void  run(int frequency);         // has to be cyclic called with <frequency>
  void  flash(int len);             // Flash Info LED for <len> milliseconds
  int   dimm(double damp, boolean sim);           // Set intensity of Info LED
  void  turn(boolean onOff);        // Switch Info LED on or off

};

//-----------------------------------------------------------------------------
#endif // _Sonoff_h
