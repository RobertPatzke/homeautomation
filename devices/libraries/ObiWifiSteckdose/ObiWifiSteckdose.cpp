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
  pinMode(pinLed, OUTPUT);
  pinMode(relayON, OUTPUT);
  pinMode(relayOff, OUTPUT);

  digitalWrite(pinLed, HIGH);
  digitalWrite(relayON, HIGH);
  digitalWrite(relayOff, HIGH);
#endif

  button.begin(pinButton, cycleTime);
}

void ObiWifiSteckdose::run()
{
  button.run();
}

