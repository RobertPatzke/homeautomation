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

  currentFreq = 1000;
  init(InfoLED);
}

PinIoCtrl::PinIoCtrl(int frequency)
{
#ifdef smnArduino

#endif

  currentFreq = frequency;
  init(InfoLED);
}

PinIoCtrl::PinIoCtrl(int frequency, int outport)
{
#ifdef smnArduino

#endif

  currentFreq = frequency;
  init(outport);
}

PinIoCtrl::PinIoCtrl(int frequency, PioDescr pioData)
{
#ifdef smnArduino

#endif

  currentFreq = frequency;
  init(pioData.pioPtr, pioData.mask);
}

void PinIoCtrl::init(int outPort)
{
  init(NULL, outPort);
}


void PinIoCtrl::init(Pio *pio, uint32_t portMask)
{
  pioOutDescr.mask      = portMask;
  pioOutDescr.pioPtr    = pio;
  doFlash               = false;
  doBlink               = false;
  flashed               = false;
  flashLen              = 2;
  outPortSet            = false;
  outPortON             = false;

  simulatedDimm         = false;
  doMorse               = false;
  morseCount            = 0;
  morseSeqIdx           = 0;
  ditLen                = 150;

  pioInDescr.mask       = -1;
  cpl                   = false;
  perInit               = false;
  chkInDuration         = false;
}


// ---------------------------------------------------------------------------
// Initialisation
// ---------------------------------------------------------------------------
// This initialisation is not part of the constructor, because we may call other
// resources which may return errors or throw exceptions
// This function also may be used for changing the Pin.
//
int PinIoCtrl::initPerif()
{
  int retv = 0;

#ifdef smnArduino
  if(pioOutDescr.pioPtr == NULL)
  {
    pinMode(pioOutDescr.mask, OUTPUT);
    if(cpl)
      digitalWrite(pioOutDescr.mask, LOW);
    else
      digitalWrite(pioOutDescr.mask, HIGH);
  }
  else
  {
  #ifdef smnSAM3X
    pioOutDescr.pioPtr->PIO_OER = pioOutDescr.mask;
    if(cpl)
      pioOutDescr.pioPtr->PIO_CODR = pioOutDescr.mask;
    else
      pioOutDescr.pioPtr->PIO_SODR = pioOutDescr.mask;
  #endif

  #ifdef smnSAMD21G18
    /*
    dword shMask = 0x00000001;
    for(int i = 0; i < 32; i++)
    {
      if(pioInDescr.mask & shMask)
        pioInDescr.pioPtr->PINCFG[i] = 0x02;
      shMask <<= 1;
    }

    pioInDescr.pioPtr->DIRCLR = pioInDescr.mask;
    */

    pioOutDescr.pioPtr->DIRSET = pioOutDescr.mask;

    if(cpl)
      pioOutDescr.pioPtr->OUTCLR = pioOutDescr.mask;
    else
      pioOutDescr.pioPtr->OUTSET = pioOutDescr.mask;
  #endif
  }
#else
  alternativly set port/register direct
#endif

  perInit       = true;
  outPortSet    = false;
  return(retv);
}

int PinIoCtrl::initPerif(PioDescr pioData)
{
  pioOutDescr = pioData;
  return(initPerif());
}

int PinIoCtrl::initPerif(int port)
{
  PioDescr pioDescr = {NULL, (uint32_t) port};
  return(initPerif(pioDescr));
}

// ---------------------------------------------------------------------------
// Auxiliary functions
// ---------------------------------------------------------------------------
//

// ---------------------------------------------------------------------------
// run()        cooperative threading
// ---------------------------------------------------------------------------
// This function has to be called cyclic, parameter is the calling frequency
//
void PinIoCtrl::run()
{
  uint32_t   tmpInt32;

  // -------------------------------------------------------------------------
  // run check digital input
  // -------------------------------------------------------------------------
  //
  if(chkInCnt > 0)
  {
  #ifdef smnArduino
    if(pioInDescr.pioPtr == NULL)
    {
      tmpInt32 = digitalRead(pioInDescr.mask);
    }
    else
    {
    #ifdef smnSAM3X
      tmpInt32 = pioInDescr.pioPtr->PIO_PDSR & pioInDescr.mask;
    #endif

    #ifdef smnSAMD21G18
      tmpInt32 = pioInDescr.pioPtr->IN & pioInDescr.mask;
    #endif
    }
  #else
    alternativly set port/register direct
  #endif

    if(tmpInt32 != chkInVal)
      chkInCnt = chkInSet;
    else
      chkInCnt--;
  }

  // -------------------------------------------------------------------------
  // run check input duration
  // -------------------------------------------------------------------------
  //
  if(chkInDuration)
  {
  #ifdef smnArduino
    if(pioInDescr.pioPtr == NULL)
    {
      tmpInt32 = digitalRead(pioInDescr.mask);
    }
    else
    {
    #ifdef smnSAM3X
      tmpInt32 = pioInDescr.pioPtr->PIO_PDSR & pioInDescr.mask;
    #endif

    #ifdef smnSAMD21G18
      tmpInt32 = pioInDescr.pioPtr->IN & pioInDescr.mask;
    #endif
    }
  #else
    alternativly set port/register direct
  #endif

    if(tmpInt32 == inDurVal)
    {
      inDurationCnt++;
      if(inDurationCnt > inDurationLim)
        inDurationCnt = inDurationLim;
    }
    else
    {
      inDurationCnt = 0;
      inDurVal = tmpInt32;
    }
  }


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
        if(pioOutDescr.pioPtr == NULL)
        {
          analogWrite(pioOutDescr.mask, 0);
          if(cpl)
            digitalWrite(pioOutDescr.mask, HIGH);
          else
            digitalWrite(pioOutDescr.mask, LOW);
        }
        else
        {
        #ifdef smnSAM3X
          if(cpl)
            pioOutDescr.pioPtr->PIO_SODR = pioOutDescr.mask;
          else
            pioOutDescr.pioPtr->PIO_CODR = pioOutDescr.mask;
        #endif

        #ifdef smnSAMD21G18
          if(cpl)
            pioOutDescr.pioPtr->OUTSET = pioOutDescr.mask;
          else
            pioOutDescr.pioPtr->OUTCLR = pioOutDescr.mask;
        #endif
        }
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
    #ifdef smnArduino
      if(pioOutDescr.pioPtr == NULL)
      {
        if(dimmed && outPortON && !simulatedDimm)
          analogWrite(pioOutDescr.mask, dimmVal);
        else
        {
          if(cpl)
            digitalWrite(pioOutDescr.mask, LOW);
          else
            digitalWrite(pioOutDescr.mask, HIGH);
        }
      }
      else
      {
      #ifdef smnSAM3X
        if(cpl)
          pioOutDescr.pioPtr->PIO_CODR = pioOutDescr.mask;
        else
          pioOutDescr.pioPtr->PIO_SODR = pioOutDescr.mask;
      #endif

      #ifdef smnSAMD21G18
        if(cpl)
          pioOutDescr.pioPtr->OUTCLR = pioOutDescr.mask;
        else
          pioOutDescr.pioPtr->OUTSET = pioOutDescr.mask;
      #endif
      }
    #else
      alternativly set port/register direct
    #endif
      outPortSet    = false;
    }
    return;     // Doing FLASH is the one and only if running
  } // end if(doFlash)

  // -------------------------------------------------------------------------
  // run blink
  // -------------------------------------------------------------------------
  //
  if(doBlink)
  {
    if(!blinked)
    {

      if(!outPortSet)
      {

        #ifdef smnArduino
        if(pioOutDescr.pioPtr == NULL)
        {
        #if !defined smnSAM3X && !defined smnSAMD21G18
          analogWrite(pioOutDescr.mask, 0);
        #endif
          if(cpl)
            digitalWrite(pioOutDescr.mask, HIGH);
          else
            digitalWrite(pioOutDescr.mask, LOW);
        }
        else
        {
        #ifdef smnSAM3X
          if(cpl)
            pioOutDescr.pioPtr->PIO_SODR = pioOutDescr.mask;
          else
            pioOutDescr.pioPtr->PIO_CODR = pioOutDescr.mask;
        #endif

        #ifdef smnSAMD21G18
          if(cpl)
            pioOutDescr.pioPtr->OUTSET = pioOutDescr.mask;
          else
            pioOutDescr.pioPtr->OUTCLR = pioOutDescr.mask;
        #endif
       }
        #else
        alternativly set port/register direct
        #endif

        outPortSet = true;
      }

      if(blinkLen > 0)
        blinkLen--;
      else
      {
        blinked = true;
        blinkLen = blinkLenSet;
      }

    }
    else
    {

      if(outPortSet)
      {

      #ifdef smnArduino
        if(pioOutDescr.pioPtr == NULL)
        {
          if(dimmed && outPortON && !simulatedDimm)
            analogWrite(pioOutDescr.mask, dimmVal);
          else
            digitalWrite(pioOutDescr.mask, HIGH);
        }
        else
        {
        #ifdef smnSAM3X
          if(cpl)
            pioOutDescr.pioPtr->PIO_CODR = pioOutDescr.mask;
          else
            pioOutDescr.pioPtr->PIO_SODR = pioOutDescr.mask;
        #endif

        #ifdef smnSAMD21G18
          if(cpl)
            pioOutDescr.pioPtr->OUTCLR = pioOutDescr.mask;
          else
            pioOutDescr.pioPtr->OUTSET = pioOutDescr.mask;
        #endif
        }
      #else
        alternativly set port/register direct
      #endif
        outPortSet    = false;
      }

      if(blinkPause > 0)
        blinkPause--;
      else
      {
        blinked = false;
        blinkPause = blinkPauseSet;
      }

    }
    return;     // Doing BLINK is the one and only if running
  } // end if(doBlink)


  // -------------------------------------------------------------------------
  // run dimmer simulation
  // -------------------------------------------------------------------------
  //
  if(outPortON)
  {
    if(dimmed)
    {
      if(simulatedDimm)
      {
        if(outPortSet)
        {
        #ifdef smnArduino
          if(pioOutDescr.pioPtr == NULL)
          {
            if(cpl)
              digitalWrite(pioOutDescr.mask, LOW);
            else
              digitalWrite(pioOutDescr.mask, HIGH);
          }
          else
          {
          #ifdef smnSAM3X
            if(cpl)
              pioOutDescr.pioPtr->PIO_CODR = pioOutDescr.mask;
            else
              pioOutDescr.pioPtr->PIO_SODR = pioOutDescr.mask;
          #endif

          #ifdef smnSAMD21G18
            if(cpl)
              pioOutDescr.pioPtr->OUTCLR = pioOutDescr.mask;
            else
              pioOutDescr.pioPtr->OUTSET = pioOutDescr.mask;
          #endif
          }
        #else
          alternativly set port/register direct
        #endif
          outPortSet = false;
        }

        if(dimmCount > 0)
        {
          dimmCount--;
        }
        else
        {
        #ifdef smnArduino
          if(pioOutDescr.pioPtr == NULL)
          {
            if(cpl)
              digitalWrite(pioOutDescr.mask, HIGH);
            else
              digitalWrite(pioOutDescr.mask, LOW);
          }
          else
          {
          #ifdef smnSAM3X
            if(cpl)
              pioOutDescr.pioPtr->PIO_SODR = pioOutDescr.mask;
            else
              pioOutDescr.pioPtr->PIO_CODR = pioOutDescr.mask;
          #endif

          #ifdef smnSAMD21G18
            if(cpl)
              pioOutDescr.pioPtr->OUTSET = pioOutDescr.mask;
            else
              pioOutDescr.pioPtr->OUTCLR = pioOutDescr.mask;
          #endif
          }
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
          analogWrite(pioOutDescr.mask, dimmVal);
        #else
          alternativly set port/register direct
        #endif

        outPortSet = true;
        }
      }
    } // end if dimmed

    else
    {
      if(!outPortSet)
      {
        #ifdef smnArduino
        if(pioOutDescr.pioPtr == NULL)
        {
          if(cpl)
            digitalWrite(pioOutDescr.mask, HIGH);
          else
            digitalWrite(pioOutDescr.mask, LOW);
        }
        else
        {
        #ifdef smnSAM3X
          if(cpl)
            pioOutDescr.pioPtr->PIO_SODR = pioOutDescr.mask;
          else
            pioOutDescr.pioPtr->PIO_CODR = pioOutDescr.mask;
        #endif

        #ifdef smnSAMD21G18
          if(cpl)
            pioOutDescr.pioPtr->OUTSET = pioOutDescr.mask;
          else
            pioOutDescr.pioPtr->OUTCLR = pioOutDescr.mask;
        #endif
        }
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
      if(pioOutDescr.pioPtr == NULL)
      {
        if(dimmed && !simulatedDimm)
          analogWrite(pioOutDescr.mask, 0);
        if(cpl)
          digitalWrite(pioOutDescr.mask, LOW);
        else
          digitalWrite(pioOutDescr.mask, HIGH);
      }
      else
      {
      #ifdef smnSAM3X
        if(cpl)
          pioOutDescr.pioPtr->PIO_CODR = pioOutDescr.mask;
        else
          pioOutDescr.pioPtr->PIO_SODR = pioOutDescr.mask;
      #endif

      #ifdef smnSAMD21G18
        if(cpl)
          pioOutDescr.pioPtr->OUTCLR = pioOutDescr.mask;
        else
          pioOutDescr.pioPtr->OUTSET = pioOutDescr.mask;
      #endif
      }
      #else
      alternativly set port/register direct
      #endif

      outPortSet = false;
    }
  }

  // -------------------------------------------------------------------------
  // run morse
  // -------------------------------------------------------------------------
  //
  if(doMorse)
  {
    if(morseCount > 0)
    {
      morseCount--;
    }
    else
    {
      if(outPortON)
      {
        morseCount = (currentFreq * ditLen) / 1000;
        outPortON = false;
        goto EndDoMorse;
      }

      byte morseCode = morseSequence[morseSeqIdx];
      morseSeqIdx++;
      if(morseSeqIdx >= smnMaxMorseLen)
        morseSeqIdx = 0;

      switch(morseCode)
      {
        case mcIgnore:
          morseCount = 0;
          break;

        case mcDit:
          morseCount = (currentFreq * ditLen) / 1000;
          outPortON = true;
          break;

        case mcDah:
          morseCount = (3 * currentFreq * ditLen) / 1000;
          outPortON = true;
          break;

        case mcPauseSig:
          morseCount = (currentFreq * ditLen) / 1000;
          outPortON = false;
          break;

        case mcPauseChar:
          morseCount = (2 * currentFreq * ditLen) / 1000;
          outPortON = false;
          break;

        case mcPauseWord:
          morseCount = (6 * currentFreq * ditLen) / 1000;
          outPortON = false;
          break;

        case mcPauseLong:
          morseCount = (34 * currentFreq * ditLen) / 1000;
          outPortON = false;
          break;

        case mcRepeat:
          morseSeqIdx = 0;
          break;

        case mcClose:
          morseCount = 0;
          morseSeqIdx = 0;
          outPortON = false;
          doMorse = false;
          break;
      }
    }
  }

EndDoMorse:
        ;
  // -------------------------------------------------------------------------
} // end run()

// ---------------------------------------------------------------------------
// flash()              start flashing the info LED
// ---------------------------------------------------------------------------
//
void PinIoCtrl::flash(int len)
{
  int calc;

  if(len < 0)
  {
    doFlash = false;
    return;
  }

  if(doFlash) return;   // Flash is still running

  if(len > 0)
  {
    calc = (currentFreq * len) / 1000;
    if(calc == 0)
      calc = 1;
    flashLen = calc - 1;
  }
  else
    flashLen = 0;

  flashed       = false;
  doFlash       = true;
  outPortSet    = false;
}

void PinIoCtrl::flashMin(int addCycle)
{
  if(doFlash) return;   // Flash is still running

  flashLen = addCycle;

  flashed       = false;
  doFlash       = true;
  outPortSet    = false;
}


void PinIoCtrl::blink(int len, int pause, bool chkBusy)
{
  if(chkBusy)
  {
    if(doBlink)
      return;
  }

  blink(len, pause);
}

//
void PinIoCtrl::blink(int len, int pause)
{
  int calc;

  if(len <= 0)
  {
    doBlink = false;
    return;
  }

  calc = (currentFreq * len) / 1000;
  if(calc == 0)
    calc = 1;
  blinkLen = blinkLenSet = calc;

  calc = (currentFreq * pause) / 1000;
  if(calc == 0)
    calc = 1;
  blinkPause = blinkPauseSet = calc;

  blinked       = false;
  doBlink       = true;
  outPortSet    = false;
}

// ---------------------------------------------------------------------------
// dimm()               set intensity of info LED
// ---------------------------------------------------------------------------
// Not all Pins support PWM. Set <sim> for simulation with simple On/Off
//
int PinIoCtrl::dimm(double damp, boolean sim)
{
  if(damp < 0.001)
  {
  #ifdef smnArduino
    analogWrite(pioOutDescr.mask, 0);
  #else
    alternativly set port/register direct
  #endif
    dimmed = false;
    return(0);
  }

  if(sim)
  {
    if(damp < 0.002) damp = 0.002;
    dimmVal = (int) (1 / damp);
    dimmCount = 0;
    simulatedDimm = true;
  }
  else
  {
    dimmVal = (int) ((1 - damp) * PWMRANGE);

    if(dimmVal == 0)
      dimmVal = 1;
  }

  outPortSet = false;
  dimmed = true;
  return(dimmVal);
}

// ---------------------------------------------------------------------------
// turn()               switch info LED on or off
// ---------------------------------------------------------------------------
//
void PinIoCtrl::turn(boolean onOff)
{
  outPortON = onOff;
}

// ---------------------------------------------------------------------------
// invert()             Change output signal from active low to active high
// ---------------------------------------------------------------------------
// ... or from active high to active low (default is active low)
//
void PinIoCtrl::invert()
{
  cpl = !cpl;
}

// ---------------------------------------------------------------------------
//
bool PinIoCtrl::inDigLevel(int port, int highLow, int periodTime)
{
  PioDescr pioData = { NULL, (uint32_t) port };
  return(inDigLevel(pioData, highLow, periodTime, 0));
}

bool PinIoCtrl::inDigLevel(PioDescr pioData, uint32_t highLow, int periodTime, int perId)
{
  if(chkInCnt > 0)
    return(false);

  if(pioInDescr.mask == (uint32_t)(-1))
  {
    chkInVal = highLow;
    chkInCnt = chkInSet = (currentFreq * periodTime) / 1000;
    pioInDescr.pioPtr   = pioData.pioPtr;
    pioInDescr.mask     = pioData.mask;

  #ifdef smnArduino
    if(pioInDescr.pioPtr == NULL)
    {
      pinMode(pioInDescr.mask, INPUT);
    }
    else
    {
    #ifdef smnSAM3X
      pioInDescr.pioPtr->PIO_ODR = pioInDescr.mask;
      pmc_enable_periph_clk(perId);
    #endif

    #ifdef smnSAMD21G18
      dword shMask = 0x00000001;
      for(int i = 0; i < 32; i++)
      {
        if(pioInDescr.mask & shMask)
          pioInDescr.pioPtr->PINCFG[i] = 0x02;
        shMask <<= 1;
      }
      pioInDescr.pioPtr->DIRCLR = pioInDescr.mask;
    #endif
    }
    #else
      alternativly set port/register direct
    #endif

    return(false);
  }

  pioInDescr.mask = -1;
  return(true);
}

// ---------------------------------------------------------------------------
// watch and evaluate digital input level
// ---------------------------------------------------------------------------
//
void PinIoCtrl::watchDigLevel(PioDescr pioData, int periodTime, int perId)
{
  if(periodTime == 0)
  {
    chkInDuration = false;
    return;
  }
  inDurationLim = (currentFreq * periodTime) / 1000;
  pioInDescr.pioPtr   = pioData.pioPtr;
  pioInDescr.mask     = pioData.mask;

#ifdef smnArduino
  if(pioInDescr.pioPtr == NULL)
  {
    pinMode(pioInDescr.mask, INPUT);
  }
  else
  {
  #ifdef smnSAM3X
    pioInDescr.pioPtr->PIO_ODR = pioInDescr.mask;
    pmc_enable_periph_clk(perId);
  #endif

  #ifdef smnSAMD21G18
    dword shMask = 0x00000001;
    for(int i = 0; i < 32; i++)
    {
      if(pioInDescr.mask & shMask)
        pioInDescr.pioPtr->PINCFG[i] = 0x02;
      shMask <<= 1;
    }
    pioInDescr.pioPtr->DIRCLR = pioInDescr.mask;
  #endif
  }
#else
  alternativly set port/register direct
#endif

  chkInDuration = true;
}

bool PinIoCtrl::stableDigLevel(uint32_t highLow)
{
  if(inDurVal != highLow)
    return(false);
  if(inDurationCnt >= inDurationLim)
    return(true);
  return(false);
}

bool PinIoCtrl::getDigLevel(uint32_t *highLow)
{
  *highLow = inDurVal;
  if(inDurationCnt < inDurationLim)
    return(false);
  return(true);
}

// ---------------------------------------------------------------------------
// sos()                start morsing SOS
// ---------------------------------------------------------------------------
//
void PinIoCtrl::sos(boolean repeat)
{
  int i = 0;

  morseSequence[i++] = mcDit;
  morseSequence[i++] = mcDit;
  morseSequence[i++] = mcDit;
  morseSequence[i++] = mcDah;
  morseSequence[i++] = mcDah;
  morseSequence[i++] = mcDah;
  morseSequence[i++] = mcDit;
  morseSequence[i++] = mcDit;
  morseSequence[i++] = mcDit;
  morseSequence[i++] = mcPauseWord;
  if(repeat)
    morseSequence[i++] = mcRepeat;
  else
    morseSequence[i++] = mcClose;
  doMorse = true;
}

// ---------------------------------------------------------------------------
// getMorseChar                 creating Morse sequence
// ---------------------------------------------------------------------------
// I've avoided to create a more smart method with using tables or so,
// because it would take RAM. This method does not take RAM.
//
int PinIoCtrl::getMorseChar(char chr, byte *buffer)
{
  int retv = 0;

  switch(toupper(chr))
  {
    case 'E':
      buffer[retv++] = mcDit;
      buffer[retv++] = mcPauseChar;
      break;

    case 'T':
      buffer[retv++] = mcDah;
      buffer[retv++] = mcPauseChar;
      break;

    case 'A':
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcPauseChar;
      break;

    case 'I':
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcPauseChar;
      break;

    case 'M':
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcPauseChar;
      break;

    case 'N':
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcPauseChar;
      break;

    case 'O':
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcPauseChar;
      break;

    case 'G':
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcPauseChar;
      break;

    case 'K':
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcPauseChar;
      break;

    case 'D':
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcPauseChar;
      break;

    case 'W':
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcPauseChar;
      break;

    case 'R':
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcPauseChar;
      break;

    case 'U':
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcPauseChar;
      break;

    case 'S':
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcPauseChar;
      break;

    case 'H':
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcPauseChar;
      break;

    case 'V':
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcPauseChar;
      break;

    case 'F':
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcPauseChar;
      break;

    /*
    case 'Ü':
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcPauseChar;
      break;
    */

    case 'L':
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcPauseChar;
      break;

    /*
    case 'Ä':
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcPauseChar;
      break;
    */

    case 'P':
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcPauseChar;
      break;

    case 'J':
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcPauseChar;
      break;

    case 'B':
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcPauseChar;
      break;

    case 'X':
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcPauseChar;
      break;

    case 'C':
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcPauseChar;
      break;

    case 'Y':
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcPauseChar;
      break;

    case 'Z':
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcPauseChar;
      break;

    case 'Q':
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcPauseChar;
      break;

    /*
    case 'Ö':
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcPauseChar;
      break;
    */

    case '1':
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcPauseChar;
      break;

    case '2':
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcPauseChar;
      break;

    case '3':
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcPauseChar;
      break;

    case '4':
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcPauseChar;
      break;

    case '5':
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcPauseChar;
      break;

    case '6':
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcPauseChar;
      break;

    case '7':
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcPauseChar;
      break;

    case '8':
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcPauseChar;
      break;

    case '9':
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcPauseChar;
      break;

    case '0':
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcPauseChar;
      break;

    case '.':
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcPauseChar;
      break;

    case ',':
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcPauseChar;
      break;

    case ':':
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcPauseChar;
      break;

    case '?':
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcPauseChar;
      break;

    case '-':
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcPauseChar;
      break;

    case '_':
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcPauseChar;
      break;

    case '(':
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcPauseChar;
      break;

    case ')':
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcPauseChar;
      break;

    case '\'':
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcPauseChar;
      break;

    case '=':
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcPauseChar;
      break;

    case '+':
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcPauseChar;
      break;

    case '/':
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcPauseChar;
      break;

    case '@':
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcPauseChar;
      break;

    /*
    case 'ß':
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcPauseChar;
      break;
    */

    case '\r':          // KA (Spruchanfang)
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcPauseChar;
      break;

    case '\n':          // AR (Spruchende)
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcPauseChar;
      break;

    case ' ':          // BT (Pause)
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcPauseChar;
      break;

    case '\t':          // VE (Verstanden)
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcPauseChar;
      break;

    case '\"':          // SK (Verkehrsende)
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcPauseChar;
      break;

    case '\a':          // SOS (Notruf)
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcPauseChar;
      break;

    default:
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      break;
  }

  return(retv);
}

