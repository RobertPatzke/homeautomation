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

GpioError nRF52840Gpio::config(int nrFrom, int nrTo, unsigned int cnfBits, GpioExtRefPtr refPtr)
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

  cnfValue  = getCnfValue(cnfBits);    // spezifische Bits setzen
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

GpioError nRF52840Gpio::config(int nr, unsigned int cnfBits, GpioExtRefPtr refPtr)
{
  return(config(nr,nr,cnfBits, refPtr));
}

GpioError nRF52840Gpio::config(GpioExtMask *maskPtr, unsigned int cnfBits, GpioExtRefPtr refPtr)
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
    if(maskPtr->port == 0)
      gpioPtr = NrfGpioPtr0;
    else
      gpioPtr = NrfGpioPtr1;

    if(maskPtr->pins & chkMask)
      gpioPtr->PIN_CNF[i] = cnfVal;

    chkMask <<= 1;
  }

  if(refPtr != NULL)
  {
    refPtr->ioPtr = (dword *) gpioPtr;
    refPtr->pins  = maskPtr->pins;
  }

  return(retv);
}

GpioError nRF52840Gpio::configArd(ArdMask ardMask, unsigned int cnfBits)
{
  GpioExtMask  ioMask;

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

  return config(&ioMask, cnfBits, NULL);
}



  // --------------------------------------------------------------------------
  // Anwendungsfunktionen
  // --------------------------------------------------------------------------
  //
bool      nRF52840Gpio::isSet(GpioExtRefPtr ioRefPtr)
{
  gpioPtr = (nrfGpioPtr) ioRefPtr->ioPtr;
  return(gpioPtr->IN & ioRefPtr->pins);
}

bool      nRF52840Gpio::anySet(GpioExtRefPtr ioRefPtr)
{
  gpioPtr = (nrfGpioPtr) ioRefPtr->ioPtr;
  return(gpioPtr->IN & ioRefPtr->pins);
}

bool      nRF52840Gpio::allSet(GpioExtRefPtr ioRefPtr)
{
  gpioPtr = (nrfGpioPtr) ioRefPtr->ioPtr;
  return((gpioPtr->IN & ioRefPtr->pins) == ioRefPtr->pins);
}


void      nRF52840Gpio::read(GpioExtRefPtr ioRefPtr, GpioExtValPtr valPtr)
{
  gpioPtr = (nrfGpioPtr) ioRefPtr->ioPtr;
  valPtr->value = gpioPtr->IN;
}

dword     nRF52840Gpio::readArd(ArdMask ardMask)
{
  dword   inVal;
  dword   retVal;

  retVal = 0;

  switch(ardMask)
  {
    case ArdA0A3:
      inVal = NrfGpioPtr0->IN;
      if(inVal & ArdA0Mask) retVal |= 0x01;
      if(inVal & ArdA1Mask) retVal |= 0x02;
      if(inVal & ArdA2Mask) retVal |= 0x04;
      if(inVal & ArdA3Mask) retVal |= 0x08;
      break;

    case ArdA4A7:
      inVal = NrfGpioPtr0->IN;
      if(inVal & ArdA4Mask) retVal |= 0x01;
      if(inVal & ArdA5Mask) retVal |= 0x02;
      if(inVal & ArdA6Mask) retVal |= 0x04;
      if(inVal & ArdA7Mask) retVal |= 0x08;
      break;

    case ArdA0A7:
      inVal = NrfGpioPtr0->IN;
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
      inVal = NrfGpioPtr1->IN;
      if(inVal & ArdD2Mask) retVal |= 0x01;
      if(inVal & ArdD3Mask) retVal |= 0x02;
      if(inVal & ArdD4Mask) retVal |= 0x04;
      if(inVal & ArdD5Mask) retVal |= 0x08;
      break;
  }

  return(retVal);
}


void      nRF52840Gpio::write(GpioExtRefPtr refPtr, GpioExtValPtr valPtr)
{
  ((nrfGpioPtr) refPtr->ioPtr)->OUTSET = valPtr->value & refPtr->pins;
  ((nrfGpioPtr) refPtr->ioPtr)->OUTCLR = ~valPtr->value & refPtr->pins;
  if(refPtr->next == NULL) return;
  ((nrfGpioPtr) refPtr->next->ioPtr)->OUTSET = valPtr->next->value & refPtr->next->pins;
  ((nrfGpioPtr) refPtr->next->ioPtr)->OUTCLR = ~valPtr->next->value & refPtr->next->pins;
}

void      nRF52840Gpio::set(GpioExtRefPtr refPtr)
{
  ((nrfGpioPtr) refPtr->ioPtr)->OUTSET = refPtr->pins;
  if(refPtr->next == NULL) return;
  ((nrfGpioPtr) refPtr->next->ioPtr)->OUTSET = refPtr->next->pins;
}

void      nRF52840Gpio::clr(GpioExtRefPtr refPtr)
{
  ((nrfGpioPtr) refPtr->ioPtr)->OUTCLR = refPtr->pins;
  if(refPtr->next == NULL) return;
  ((nrfGpioPtr) refPtr->next->ioPtr)->OUTCLR = refPtr->next->pins;
}



void      nRF52840Gpio::writeArd(ArdMask ardMask, dword value)
{
  dword   set0 = 0, set1 = 0;
  dword   clr0 = 0, clr1 = 0;

  switch(ardMask)
  {
    case ArdA0A3:
      if(value & 0x01) set0 |= ArdA0Mask;
      else clr0 |= ArdA0Mask;
      if(value & 0x02) set0 |= ArdA1Mask;
      else clr0 |= ArdA1Mask;
      if(value & 0x04) set0 |= ArdA2Mask;
      else clr0 |= ArdA2Mask;
      if(value & 0x08) set0 |= ArdA3Mask;
      else clr0 |= ArdA3Mask;

      NrfGpioPtr0->OUTSET = set0;
      NrfGpioPtr0->OUTCLR = clr0;
      break;

    case ArdA4A7:
      if(value & 0x01) set0 |= ArdA4Mask;
      else clr0 |= ArdA4Mask;
      if(value & 0x02) set0 |= ArdA5Mask;
      else clr0 |= ArdA5Mask;
      if(value & 0x04) set0 |= ArdA6Mask;
      else clr0 |= ArdA6Mask;
      if(value & 0x08) set0 |= ArdA7Mask;
      else clr0 |= ArdA7Mask;

      NrfGpioPtr0->OUTSET = set0;
      NrfGpioPtr0->OUTCLR = clr0;
      break;

    case ArdA0A7:
      if(value & 0x01) set0 |= ArdA0Mask;
      else clr0 |= ArdA0Mask;
      if(value & 0x02) set0 |= ArdA1Mask;
      else clr0 |= ArdA1Mask;
      if(value & 0x04) set0 |= ArdA2Mask;
      else clr0 |= ArdA2Mask;
      if(value & 0x08) set0 |= ArdA3Mask;
      else clr0 |= ArdA3Mask;
      if(value & 0x01) set0 |= ArdA4Mask;
      else clr0 |= ArdA4Mask;
      if(value & 0x02) set0 |= ArdA5Mask;
      else clr0 |= ArdA5Mask;
      if(value & 0x04) set0 |= ArdA6Mask;
      else clr0 |= ArdA6Mask;
      if(value & 0x08) set0 |= ArdA7Mask;
      else clr0 |= ArdA7Mask;

      NrfGpioPtr0->OUTSET = set0;
      NrfGpioPtr0->OUTCLR = clr0;
      break;

    case ArdD2D5:
      if(value & 0x01) set1 |= ArdD2Mask;
      else clr1 |= ArdD2Mask;
      if(value & 0x02) set1 |= ArdD3Mask;
      else clr1 |= ArdD3Mask;
      if(value & 0x04) set1 |= ArdD4Mask;
      else clr1 |= ArdD4Mask;
      if(value & 0x08) set1 |= ArdD5Mask;
      else clr1 |= ArdD5Mask;

      NrfGpioPtr1->OUTSET = set1;
      NrfGpioPtr1->OUTCLR = clr1;
      break;
  }
}


  // ----------------------------------------------------------------------------
  // Ereignisbearbeitung und Interrupts
  // ----------------------------------------------------------------------------
  //
