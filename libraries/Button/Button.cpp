// ---------------------------------------------------------------------------
// File:        Button.h
// Editors:     Robert Patzke,
// Start:       14. April 2018
// Last change: 14. April 2018
// URI/URL:     www.mfp-portal.de, homeautomation.x-api.de
// Licence:     Creative Commons CC-BY-SA
// ---------------------------------------------------------------------------
//

#include "Button.h"

// ---------------------------------------------------------------------------
// constructors and initialisations
// ---------------------------------------------------------------------------
//

Button::Button()
{
}

void Button::begin(uint8_t pin, int cycTime)
{
#ifdef useArduinoLibs
  pinMode(pin, INPUT_PULLUP);
#endif

  pinButton = pin;
  cycleTime = cycTime;
  frequency = 1000 / cycleTime;

  buttonCheckState      = bcsIdle;
  buttonUpIdleTime      = buttonIdleTime / cycleTime;
  buttonUpTime          = buttonUpIdleTime;
  buttonDownResetTime   = buttonResetTime / cycleTime;
  buttonDownDetectTime  = buttonDetectTime / cycleTime;
  buttonUpDetectTime    = buttonDownDetectTime;
  buttonUpClickTime     = buttonClickTime / cycleTime;
  buttonDownTime        = 0;
  buttonClickCounter    = 0;
  buttonReset           = false;

}

// ---------------------------------------------------------------------------
// user functions
// ---------------------------------------------------------------------------
//

void Button::run()
{
  // check button
  //
  if(digitalRead(pinButton) == LOW)
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
  } // switch


}


int Button::clicks()
{
  int retv = 0;

  if(!buttonClicked)
    return(retv);

  if(buttonReset)
    retv = -1;
  else
    retv = buttonClickCounter;

  buttonClickCounter = 0;
  buttonClicked = false;
  buttonMultClicked = false;
  buttonReset = false;

  return(retv);
}

