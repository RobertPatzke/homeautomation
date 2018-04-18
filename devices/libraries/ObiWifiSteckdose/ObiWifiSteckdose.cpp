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
// user system functions
// ---------------------------------------------------------------------------
//
void ObiWifiSteckdose::begin()
{
#ifdef useArduinoLibs
  pinMode(pinLed, OUTPUT);
  pinMode(pinRelayOn, OUTPUT);
  pinMode(pinRelayOff, OUTPUT);

  digitalWrite(pinLed, HIGH);
  digitalWrite(pinRelayOn, HIGH);
  digitalWrite(pinRelayOff, HIGH);
#endif

  relayOn       = false;
  relayStateOn  = false;
  relayToggleOn = false;
  relayBlink    = false;

  button.begin(pinButton, cycleTime);

  ledBlue = new PinIoCtrl(1000 / cycleTime, pinLed);
}

void ObiWifiSteckdose::run()
{
  // check the button state via library class Button
  //
  button.run();

  // run task of pin control
  //
  ledBlue->run();

  // switch relay ON if relayOn bit is set and relay is off
  //
  if(relayOn && !relayStateOn)
  {
    if(!relayToggleOn)
    {
#ifdef useArduinoLibs
      digitalWrite(pinRelayOn, LOW);
#endif
      relayToggleOn = true;
    }
    else
    {
#ifdef useArduinoLibs
      digitalWrite(pinRelayOn, HIGH);
#endif
      relayToggleOn = false;
      relayStateOn  = true;
    }
  }

  // switch relay OFF if relayOn bit is reset and relay is on
  //
  if(!relayOn && relayStateOn)
  {
    if(!relayToggleOn)
    {
#ifdef useArduinoLibs
      digitalWrite(pinRelayOff, LOW);
#endif
      relayToggleOn = true;
    }
    else
    {
#ifdef useArduinoLibs
      digitalWrite(pinRelayOff, HIGH);
#endif
      relayToggleOn = false;
      relayStateOn  = false;
    }
  }

  // blinking of relay
  // is the last activity in run(), using return to leave
  //
  if(relayBlink)
  {
    if(relayOn)
    {
      if(blinkCounter > 0)
      {
        blinkCounter--;
        return;
      }

      relayOn = false;
      blinkCounter = blinkOffTime;
    }
    else
    {
      if(blinkCounter > 0)
      {
        blinkCounter--;
        return;
      }

      if(blinkRepeatCounter != 1)
      {
        relayOn = true;
        blinkCounter = blinkOnTime;
        if(blinkRepeatCounter > 1)
          blinkRepeatCounter--;
      }
      else
        relayBlink = false;
    }
  }

}

// ---------------------------------------------------------------------------
// user control and measure functions
// ---------------------------------------------------------------------------
//

int ObiWifiSteckdose::getButtonClicks()
{
  return(button.clicks());
}

void  ObiWifiSteckdose::setBlinkRelay(int onTime, int offTime, int repeat)
{
  if(onTime < 50)  onTime  = 50;
  if(offTime < 50) offTime = 50;

  blinkOnTime           = onTime / cycleTime;
  blinkOffTime          = offTime / cycleTime;
  blinkRepeatCounter    = repeat;

  relayOn       = true;
  relayBlink    = true;
  blinkCounter  = blinkOnTime;
}

void ObiWifiSteckdose::setRelayOn()
{
  relayOn = true;
}

void ObiWifiSteckdose::setRelayOff()
{
  relayOn = false;
}

