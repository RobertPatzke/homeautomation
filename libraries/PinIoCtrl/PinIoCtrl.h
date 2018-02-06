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

#define PORT_LOW    \
        #ifdef smnArduino \
        digitalWrite(outPort, LOW); \
        #else \
        alternativly set port/register direct \
        #endif

#define PORT_HIGH    \
        #ifdef smnArduino \
        digitalWrite(outPort, HIGH); \
        #else \
        alternativly set port/register direct \
        #endif \


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
  boolean   outPortSet, outPortON;
  boolean   doFlash, flashed;
  int       flashLen;
  int       currentFreq;
  int       dimmVal, dimmCount;
  boolean   dimmed, simulatedDimm;
  int       ditLen, morseCount;
  int       morseSeqIdx;
  byte      morseSequence[smnMaxMorseLen];
  boolean   doMorse;

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
  int   dimm(double damp, boolean sim);           // Set intensity of Info LED
  void  turn(boolean onOff);        // Switch Info LED on or off
  void  sos(boolean repeat);        // Start morsing SOS

};

//-----------------------------------------------------------------------------
#endif // _PinIoCtrl_h
