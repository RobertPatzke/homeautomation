//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   nRF52840Gpio.cpp
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
// Datum:   29. Juni 2021
//

#include "nRF52840Gpio.h"

  // --------------------------------------------------------------------------
  // Konstruktoren
  // --------------------------------------------------------------------------
  //
  nRF52840Gpio::nRF52840Gpio()
  {
    gpioPtr = NULL;
  }



  // --------------------------------------------------------------------------
  // Konfigurationen
  // --------------------------------------------------------------------------
  //

dword     nRF52840Gpio::getCnfValue(unsigned int cnfBits)
{
  dword tmpMask = 0;

  if(cnfBits & IfDrvPullUp) tmpMask |= GpioPinCnf_PULL(GpioPullUp);
  if(cnfBits & IfDrvPullDown) tmpMask |= GpioPinCnf_PULL(GpioPullDown);

  if((cnfBits & IfDrvOutput))     // Ausgang **********************************
  {
    tmpMask |= GpioPinCnf_DIR;

    if(cnfBits & IfDrvStrongHigh)       // StrongHigh = H1
    {
      if(cnfBits & IfDrvOpenSource)     // OpenSource = D0
        tmpMask |= GpioPinCnf_DRIVE(GpioDriveD0H1);
      else if(cnfBits & IfDrvStrongLow) // StrongLow = H0
        tmpMask |= GpioPinCnf_DRIVE(GpioDriveH0H1);
      else
        tmpMask |= GpioPinCnf_DRIVE(GpioDriveS0H1);
    }
    else if(cnfBits & IfDrvStrongLow)   // StrongLow = H0
    {
      if(cnfBits & IfDrvOpenDrain)      // OpenDrain = D1
        tmpMask |= GpioPinCnf_DRIVE(GpioDriveH0D1);
      else
        tmpMask |= GpioPinCnf_DRIVE(GpioDriveH0S1);
    }
    else
    {
      if(cnfBits & IfDrvOpenSource)     // OpenSource = D0
        tmpMask |= GpioPinCnf_DRIVE(GpioDriveD0S1);
      else if(cnfBits & IfDrvOpenDrain) // OpenDrain = D1
        tmpMask |= GpioPinCnf_DRIVE(GpioDriveS0D1);
      else
        tmpMask |= GpioPinCnf_DRIVE(GpioDriveS0S1);
    }
  }
  else                            // Eingang **********************************
  {
    tmpMask &= 0xFFFFFFFC;
  }

  return(tmpMask);
}

GpioError nRF52840Gpio::config(int nrFrom, int nrTo, unsigned int cnfBits, GpioRefPtr refPtr)
{
  GpioError retv = GEnoError;
  int       portNum;
  int       pinNum;
  dword     tmpMask;
  dword     cnfValue;

  tmpMask = IfDrvOpenDrain | IfDrvOpenSource;
  if((cnfBits & tmpMask) == tmpMask) return (GEcdictPar);

  tmpMask = IfDrvPullDown | IfDrvPullUp;
  if((cnfBits & tmpMask) == tmpMask) return (GEcdictPar);

  cnfValue  = getCnfValue(cnfBits);
  tmpMask   = 0;

  // Bedienen des angegebenen Bereiches
  //
  for(int i = nrFrom; i <= nrTo; i++)
  {
    portNum = (i & 0x0E0) >> 5;
    pinNum  =  i & 0x01F;

    tmpMask |= (1 << i);

    if(portNum == 0)
      gpioPtr = NrfGpioPtr0;
    else
      gpioPtr = NrfGpioPtr1;

      gpioPtr->PIN_CNF[pinNum] = cnfValue;
  }

  refPtr->ioPtr = (dword *) gpioPtr;
  refPtr->pins  = tmpMask;

  return(retv);
}

GpioError nRF52840Gpio::config(int nr, unsigned int cnfBits, GpioRefPtr refPtr)
{
  return(config(nr,nr,cnfBits, refPtr));
}

GpioError nRF52840Gpio::config(GpioMask mask, unsigned int cnfBits, GpioRefPtr refPtr)
{
  GpioError retv = GEnoError;
  dword     cnfVal;

  cnfVal = IfDrvOpenDrain | IfDrvOpenSource;
  if((cnfBits & cnfVal) == cnfVal) return (GEcdictPar);

  cnfVal = IfDrvPullDown | IfDrvPullUp;
  if((cnfBits & cnfVal) == cnfVal) return (GEcdictPar);

  cnfVal = getCnfValue(cnfBits);

  // Bedienen des angegebenen Bereiches
  //
  dword chkMask = 1;

  for(int i = 0; i < 32; i++)
  {
    if(mask.port == 0)
      gpioPtr = NrfGpioPtr0;
    else
      gpioPtr = NrfGpioPtr1;

    if(mask.pins & chkMask)
      gpioPtr->PIN_CNF[i] = cnfVal;

    chkMask <<= 1;
  }

  refPtr->ioPtr = (dword *) gpioPtr;
  refPtr->pins  = mask.pins;

  return(retv);
}

GpioError nRF52840Gpio::configArd(ArdMask ardMask, unsigned int cnfBits, GpioRefPtr refPtr)
{
  GpioMask  ioMask;

  switch(ardMask)
  {
    case ArdA0A3:
      ioMask.port = 0;
      ioMask.pins = ArdA0Mask | ArdA1Mask | ArdA2Mask | ArdA3Mask;
      break;

    case ArdA4A7:
      ioMask.port = 0;
      ioMask.pins = ArdA4Mask | ArdA5Mask | ArdA6Mask | ArdA7Mask;
      break;

    case ArdA0A7:
      ioMask.port = 0;
      ioMask.pins = ArdA0Mask | ArdA1Mask | ArdA2Mask | ArdA3Mask |
                    ArdA4Mask | ArdA5Mask | ArdA6Mask | ArdA7Mask;
      break;

    case ArdD2D5:
      ioMask.port = 1;
      ioMask.pins = ArdD2Mask | ArdD3Mask | ArdD4Mask | ArdD5Mask;
      break;
  }

  return config(ioMask, cnfBits, refPtr);
}



  // --------------------------------------------------------------------------
  // Anwendungsfunktionen
  // --------------------------------------------------------------------------
  //
dword     nRF52840Gpio::read(GpioRef ioRef)
{
  gpioPtr = (nrfGpioPtr) ioRef.ioPtr;
  return(gpioPtr->IN & ioRef.pins);
}

dword     nRF52840Gpio::readArd(ArdMask ardMask, GpioRef ioRef)
{
  dword   inVal;
  dword   retVal;

  gpioPtr = (nrfGpioPtr) ioRef.ioPtr;
  inVal = gpioPtr->IN;
  retVal = 0;

  switch(ardMask)
  {
    case ArdA0A3:
      if(inVal & ArdA0Mask) retVal |= 0x01;
      if(inVal & ArdA1Mask) retVal |= 0x02;
      if(inVal & ArdA2Mask) retVal |= 0x04;
      if(inVal & ArdA3Mask) retVal |= 0x08;
      break;

    case ArdA4A7:
      if(inVal & ArdA4Mask) retVal |= 0x01;
      if(inVal & ArdA5Mask) retVal |= 0x02;
      if(inVal & ArdA6Mask) retVal |= 0x04;
      if(inVal & ArdA7Mask) retVal |= 0x08;
      break;

    case ArdA0A7:
      if(inVal & ArdA0Mask) retVal |= 0x01;
      if(inVal & ArdA1Mask) retVal |= 0x02;
      if(inVal & ArdA2Mask) retVal |= 0x04;
      if(inVal & ArdA3Mask) retVal |= 0x08;
      if(inVal & ArdA4Mask) retVal |= 0x10;
      if(inVal & ArdA5Mask) retVal |= 0x20;
      if(inVal & ArdA6Mask) retVal |= 0x40;
      if(inVal & ArdA7Mask) retVal |= 0x80;
      break;

    case ArdD2D5:
      if(inVal & ArdD2Mask) retVal |= 0x01;
      if(inVal & ArdD3Mask) retVal |= 0x02;
      if(inVal & ArdD4Mask) retVal |= 0x04;
      if(inVal & ArdD5Mask) retVal |= 0x08;
      break;
  }

  return(retVal);
}


  // ----------------------------------------------------------------------------
  // Ereignisbearbeitung und Interrupts
  // ----------------------------------------------------------------------------
  //
