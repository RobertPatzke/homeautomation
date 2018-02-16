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

  simulatedDimm = false;
  doMorse       = false;
  morseCount    = 0;
  morseSeqIdx   = 0;
  ditLen        = 150;
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
   outPortSet = false;
  return(retv);
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
        analogWrite(outPort, 0);
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
    #ifdef smnArduino
      if(dimmed && outPortON && !simulatedDimm)
        analogWrite(outPort, dimmVal);
      else
        digitalWrite(outPort, HIGH);
    #else
      alternativly set port/register direct
    #endif
      outPortSet    = false;
    }
    return;     // Doing FLASH is the one and only if running
  } // end if(doFlash)

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
          digitalWrite(outPort, HIGH);
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
          analogWrite(outPort, dimmVal);
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
      if(dimmed && !simulatedDimm)
        analogWrite(outPort, 0);
      digitalWrite(outPort, HIGH);
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

  if(doFlash) return;   // Flash is still running

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
  outPortSet = false;
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
    analogWrite(outPort, 0);
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
// Not all Pins support PWM. Set <sim> for simulation
//
void PinIoCtrl::turn(boolean onOff)
{
  outPortON = onOff;
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

    case 'Ü':
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcPauseChar;
      break;

    case 'L':
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcPauseChar;
      break;

    case 'Ä':
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcPauseChar;
      break;

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

    case 'Ö':
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDah;
      buffer[retv++] = mcDit;
      buffer[retv++] = mcPauseChar;
      break;

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

