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

// Test for type definitions of SAM3X
//
#ifndef PIO_PER_P0
  #define Pio void
#endif

#include "environment.h"

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

typedef struct
{
  Pio       *pioPtr;
  uint32_t  mask;
} PioDescr, *ptrPioDescr;


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
  bool      outPortSet, outPortON;
  bool      cpl;
  bool      perInit;
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

  int       chkInSet;
  int       chkInCnt;
  uint32_t  chkInVal;

  bool      chkInDuration;
  int       inDurationCnt;
  int       inDurationLim;
  uint32_t  inDurVal;

  PioDescr  pioOutDescr;
  PioDescr  pioInDescr;


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
  PinIoCtrl(); PinIoCtrl(int frequency); PinIoCtrl(int frequency, int outport);
  PinIoCtrl(int frequency, PioDescr pioData);
  int initPerif(); int initPerif(int port); int initPerif(PioDescr pioData);
  void init(int port); void init(Pio *pio, uint32_t portMask);

  // -------------------------------------------------------------------------
  // user functions
  // -------------------------------------------------------------------------
  //
  void  run();                      // has to be cyclic called with <frequency>
  void  flash(int len);             // Flash Info LED for <len> milliseconds
  void  flashMin(int addCycle);     // Flash Pin for (1 + addCycle) cycles

  void  blink(int len, int pause);
  void  blink(int len, int pause, bool chkBusy);
  // Blink pin for <len> milliseconds ON and pause OFF (use invert() if LED)
  // do not set values if blinking is active with <chkBusy>

  void  invert();                   // Invert output level (LOW/HIGH)
  int   dimm(double damp, boolean sim);           // Set intensity of Info LED
  void  turn(boolean onOff);        // Switch Info LED on or off
  void  sos(boolean repeat);        // Start morsing SOS
  bool  inDigLevel(int port, int highLow, int periodTime); // check stay input
  bool  inDigLevel(PioDescr pioData, uint32_t highLow, int periodTime, int perId);
  void  watchDigLevel(PioDescr pioData, int periodTime, int perId);
  bool  stableDigLevel(uint32_t highLow);
  bool  getDigLevel(uint32_t *highLow);
};

//-----------------------------------------------------------------------------
#endif // _PinIoCtrl_h
