// ---------------------------------------------------------------------------
// File:        Button.h
// Editors:     Robert Patzke,
// Start:       16. April 2018
// Last change: 16. April 2018
// URI/URL:     www.mfp-portal.de, homeautomation.x-api.de
// Licence:     Creative Commons CC-BY-SA
// ---------------------------------------------------------------------------
//

#ifndef _Button_H
#define _Button_H
// ----------------------------------------------------------------------------
#define useArduinoLibs

#ifdef useArduinoLibs
  #include "Arduino.h"
#endif

#define buttonIdleTime      2000
#define buttonResetTime     2500
#define buttonDetectTime    50
#define buttonClickTime     500


// ---------------------------------------------------------------------------
// class Button
// ---------------------------------------------------------------------------
//
class Button
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
  uint8_t   pinButton;

  bool      buttonDown;
  bool      buttonReset;
  bool      buttonClicked;
  bool      buttonMultClicked;

  int       buttonDownTime;
  int       buttonUpTime;

  int       buttonUpIdleTime;
  int       buttonUpDetectTime;
  int       buttonUpClickTime;
  int       buttonDownResetTime;
  int       buttonDownDetectTime;
  int       buttonClickCounter;

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
  Button();
  void  begin(uint8_t pin, int cycTime);

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
  void  run();
  int   clicks();

};

// ----------------------------------------------------------------------------
#endif //_Button_H

