//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   GpioCtrl.cpp
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (siehe Wikipedia: Creative Commons)
//

#include "GpioCtrl.h"

// --------------------------------------------------------------------------
// Konstruktoren
// --------------------------------------------------------------------------
//

GpioCtrl::GpioCtrl(IntrfGpio *inGpioPtr, int inPeriod)
{
  gpioPtr = inGpioPtr;
  period  = inPeriod;
}

// --------------------------------------------------------------------------
// Konfigurationen
// --------------------------------------------------------------------------
//



// --------------------------------------------------------------------------
// Anwendungsfunktionen allgemein
// --------------------------------------------------------------------------
//
void GpioCtrl::run()
{
  BlinkPtr blinkPtr;

  for(int i = 0; i < MaxGpioElements; i++)
  {
    // -----------------------------------------------------------------------
    // run blink
    // -----------------------------------------------------------------------
    //
    blinkPtr = &blinkEl[i];

    if(blinkPtr->doBlink && (blinkPtr->portRef != NULL))
    {
    // Blinken wird aktiviert/deaktiviert
    // --------------------------------------------------------
      if(!blinkPtr->blinked)    // Noch kein Blinken
      {
        if(!blinkPtr->portSet)  // Port noch nicht gesetzt
        {
          gpioPtr->set(blinkPtr->portRef);
          blinkPtr->portSet = true;
        }

        if(blinkPtr->blinkLen > 0)
          blinkPtr->blinkLen--;
        else
        {
          blinkPtr->blinked = true;
          blinkPtr->blinkLen = blinkPtr->blinkLenSet;
        }
      }
      else // blinked
      {
        if(blinkPtr->portSet)   // Port ist gesetzt
        {
          gpioPtr->clr(blinkPtr->portRef);
          blinkPtr->portSet = false;
        }

        if(blinkPtr->blinkPause > 0)
          blinkPtr->blinkPause--;
        else
        {
          blinkPtr->blinked = false;
          blinkPtr->blinkPause = blinkPtr->blinkPauseSet;
        }
      }
    // --------------------------------------------------------
    } // doBlink, portRef
  } // for
} // run

// --------------------------------------------------------------------------
// Anwendungsfunktionen Steuerung der Pins
// --------------------------------------------------------------------------
//

void  GpioCtrl::blink(GpioExtRefPtr portRef, int chn, int len, int pause)
{
  BlinkPtr  blinkPtr;
  int       calc;

  if(chn < 0 || chn >= MaxGpioElements) return;
  if(len <= 0) return;
  if(period == 0) return;

  blinkPtr = &blinkEl[chn];
  blinkPtr->portRef = portRef;

  calc = (1000 * len) / period;
  if(calc == 0)
    calc = 1;
  blinkPtr->blinkLen = blinkPtr->blinkLenSet = calc;

  calc = (1000 * pause) / period;
  if(calc == 0)
    calc = 1;
  blinkPtr->blinkPause = blinkPtr->blinkPauseSet = calc;

  blinkPtr->blinked   = false;
  blinkPtr->doBlink   = true;
  blinkPtr->portSet   = false;
}


// ----------------------------------------------------------------------------
// Ereignisbearbeitung und Interrupts
// ----------------------------------------------------------------------------
//
