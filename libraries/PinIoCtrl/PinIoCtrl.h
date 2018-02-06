//-----------------------------------------------------------------------------
// Topic:   Decentral home automation / smart devices
// File:    PinIoCtrl.h
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de  / homeautomation.x-api.de
//-----------------------------------------------------------------------------
// Licence: CC-BY-SA  (see Wikipedia: Creative Commons)
//

#ifndef _PinIoCtrl_h
#define _PinIoCtrl_h
//-----------------------------------------------------------------------------

#ifndef smnNoArduinoLibs
  #define smnArduino
#endif

#ifdef smnArduino
  #include "Arduino.h"
#endif

#define InfoLED     13

enum MorseCode
{
  Ignore,
  Dit,
  Dah,
  PauseDit,
  PauseDah,
  PauseWord,
  PauseLong,
  Repeat,
  Close
};

// ---------------------------------------------------------------------------
// class PinIoCtrl
// ---------------------------------------------------------------------------
//

class PinIoCtrl
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
  int       morseSmallSeq;
  boolean   doMorse;

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
  PinIoCtrl(); PinIoCtrl(int outport);
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
#endif // _PinIoCtrl_h
