#include "Arduino.h"

#include "LoopCheck.h"
#include "ObiWifiSteckdose.h"

LoopCheck   loopCheck;

#define sdCycleTime 5
ObiWifiSteckdose    obiSd(sdCycleTime);

void setup()
{
  Serial.begin(115200);
  obiSd.begin();
}

// The loop function is called in an endless loop
void loop()
{
  loopCheck.begin();
  // --------------------------------------------------------------------------

  if(loopCheck.once(0, 1000))
  {
    obiSd.setBlinkRelay(500, 500, 10);
    obiSd.ledBlue->blink(900, 100);
  }

  if(loopCheck.timerMilli(0, sdCycleTime, 0))
    obiSd.run();

  if(loopCheck.timerMilli(1, 1000, 0))
    Serial.println(loopCheck.refDateTime());

  if(loopCheck.timerMilli(2, 100, 0))
  {
    int nrClicks = obiSd.getButtonClicks();
    if(nrClicks > 0)
      Serial.println(nrClicks);
    else if(nrClicks < 0)
      Serial.println("Reset");
  }

  if(loopCheck.timerMilli(3, 20000, 0))
  {
    Serial.print("Relay switch ");
    if(loopCheck.toggle(0))
    {
      obiSd.setRelayOn();
      Serial.println("ON");
    }
    else
    {
      obiSd.setRelayOff();
      Serial.println("OFF");
    }
  }

  // --------------------------------------------------------------------------
  loopCheck.end();
}
