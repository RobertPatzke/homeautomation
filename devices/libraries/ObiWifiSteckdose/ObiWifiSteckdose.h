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

#define pinLed              4
#define relayON             12
#define relayOff            5
#define button              14

#define buttonIdleTime      2000
#define buttonResetTime     2000
#define buttonDetectTime    50
#define buttonClickTime     1000

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
  enum ButtonCheckState
  {
    bcsIdle,    // button is up (for long time)
    bcsLow,     // button is down (counting)
    bcsHigh,    // button is up (counting)
    bcsNrOfStates
  };

private:
  // -------------------------------------------------------------------------
  // local variables
  // -------------------------------------------------------------------------
  //
  bool relayBlink;
  bool ledBlink;
  bool buttonDown;
  bool buttonReset;
  bool buttonClicked;
  bool buttonMultClicked;

  int  buttonDownTime;
  int  buttonUpTime;

  int  buttonUpIdleTime;
  int  buttonUpDetectTime;
  int  buttonUpClickTime;
  int  buttonDownResetTime;
  int  buttonDownDetectTime;
  int  buttonClickCounter;

  ButtonCheckState  buttonCheckState;

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

  // -------------------------------------------------------------------------
  // user functions
  // -------------------------------------------------------------------------
  //
  void  begin();
  void  run();
  int   clicks();

};

// ----------------------------------------------------------------------------
#endif //_ObiWifiSteckdose_H
