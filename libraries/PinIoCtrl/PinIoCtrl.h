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

#define InfoLED         13
#define smnMaxMorseLen  256

#define ON      true
#define OFF     false

#ifndef PWMRANGE
  #define PWMRANGE 255
#endif

enum MorseCode
{
  mcIgnore,
  mcDit,
  mcDah,
  mcPauseSig,
  mcPauseChar,
  mcPauseWord,
  mcPauseLong,
  mcRepeat,
  mcClose
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
  bool      outPortSet, outPortON;
  bool      doFlash, flashed;
  int       flashLen;
  bool      doBlink, blinked;
  int       blinkLen, blinkLenSet;
  int       blinkPause, blinkPauseSet;
  int       currentFreq;
  int       dimmVal, dimmCount;
  bool      dimmed, simulatedDimm;
  int       ditLen, morseCount;
  int       morseSeqIdx;
  byte      morseSequence[smnMaxMorseLen];
  bool      doMorse;
  int       chkInPort;
  int       chkInSet;
  int       chkInCnt;
  int       chkInVal;

  // -------------------------------------------------------------------------
  // local functions/methods
  // -------------------------------------------------------------------------
  //
  int getMorseChar(char chr, byte *buffer);

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
  void  blink(int len, int pause);
  // Blink Info LED for <len> milliseconds ON and pause OFF
  int   dimm(double damp, boolean sim);           // Set intensity of Info LED
  void  turn(boolean onOff);        // Switch Info LED on or off
  void  sos(boolean repeat);        // Start morsing SOS
  bool  inDigLevel(int port, int highLow, int periodTime); // check stay input

};

//-----------------------------------------------------------------------------
#endif // _PinIoCtrl_h
