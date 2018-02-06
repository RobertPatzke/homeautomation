//-----------------------------------------------------------------------------
// Topic:   Decentral home automation / smart devices
// File:    PinIoCtrl.cpp
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de  / homeautomation.x-api.de
//-----------------------------------------------------------------------------
// Licence: CC-BY-SA  (see Wikipedia: Creative Commons)
//
#include "PinIoCtrl.h"

// ---------------------------------------------------------------------------
// Constructors and initialisations
// ---------------------------------------------------------------------------
//

PinIoCtrl::PinIoCtrl()
{
#ifdef smnArduino

#endif

  init(InfoLED);
}

PinIoCtrl::PinIoCtrl(int outport)
{
#ifdef smnArduino

#endif

  init(outport);
}

void PinIoCtrl::init(int port)
{
  outPort       = port;
  doFlash       = false;
  flashed       = false;
  flashLen      = 2;
  outPortSet    = false;
  outPortON     = false;
  doMorse       = false;
}


// ---------------------------------------------------------------------------
// Initialisation
// ---------------------------------------------------------------------------
// This nitialisation is not part of the constructor, because we may call other
// resources which may return errors or throw exceptions
//
int PinIoCtrl::initPerif()
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
void PinIoCtrl::run(int frequency)
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
  if(outPortON)
  {
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
    } // end if simulatedDimm
    else
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
    }
  } // end if outPortON
  else
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
  }


  // -------------------------------------------------------------------------
} // end run()

// ---------------------------------------------------------------------------
// flash()              start flashing the info LED
// ---------------------------------------------------------------------------
//
void PinIoCtrl::flash(int len)
{
  int calc;

  if(len > 0)
  {
    calc = (currentFreq * len) / 1000;
    if(calc == 0)
      calc = 1;
    flashLen = calc;
  }
  else
    flashLen = 0;

  flashed = false;
  doFlash = true;
}

// ---------------------------------------------------------------------------
// dimm()               set intensity of info LED
// ---------------------------------------------------------------------------
// Not all Pins support PWM. Set <sim> for simulation
//
int PinIoCtrl::dimm(double damp, boolean sim)
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

// ---------------------------------------------------------------------------
// turn()               switch info LED on or off
// ---------------------------------------------------------------------------
// Not all Pins support PWM. Set <sim> for simulation
//
void PinIoCtrl::turn(boolean onOff)
{
  outPortON = onOff;
}

