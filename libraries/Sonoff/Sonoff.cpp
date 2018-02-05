//-----------------------------------------------------------------------------
// Topic:   Decentral home automation / smart devices
// File:    Sonoff.cpp
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de  / homeautomation.x-api.de
//-----------------------------------------------------------------------------
// Licence: CC-BY-SA  (see Wikipedia: Creative Commons)
//
#include "Sonoff.h"

// ---------------------------------------------------------------------------
// Constructors and initialisations
// ---------------------------------------------------------------------------
//

Sonoff::Sonoff()
{
#ifdef smnArduino

#endif

  init(InfoLED);
}

Sonoff::Sonoff(int outport)
{
#ifdef smnArduino

#endif

  init(outport);
}

void Sonoff::init(int port)
{
  outPort       = port;
  doFlash       = false;
  outPortSet    = false;
  flashLen      = 2;
}


// ---------------------------------------------------------------------------
// Initialisation
// ---------------------------------------------------------------------------
// This nitialisation is not part of the constructor, because we may call other
// resources which may return errors or throw exceptions
//
int Sonoff::initPerif()
{
  int retv = 0;

#ifdef smnArduino
  pinMode(outPort, OUTPUT);
  digitalWrite(outPort, HIGH);
#else
   here may be direct port/register access used
#endif

  return(retv);
}

// ---------------------------------------------------------------------------
// run()        cooperative threading
// ---------------------------------------------------------------------------
// This function has to be called cyclic, parameter is the calling frequency
//
void Sonoff::run(int frequency)
{
  currentFreq = frequency;

  // -------------------------------------------------------------------------
  // run flash
  // -------------------------------------------------------------------------
  //
  if(doFlash)
  {
    if(!flashed)
    {
      if(!outPortSet)
      {
        #ifdef smnArduino
        digitalWrite(outPort, LOW);
        #else
        alternativly set port/register direct
        #endif
        outPortSet = true;
      }
      if(flashLen > 0)
        flashLen--;
      else
        flashed = true;
    }
    else
    {
      doFlash       = false;
      flashed       = false;
      outPortSet    = false;
      #ifdef smnArduino
      digitalWrite(outPort, HIGH);
      #else
      alternativly set port/register direct
      #endif
    }
  }

  // -------------------------------------------------------------------------
  // run dimmer simulation
  // -------------------------------------------------------------------------
  //
  if(simulatedDimm)
  {
    if(outPortSet)
    {
      #ifdef smnArduino
      digitalWrite(outPort, HIGH);
      #else
      alternativly set port/register direct
      #endif
      outPortSet = false;
    }

    if(dimmCount > 0)
      dimmCount--;
    else
    {
      #ifdef smnArduino
      digitalWrite(outPort, LOW);
      #else
      alternativly set port/register direct
      #endif
      outPortSet = true;

      dimmCount = dimmVal;
    }
  }


  // -------------------------------------------------------------------------
} // end run()

// ---------------------------------------------------------------------------
// flash()              start flashing the info LED
// ---------------------------------------------------------------------------
//
void Sonoff::flash(int len)
{
  int calc;

  calc = (currentFreq * len) / 1000;
  if(calc == 0) calc = 1;
  flashLen = calc;
  doFlash = true;
}

// ---------------------------------------------------------------------------
// dimm()               set intensity of info LED
// ---------------------------------------------------------------------------
// Not all Pins support PWM. Set <sim> for simulation
//
int Sonoff::dimm(double damp, boolean sim)
{
  if(sim)
  {
    if(damp < 0.002) damp = 0.002;
    dimmVal = (int) (1 / damp);
    dimmCount = 0;
    outPortSet = false;
    simulatedDimm = true;
  }
  else
  {
    dimmVal = (int) (damp * 255);

    if(dimmVal == 0)
      dimmVal = 1;

    #ifdef smnArduino
    analogWrite(outPort, dimmVal);
    #else
    alternativly set port/register direct
    #endif
  }

  dimmed = true;
  return(dimmVal);
}
