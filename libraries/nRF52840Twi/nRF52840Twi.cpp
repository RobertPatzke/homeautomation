//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   nRF52840Radio.cpp
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
//

#include "nRF52840Twi.h"
#include <string.h>

// ----------------------------------------------------------------------------
// Initialisierungen
// ----------------------------------------------------------------------------

nRF52840Twi::nRF52840Twi()
{
  twiPtr      = NULL;
  instPtr0    = NULL;
  instPtr1    = NULL;
  irqCounter  = 0;
}

// ----------------------------------------------------------------------------
// Konfiguration
// ----------------------------------------------------------------------------
//
TwiError nRF52840Twi::begin(TwiParamsPtr inParPtr)
{
  TwiError    retv;
  nrfGpioPtr  gpioPtr;
  dword       regVal;

  retv = TEnoError;

  // Setzen des Peripheriezeigers anhand der Instanz
  // und Initialisieren weiterer Variablen/Zeiger
  //
  if(inParPtr->inst == 0)
  {
    twiPtr    = NrfTwiPtr0;
    clrAllEvents();
    instPtr0  = this;

    // Interruptvektor setzen
    //
    __NVIC_SetVector((IRQn_Type) 3, (dword) nRF52840Twi::irqHandler0);
    __NVIC_SetPriority((IRQn_Type) 3, 1);
    __NVIC_EnableIRQ((IRQn_Type) 3);
  }
  else
  {
    twiPtr    = NrfTwiPtr1;
    clrAllEvents();
    instPtr1  = this;

    // Interruptvektor setzen
    //
    __NVIC_SetVector((IRQn_Type) 4, (dword) nRF52840Twi::irqHandler1);
    __NVIC_SetPriority((IRQn_Type) 4, 1);
    __NVIC_EnableIRQ((IRQn_Type) 4);
  }

  // Alternative Peripherie (gleiche ID, also Alles) abschalten
  //
  twiPtr->ENABLE = TwiDisable;


  // Takt
  // -------------------------------------------------
  //
  // Pins zuweisen und initialisieren
  //
  if(inParPtr->clkPort == 1)
  {
    regVal = 32 + inParPtr->clkPin;
    gpioPtr = NrfGpioPtr1;
  }
  else
  {
    regVal = inParPtr->clkPin;
    gpioPtr = NrfGpioPtr0;
  }

  // Connect (hoechstwertiges Bit) beruecksichtigen
  //
  twiPtr->PSEL_SCL = regVal | 0x7FFFFFC0;

  // Zugewiesenen Pin als Eingang schalten und Treibermodus setzen
  // Laut Datenblatt ist das entsprechende Bit im Konfigurationsregister
  // mit dem DIR-Register physikalisch verbunden
  //
  gpioPtr->PIN_CNF[inParPtr->clkPin] = GpioPinCnf_DRIVE(GpioDriveS0D1);

  // Daten
  // -------------------------------------------------
  //
  if(inParPtr->dataPort == 1)
  {
    regVal = 32 + inParPtr->dataPin;
    gpioPtr = NrfGpioPtr1;
  }
  else
  {
    regVal = inParPtr->dataPin;
    gpioPtr = NrfGpioPtr0;
  }

  // Connect (hoechstwertiges Bit) beruecksichtigen
  //
  twiPtr->PSEL_SDA = regVal | 0x7FFFFFC0;

  // Zugewiesenen Pin als Eingang schalten und Treibermodus setzen
  // Laut Datenblatt ist das entsprechende Bit im Konfigurationsregister
  // mit dem DIR-Register physikalisch verbunden
  //
  gpioPtr->PIN_CNF[inParPtr->dataPin] = GpioPinCnf_DRIVE(GpioDriveS0D1);

  // Frequenz einstellen
  //
  if(inParPtr->speed == Twi100k)
    regVal = NrfTwi100k;
  else if(inParPtr->speed == Twi400k)
    regVal = NrfTwi400k;
  else
    regVal = NrfTwi250k;

  twiPtr->FREQUENCY = regVal;

  twiPtr->SHORTS = 0;

  // Interrupts freischalten
  //
  twiPtr->INTENSET = (TwiInt_TXDSENT | TwiInt_RXDREADY | TwiInt_ERROR | TwiInt_STOPPED);

  // Und bereit machen
  //
  twiPtr->ENABLE = TwiEnable;


  return(retv);
}



// ----------------------------------------------------------------------------
// Steuerfunktionen, gezielte Prozessorzugriffe und Hilfsfunktionen
// ----------------------------------------------------------------------------
//
void nRF52840Twi::clrAllEvents()
{
  twiPtr->EVENTS_STOPPED    = 0;
  twiPtr->EVENTS_RXDREADY   = 0;
  twiPtr->EVENTS_TXDSENT    = 0;
  twiPtr->EVENTS_SUSPENDED  = 0;
  twiPtr->EVENTS_BB         = 0;
  twiPtr->EVENTS_ERROR      = 0;
}

// ----------------------------------------------------------------------------
// Ereignisbearbeitung und Interrupts
// ----------------------------------------------------------------------------
//
nRF52840Twi *nRF52840Twi::instPtr0 = NULL;

void nRF52840Twi::irqHandler0()
{
  if(instPtr0 == NULL) return;
  instPtr0->irqHandler();
}

nRF52840Twi *nRF52840Twi::instPtr1 = NULL;

void nRF52840Twi::irqHandler1()
{
  if(instPtr1 == NULL) return;
  instPtr1->irqHandler();
}

// ----------------------------------------------------------------------------
//                      M a s t e r
// ----------------------------------------------------------------------------
//

  // --------------------------------------------------------------------------
  // Datenaustausch
  // --------------------------------------------------------------------------
  //
TwiError nRF52840Twi::writeByte(int adr, TwiBytePtr refByte)
{
  TwiError retv = TEnoError;
  lastError     = 0;

  byteStruPtr     = refByte;
  twiPtr->ADDRESS = adr;

  byteStruPtr->twiStatus  = TwStWrReq;
  trfMode                 = ttmWriteByte;

  twiPtr->TASKS_STARTTX   = 1;
  twiPtr->TXD             = refByte->value;

  return(retv);
}

TwiError nRF52840Twi::readByteReg(int adr, int reg, TwiBytePtr refByte)
{
  TwiError retv = TEnoError;

  //dynHand = &nRF52840Twi::irqHandler;

  byteStruPtr     = refByte;
  twiPtr->ADDRESS = adr;

  byteStruPtr->twiStatus  = TwStRdReq;
  trfMode                 = ttmReadByteReg;

  twiPtr->TASKS_STARTTX   = 1;
  twiPtr->TXD             = reg;

  return(retv);
}

TwiError nRF52840Twi::readByteRegSeq(int adr, int reg, TwiByteSeqPtr refByteSeq)
{
  TwiError retv = TEnoError;

  //dynHand = &nRF52840Twi::irqHandler;

  byteSeqPtr      = refByteSeq;
  recIdx          = 0;
  twiPtr->ADDRESS = adr;

  byteStruPtr->twiStatus  = TwStRdReq;
  trfMode                 = ttmReadByteReg;

  twiPtr->TASKS_STARTTX   = 1;
  twiPtr->TXD             = reg;

  return(retv);
}



  // --------------------------------------------------------------------------
  // Interrupts (Ereignisbehandlung)
  // --------------------------------------------------------------------------
  //
void nRF52840Twi::irqHandler()
{
  irqCounter++;

  switch(trfMode)
  {
    case ttmWriteByte:
    // ------------------------------------------------------------------------

      if(twiPtr->EVENTS_ERROR)
      {
        twiPtr->EVENTS_ERROR    = 0;
        lastError               = twiPtr->ERRORSRC;
        twiPtr->ERRORSRC        = (lastError & 0x06);   // Clear AdrNak/DataNak
        byteStruPtr->twiStatus  = (TwiStatus) ( (int) TwStError + lastError);
        twiPtr->TASKS_STOP      = 1;
        return;
      }

      if(twiPtr->EVENTS_TXDSENT)
      {
        twiPtr->EVENTS_TXDSENT   = 0;
        byteStruPtr->twiStatus   = TwStSent;
        twiPtr->TASKS_STOP       = 1;
        return;
      }

      if(twiPtr->EVENTS_STOPPED)
      {
        twiPtr->EVENTS_STOPPED  = 0;
        if(lastError == 0)
          byteStruPtr->twiStatus  = TwStFin;
      }

      break;

    case ttmReadByteReg:
    // ------------------------------------------------------------------------

      if(twiPtr->EVENTS_ERROR)
      {
        twiPtr->EVENTS_ERROR    = 0;
        lastError               = twiPtr->ERRORSRC;
        twiPtr->ERRORSRC        = (lastError & 0x06);   // Clear AdrNak/DataNak
        byteStruPtr->twiStatus  = (TwiStatus) ( (int) TwStError + lastError);
        twiPtr->TASKS_STOP      = 1;
        return;
      }

      if(twiPtr->EVENTS_TXDSENT)
      {
        twiPtr->EVENTS_TXDSENT   = 0;
        byteStruPtr->twiStatus   = TwStSent;
        twiPtr->TASKS_STARTRX    = 1;
        return;
      }

      if(twiPtr->EVENTS_STOPPED)
      {
        twiPtr->EVENTS_STOPPED  = 0;
        if(lastError == 0)
          byteStruPtr->twiStatus  = TwStFin;
        return;
      }

      if(twiPtr->EVENTS_RXDREADY)
      {
        twiPtr->EVENTS_RXDREADY = 0;
        byteStruPtr->twiStatus  = TwStRecvd;
        twiPtr->TASKS_STOP      = 1;
        byteStruPtr->value      = twiPtr->RXD;
        return;
      }

      break;

    case ttmReadByteRegSeq:
    // ------------------------------------------------------------------------

      if(twiPtr->EVENTS_ERROR)
      {
        twiPtr->EVENTS_ERROR    = 0;
        lastError               = twiPtr->ERRORSRC;
        twiPtr->ERRORSRC        = (lastError & 0x06);   // Clear AdrNak/DataNak
        byteSeqPtr->twiStatus   = (TwiStatus) ( (int) TwStError + lastError);
        twiPtr->TASKS_STOP      = 1;
        return;
      }

      if(twiPtr->EVENTS_TXDSENT)
      {
        twiPtr->EVENTS_TXDSENT   = 0;
        byteSeqPtr->twiStatus    = TwStSent;
        twiPtr->TASKS_STARTRX    = 1;
        return;
      }

      if(twiPtr->EVENTS_STOPPED)
      {
        twiPtr->EVENTS_STOPPED   = 0;
        if(lastError == 0)
          byteSeqPtr->twiStatus  = TwStFin;
        return;
      }

      if(twiPtr->EVENTS_RXDREADY)
      {
        twiPtr->EVENTS_RXDREADY       = 0;
        byteSeqPtr->twiStatus         = TwStRecvd;

        if(recIdx == (byteSeqPtr->len - 1))
          twiPtr->TASKS_STOP          = 1;
        byteSeqPtr->valueRef[recIdx]  = twiPtr->RXD;
        recIdx++;
        return;
      }

      break;

  }

  /*
  if(twiPtr->EVENTS_ERROR)
  {
    twiPtr->EVENTS_ERROR    = 0;
    byteStruPtr->twiStatus  = (TwiStatus) ( (int) TwStError +  twiPtr->ERRORSRC);
    return;
  }

  if(twiPtr->EVENTS_TXDSENT)
  {
    twiPtr->EVENTS_TXDSENT  = 0;
    byteStruPtr->twiStatus  = TwStSent;
    twiPtr->TASKS_STARTRX   = 1;
    return;
  }

  if(twiPtr->EVENTS_RXDREADY)
  {
    twiPtr->EVENTS_RXDREADY = 0;
    byteStruPtr->twiStatus  = TwStRecvd;
    byteStruPtr->value      = twiPtr->RXD;
    twiPtr->TASKS_STOP      = 1;
    return;
  }

  if(twiPtr->EVENTS_STOPPED)
  {
    twiPtr->EVENTS_STOPPED  = 0;
    byteStruPtr->twiStatus  = TwStFin;
  }
  */

}

// ----------------------------------------------------------------------------
//                      S l a v e
// ----------------------------------------------------------------------------

// Starten des Datenempfangs
//

// ----------------------------------------------------------------------------
//                      D e b u g - H i l f e n
// ----------------------------------------------------------------------------
//
dword nRF52840Twi::getIrqCount()
{
  return(irqCounter);
}







