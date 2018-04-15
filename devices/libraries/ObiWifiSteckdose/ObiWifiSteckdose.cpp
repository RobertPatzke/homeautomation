// ---------------------------------------------------------------------------
// File:        ObiWifiSteckdose.h
// Editors:     Robert Patzke,
// Start:       14. April 2018
// Last change: 14. April 2018
// URI/URL:     www.mfp-portal.de, homeautomation.x-api.de
// Licence:     Creative Commons CC-BY-SA
// ---------------------------------------------------------------------------
//

#include "ObiWifiSteckdose.h"

// ---------------------------------------------------------------------------
// constructors and initialisations
// ---------------------------------------------------------------------------
//

ObiWifiSteckdose::ObiWifiSteckdose(int cycTime)
{
  cycleTime = cycTime;
  frequency = 1000 / cycleTime;
}


// ---------------------------------------------------------------------------
// user functions
// ---------------------------------------------------------------------------
//
void ObiWifiSteckdose::begin()
{
#ifdef useArduinoLibs
  pinMode(button, INPUT_PULLUP);
  pinMode(pinLed, OUTPUT);
  pinMode(relayON, OUTPUT);
  pinMode(relayOff, OUTPUT);

  digitalWrite(pinLed, HIGH);
  digitalWrite(relayON, HIGH);
  digitalWrite(relayOff, HIGH);
#endif

  buttonCheckState      = bcsIdle;
  buttonUpIdleTime      = buttonIdleTime / cycleTime;
  buttonUpTime          = buttonUpIdleTime;
  buttonDownDetectTime  = buttonDetectTime / cycleTime;
  buttonUpDetectTime    = buttonDownDetectTime;
  buttonUpClickTime     = buttonClickTime / cycleTime;
  buttonDownTime        = 0;
  buttonClickCounter    = 0;
  buttonReset           = false;
}

void ObiWifiSteckdose::run()
{
  // check button
  //
  if(digitalRead(button) == LOW)
  {
    buttonDownTime++;
    buttonUpTime = 0;
  }
  else
  {
    buttonUpTime++;
    buttonDownTime = 0;
  }

  // evaluate button state
  //
  switch(buttonCheckState)
  {
    case bcsIdle:
    // ------------------------------------------------------------------------
      if(buttonUpTime > buttonUpIdleTime)
      {
        // if button is not touched
        // simply limit up time value
        //
        buttonUpTime = buttonUpIdleTime;
        break;
      }

      if(buttonDownTime > buttonDownDetectTime)
      {
        // if button is pushed
        // wait until down time is long enough
        // for marking a click of button
        //
        buttonClickCounter++;
        buttonCheckState = bcsLow;
      }
      break;

    case bcsLow:
    // ------------------------------------------------------------------------
      if(buttonDownTime > buttonDownResetTime)
      {
        // if button is pressed a long time
        // it indicates a reset
        //
        buttonReset = true;
        buttonDownTime = buttonDownResetTime;
        break;
      }

      if(buttonUpTime > buttonUpDetectTime)
      {
        // if button is released
        // wait until up time is long enough
        // for enter a new state
        //
        buttonCheckState = bcsHigh;
      }
      break;

    case bcsHigh:
    // ------------------------------------------------------------------------
      if(buttonUpTime > buttonUpClickTime)
      {
        // button is pressed only for one click
        //
        buttonClicked = true;
        buttonCheckState = bcsIdle;
        break;
      }

      if(buttonDownTime > buttonDownDetectTime)
      {
        // button is pressed again before click time
        //
        buttonMultClicked = true;
        buttonClickCounter++;
        buttonCheckState = bcsLow;
      }
      break;
  }

}

int ObiWifiSteckdose::clicks()
{
  int retv = 0;

  if(!buttonClicked)
    return(retv);

  retv = buttonClickCounter;
  buttonClickCounter = 0;
  buttonClicked = false;
  return(retv);
}
