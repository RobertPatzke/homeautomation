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

void      SamD21Gpio::setCnf(unsigned int cnfBits, GpioExtRefPtr refPtr)
{
  if((cnfBits & IfDrvOutput))     // Ausgang **********************************
  {
    ((sd21GpioPtr)refPtr->ioPtr)->DIRSET = refPtr->pins;
    if(refPtr->next != NULL)
      ((sd21GpioPtr)refPtr->next->ioPtr)->DIRSET = refPtr->pins;
  }
  else                            // Eingang **********************************
  {
    ((sd21GpioPtr)refPtr->ioPtr)->DIRCLR = refPtr->pins;
    if(cnfBits & IfDrvPullUp)
      ((sd21GpioPtr)refPtr->ioPtr)->OUTSET = refPtr->pins;
    if(cnfBits & IfDrvPullDown)
      ((sd21GpioPtr)refPtr->ioPtr)->OUTCLR = refPtr->pins;

    if(refPtr->next != NULL)
    {
      ((sd21GpioPtr)refPtr->next->ioPtr)->DIRCLR = refPtr->next->pins;
      if(cnfBits & IfDrvPullUp)
        ((sd21GpioPtr)refPtr->next->ioPtr)->OUTSET = refPtr->next->pins;
      if(cnfBits & IfDrvPullDown)
        ((sd21GpioPtr)refPtr->next->ioPtr)->OUTCLR = refPtr->next->pins;
    }
  }
}


GpioError SamD21Gpio::config(int nrFrom, int nrTo, unsigned int cnfBits, GpioExtRefPtr refPtr)
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
    refPtr->next  = NULL;
  }
  else
  {
    refPtr->ioPtr = (dword *) SD21GpioPtr0;
    refPtr->pins  = tmpMask0;
    if(refPtr->next != NULL)
    {
      refPtr->next->ioPtr = (dword *) SD21GpioPtr1;
      refPtr->next->pins  = tmpMask1;
    }
  }

  setCnf(cnfBits, refPtr);
  return(retv);
}

GpioError SamD21Gpio::config(int nr, unsigned int cnfBits, GpioExtRefPtr refPtr)
{
  return(config(nr,nr,cnfBits, refPtr));
}

GpioError SamD21Gpio::config(GpioExtMask mask, unsigned int cnfBits, GpioExtRefPtr refPtr)
{
  GpioError retv = GEnoError;
  byte      cnfVal;
  dword     tmpMask;

  GpioExtRef     locRef;
  GpioExtRef     locRefExt;
  GpioExtRefPtr  locRefPtr;

  if(refPtr == NULL)
  {
    locRefPtr = &locRef;
    locRef.next = &locRefExt;
  }
  else
    locRefPtr = refPtr;

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

  locRefPtr->ioPtr = (dword *) gpioPtr;
  locRefPtr->pins  = mask.pins;

  if(mask.next != NULL)
  {
    chkMask = 1;

    for(int i = 0; i < 32; i++)
    {
      if(mask.next->port == 0)
        gpioPtr = SD21GpioPtr0;
      else
        gpioPtr = SD21GpioPtr1;

      if(mask.next->pins & chkMask)
        gpioPtr->PINCFG[i] = cnfVal;

      chkMask <<= 1;
    }

    if(locRefPtr->next != NULL)
    {
      locRefPtr->next->ioPtr = (dword *) gpioPtr;
      locRefPtr->next->pins  = mask.next->pins;
    }
  }

  setCnf(cnfBits, locRefPtr);
  return(retv);
}

GpioError SamD21Gpio::configArd(ArdMask ardMask, unsigned int cnfBits)
{
  GpioExtMask  ioMask;
  GpioExtMask  ioMaskExt;

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

  return config(ioMask, cnfBits, NULL);
}



  // --------------------------------------------------------------------------
  // Anwendungsfunktionen
  // --------------------------------------------------------------------------
  //
void      SamD21Gpio::read(GpioExtRefPtr refPtr, GpioExtValPtr valPtr)
{
  valPtr->value = ((sd21GpioPtr) refPtr->ioPtr)->IN & refPtr->pins;
  if((valPtr->next == NULL) || (refPtr->next == NULL)) return;
  valPtr->next->value = ((sd21GpioPtr) refPtr->next->ioPtr)->IN & refPtr->next->pins;
}

dword     SamD21Gpio::readArd(ArdMask ardMask)
{
  dword   inVal0, inVal1;
  dword   retVal;

  inVal0 = SD21GpioPtr0->IN;
  inVal1 = SD21GpioPtr1->IN;

  retVal = 0;

  switch(ardMask)
  {
    case ArdA0A3:
      if(inVal0 & ArdA0Mask) retVal |= 0x01;
      if(inVal1 & ArdA1Mask) retVal |= 0x02;
      if(inVal1 & ArdA2Mask) retVal |= 0x04;
      if(inVal0 & ArdA3Mask) retVal |= 0x08;
      break;

    case ArdA4A5:
      if(inVal0 & ArdA4Mask) retVal |= 0x01;
      if(inVal1 & ArdA5Mask) retVal |= 0x02;
      break;

    case ArdA0A5:
      if(inVal0 & ArdA0Mask) retVal |= 0x01;
      if(inVal1 & ArdA1Mask) retVal |= 0x02;
      if(inVal1 & ArdA2Mask) retVal |= 0x04;
      if(inVal0 & ArdA3Mask) retVal |= 0x08;
      if(inVal0 & ArdA4Mask) retVal |= 0x10;
      if(inVal1 & ArdA5Mask) retVal |= 0x20;
      break;

    case ArdD2D5:
      if(inVal0 & ArdD2Mask) retVal |= 0x01;
      if(inVal0 & ArdD3Mask) retVal |= 0x02;
      if(inVal0 & ArdD4Mask) retVal |= 0x04;
      if(inVal0 & ArdD5Mask) retVal |= 0x08;
      break;
  }

  return(retVal);
}

void      SamD21Gpio::write(GpioExtRefPtr refPtr, GpioExtValPtr valPtr)
{
  ((sd21GpioPtr) refPtr->ioPtr)->OUTSET = valPtr->value & refPtr->pins;
  ((sd21GpioPtr) refPtr->ioPtr)->OUTCLR = ~valPtr->value & refPtr->pins;
  if(refPtr->next == NULL) return;
  ((sd21GpioPtr) refPtr->next->ioPtr)->OUTSET = valPtr->next->value & refPtr->next->pins;
  ((sd21GpioPtr) refPtr->next->ioPtr)->OUTCLR = ~valPtr->next->value & refPtr->next->pins;
}

void      SamD21Gpio::set(GpioExtRefPtr refPtr)
{
  ((sd21GpioPtr) refPtr->ioPtr)->OUTSET = refPtr->pins;
  if(refPtr->next == NULL) return;
  ((sd21GpioPtr) refPtr->next->ioPtr)->OUTSET = refPtr->next->pins;
}

void      SamD21Gpio::clr(GpioExtRefPtr refPtr)
{
  ((sd21GpioPtr) refPtr->ioPtr)->OUTCLR = refPtr->pins;
  if(refPtr->next == NULL) return;
  ((sd21GpioPtr) refPtr->next->ioPtr)->OUTCLR = refPtr->next->pins;
}

void      SamD21Gpio::writeArd(ArdMask ardMask, dword value)
{
  dword   set0 = 0, set1 = 0;
  dword   clr0 = 0, clr1 = 0;

  switch(ardMask)
  {
    case ArdA0A3:
      if(value & 0x01) set0 |= ArdA0Mask;
      else clr0 |= ArdA0Mask;
      if(value & 0x02) set1 |= ArdA1Mask;
      else clr1 |= ArdA1Mask;
      if(value & 0x04) set1 |= ArdA2Mask;
      else clr1 |= ArdA2Mask;
      if(value & 0x08) set0 |= ArdA3Mask;
      else clr0 |= ArdA3Mask;

      SD21GpioPtr0->OUTSET = set0;
      SD21GpioPtr0->OUTCLR = clr0;
      SD21GpioPtr1->OUTSET = set1;
      SD21GpioPtr1->OUTCLR = clr1;
      break;

    case ArdA4A5:
      if(value & 0x10) set0 |= ArdA4Mask;
      else clr0 |= ArdA4Mask;
      if(value & 0x20) set1 |= ArdA5Mask;
      else clr1 |= ArdA5Mask;

      SD21GpioPtr0->OUTSET = set0;
      SD21GpioPtr0->OUTCLR = clr0;
      SD21GpioPtr1->OUTSET = set1;
      SD21GpioPtr1->OUTCLR = clr1;
      break;

    case ArdA0A5:
      if(value & 0x01) set0 |= ArdA0Mask;
      else clr0 |= ArdA0Mask;
      if(value & 0x02) set1 |= ArdA1Mask;
      else clr1 |= ArdA1Mask;
      if(value & 0x04) set1 |= ArdA2Mask;
      else clr1 |= ArdA2Mask;
      if(value & 0x08) set0 |= ArdA3Mask;
      else clr0 |= ArdA3Mask;
      if(value & 0x10) set0 |= ArdA4Mask;
      else clr0 |= ArdA4Mask;
      if(value & 0x20) set1 |= ArdA5Mask;
      else clr1 |= ArdA5Mask;

      SD21GpioPtr0->OUTSET = set0;
      SD21GpioPtr0->OUTCLR = clr0;
      SD21GpioPtr1->OUTSET = set1;
      SD21GpioPtr1->OUTCLR = clr1;
      break;

    case ArdD2D5:
      if(value & 0x01) set0 |= ArdD2Mask;
      else clr0 |= ArdD2Mask;
      if(value & 0x02) set0 |= ArdD3Mask;
      else clr0 |= ArdD3Mask;
      if(value & 0x04) set0 |= ArdD4Mask;
      else clr0 |= ArdD4Mask;
      if(value & 0x08) set0 |= ArdD5Mask;
      else clr0 |= ArdD5Mask;

      SD21GpioPtr0->OUTSET = set0;
      SD21GpioPtr0->OUTCLR = clr0;
      break;
  }
}


  // ----------------------------------------------------------------------------
  // Ereignisbearbeitung und Interrupts
  // ----------------------------------------------------------------------------
  //
