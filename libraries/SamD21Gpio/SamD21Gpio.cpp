//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   SamD21Gpio.cpp
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
// Datum:   15. April 2022
//

#include "SamD21Gpio.h"

  // --------------------------------------------------------------------------
  // Konstruktoren
  // --------------------------------------------------------------------------
  //
  SamD21Gpio::SamD21Gpio()
  {
    gpioPtr = NULL;
  }



  // --------------------------------------------------------------------------
  // Konfigurationen
  // --------------------------------------------------------------------------
  //

byte     SamD21Gpio::getCnfValue(unsigned int cnfBits)
{
  byte tmpMask = GpioPinCnf_INEN;

  if((cnfBits & IfDrvPullUp) || (cnfBits & IfDrvPullDown)) tmpMask |= GpioPinCnf_PULLEN;

  if((cnfBits & IfDrvOutput))     // Ausgang **********************************
  {
    if((cnfBits & IfDrvStrongHigh) || (cnfBits & IfDrvStrongLow))
    {
      tmpMask |= GpioPinCnf_DRVSTR;
    }
  }
  return(tmpMask);
}

GpioError SamD21Gpio::config(int nrFrom, int nrTo, unsigned int cnfBits, GpioRefPtr refPtr)
{
  GpioError retv = GEnoError;
  int       portNum;
  int       pinNum;
  dword     tmpMask0, tmpMask1;
  byte      cnfValue;

  tmpMask0 = IfDrvOpenDrain | IfDrvOpenSource;
  if((cnfBits & tmpMask0) == tmpMask0) return (GEcdictPar);

  tmpMask1 = IfDrvPullDown | IfDrvPullUp;
  if((cnfBits & tmpMask1) == tmpMask1) return (GEcdictPar);

  cnfValue  = getCnfValue(cnfBits);
  tmpMask0 = tmpMask1 = 0;

  // Bedienen des angegebenen Bereiches
  //
  for(int i = nrFrom; i <= nrTo; i++)
  {
    portNum = (i & 0x0E0) >> 5;
    pinNum  =  i & 0x01F;


    if(portNum == 0)
    {
      gpioPtr = SD21GpioPtr0;
      tmpMask0 |= (1 << pinNum);
    }
    else
    {
      gpioPtr = SD21GpioPtr1;
      tmpMask1 |= (1 << pinNum);
    }

      gpioPtr->PINCFG[pinNum] = cnfValue;
  }

  if((nrFrom & 0x0E0) == (nrTo & 0x0E0))
  {
    refPtr->ioPtr = (dword *) gpioPtr;
    refPtr->pins  = tmpMask0 | tmpMask1;
  }
  else
  {
    refPtr->ioPtr = (dword *) SD21GpioPtr0;
    refPtr->pins  = tmpMask0;
    refPtr->next->ioPtr = (dword *) SD21GpioPtr1;
    refPtr->next->pins  = tmpMask1;
  }

  if((cnfBits & IfDrvOutput))     // Ausgang **********************************
  {
    Hier weiter
  }

  return(retv);
}

GpioError SamD21Gpio::config(int nr, unsigned int cnfBits, GpioRefPtr refPtr)
{
  return(config(nr,nr,cnfBits, refPtr));
}

GpioError SamD21Gpio::config(GpioMask mask, unsigned int cnfBits, GpioRefPtr refPtr)
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
      gpioPtr = SD21GpioPtr0;
    else
      gpioPtr = SD21GpioPtr1;

    if(mask.pins & chkMask)
      gpioPtr->PINCFG[i] = cnfVal;

    chkMask <<= 1;
  }

  refPtr->ioPtr = (dword *) gpioPtr;
  refPtr->pins  = mask.pins;

  return(retv);
}

GpioError SamD21Gpio::configArd(ArdMask ardMask, unsigned int cnfBits, GpioRefPtr refPtr)
{
  GpioMask  ioMask;
  GpioMask  ioMaskExt;

  ioMask.next = &ioMaskExt;

  switch(ardMask)
  {
    case ArdA0A3:
      ioMask.port = ArdA0Port;
      ioMask.pins = ArdA0Mask | ArdA3Mask;
      ioMaskExt.port = ArdA1Port;
      ioMaskExt.pins = ArdA1Mask | ArdA2Mask;
      break;

    case ArdA4A5:
      ioMask.port = ArdA4Port;
      ioMask.pins = ArdA4Mask;
      ioMaskExt.port = ArdA5Port;
      ioMaskExt.pins = ArdA5Mask;
      break;

    case ArdA0A5:
      ioMask.port = ArdA0Port;
      ioMask.pins = ArdA0Mask | ArdA3Mask | ArdA4Mask;
      ioMaskExt.port = ArdA1Port;
      ioMaskExt.pins = ArdA1Mask | ArdA2Mask | ArdA5Mask;
      break;

    case ArdD2D5:
      ioMask.port = 0;
      ioMask.pins = ArdD2Mask | ArdD3Mask | ArdD4Mask | ArdD5Mask;
      break;
  }

  return config(ioMask, cnfBits, refPtr);
}



  // --------------------------------------------------------------------------
  // Anwendungsfunktionen
  // --------------------------------------------------------------------------
  //
dword     SamD21Gpio::read(GpioRef ioRef)
{
  gpioPtr = (sd21GpioPtr) ioRef.ioPtr;
  return(gpioPtr->IN & ioRef.pins);
}

dword     SamD21Gpio::readArd(ArdMask ardMask, GpioRef ioRef)
{
  dword   inVal;
  dword   retVal;

  gpioPtr = (sd21GpioPtr) ioRef.ioPtr;
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

    case ArdA4A5:
      if(inVal & ArdA4Mask) retVal |= 0x01;
      if(inVal & ArdA5Mask) retVal |= 0x02;
      break;

    case ArdA0A5:
      if(inVal & ArdA0Mask) retVal |= 0x01;
      if(inVal & ArdA1Mask) retVal |= 0x02;
      if(inVal & ArdA2Mask) retVal |= 0x04;
      if(inVal & ArdA3Mask) retVal |= 0x08;
      if(inVal & ArdA4Mask) retVal |= 0x10;
      if(inVal & ArdA5Mask) retVal |= 0x20;
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
