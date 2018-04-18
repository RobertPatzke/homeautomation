// ---------------------------------------------------------------------------
// File:        ObiWifiSteckdose.h
// Editors:     Robert Patzke,
// Start:       14. April 2018
// Last change: 14. April 2018
// URI/URL:     www.mfp-portal.de, homeautomation.x-api.de
// Licence:     Creative Commons CC-BY-SA
// ---------------------------------------------------------------------------
//

#ifndef _ObiWifiSteckdose_H
#define _ObiWifiSteckdose_H
// ----------------------------------------------------------------------------
#define useArduinoLibs

#ifdef useArduinoLibs
  #include "Arduino.h"
#endif

#include "Button.h"
#include "PinIoCtrl.h"

#define pinLed               4
#define pinRelayOn          12
#define pinRelayOff          5
#define pinButton           14


// ---------------------------------------------------------------------------
// class ObiWifiSteckdose
// ---------------------------------------------------------------------------
//
class ObiWifiSteckdose
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
  Button    button;
  bool      relayBlink;
  bool      ledBlink;
  bool      relayToggleOn;
  bool      relayStateOn;
  bool      relayOn;

  int       blinkCounter;
  int       blinkOnTime;
  int       blinkOffTime;
  int       blinkRepeatCounter;

  // -------------------------------------------------------------------------
  // local functions
  // -------------------------------------------------------------------------
  //

public:
  // -------------------------------------------------------------------------
  // constructors and initialisations
  // -------------------------------------------------------------------------
  //
  ObiWifiSteckdose(int cycleTime);

  // -------------------------------------------------------------------------
  //  global variables
  // -------------------------------------------------------------------------
  //
  int frequency;
  int cycleTime;

  PinIoCtrl *ledBlue;

  // -------------------------------------------------------------------------
  // user system functions
  // -------------------------------------------------------------------------
  //
  void  begin();
  void  run();

  // -------------------------------------------------------------------------
  // user control and measure functions
  // -------------------------------------------------------------------------
  //
  int   getButtonClicks();
  void  setBlinkRelay(int onTime, int offTime, int repeat);
  void  setRelayOn(); void setRelayOff();

};

// ----------------------------------------------------------------------------
#endif //_ObiWifiSteckdose_H