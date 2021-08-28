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

  params = *inParPtr;

  // Setzen des Peripheriezeigers anhand der Instanz
  // und Initialisieren weiterer Variablen/Zeiger
  //
  if(inParPtr->inst == 0)
  {
    twiPtr    = NrfTwiPtr0;
    clrAllEvents();
    instPtr0  = this;
    curIRQ    = 3;

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
    curIRQ    = 4;

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
  curIntEn = (TwiInt_TXDSENT | TwiInt_RXDREADY | TwiInt_ERROR | TwiInt_STOPPED);
  twiPtr->INTENSET = curIntEn;

  // Und bereit machen
  //
  twiPtr->ENABLE = TwiEnable;

  delay(10);

  return(retv);
}


void nRF52840Twi::getParams(TwiParamsPtr parPtr)
{
  *parPtr = params;
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
//                      M a s t e r
// ----------------------------------------------------------------------------
//

  // --------------------------------------------------------------------------
  // Datenaustausch
  // --------------------------------------------------------------------------
  //
TwiError nRF52840Twi::sendByte(int adr, TwiBytePtr refByte)
{
  TwiError retv = TEnoError;
  lastError     = 0;

  resetIrqList();

  byteStruPtr     = refByte;
  twiPtr->ADDRESS = adr;

  byteStruPtr->twiStatus  = TwStWrReq;
  trfMode                 = ttmWriteByte;

  twiPtr->TASKS_STARTTX   = 1;
  twiPtr->TXD             = refByte->value;

  return(retv);
}

TwiError nRF52840Twi::sendByteReg(int adr, int reg, TwiBytePtr refByte)
{
  TwiError retv = TEnoError;

  //dynHand = &nRF52840Twi::irqHandler;

  resetIrqList();

  byteStruPtr     = refByte;
  twiPtr->ADDRESS = adr;

  byteStruPtr->twiStatus  = TwStWrReq;
  trfMode                 = ttmWriteByteReg;
  comIdx                  = 1;
  irqIdx                  = 0;

  twiPtr->TASKS_STARTTX   = 1;
  twiPtr->TXD             = reg;

  return(retv);
}

TwiStatus nRF52840Twi::writeByteReg(int adr, int reg, byte value)
{
  twiPtr->INTENCLR = curIntEn;

  tmpByte.value = value;

  sendByteReg(adr, reg, &tmpByte);

  while(tmpByte.twiStatus != TwStFin)
  {
    irqHandler();

    if(tmpByte.twiStatus & TwStError)
      break;
  }

  twiPtr->INTENSET = curIntEn;

  return(tmpByte.twiStatus);
}


TwiError nRF52840Twi::recByteReg(int adr, int reg, TwiBytePtr refByte)
{
  TwiError retv = TEnoError;

  //dynHand = &nRF52840Twi::irqHandler;

  byteStruPtr     = refByte;
  twiPtr->ADDRESS = adr;

  resetIrqList();

  byteStruPtr->twiStatus  = TwStRdReq;
  trfMode                 = ttmReadByteReg;

  twiPtr->TASKS_STARTTX   = 1;
  twiPtr->TXD             = reg;

  return(retv);
}

int nRF52840Twi::readByteReg(int adr, int reg)
{
  twiPtr->INTENCLR = curIntEn;

  recByteReg(adr, reg, &tmpByte);

  while(tmpByte.twiStatus != TwStFin)
  {
    irqHandler();

    if(tmpByte.twiStatus & TwStError)
      break;
  }

  twiPtr->INTENSET = curIntEn;

  if(tmpByte.twiStatus == TwStFin)
    return(tmpByte.value);
  else
    return(-1);
}


TwiError nRF52840Twi::recByteRegSeq(int adr, int reg, TwiByteSeqPtr refByteSeq)
{
  TwiError retv = TEnoError;

  byteSeqPtr      = refByteSeq;
  comIdx          = 0;
  twiPtr->ADDRESS = adr;

  byteSeqPtr->twiStatus   = TwStRdReq;
  trfMode                 = ttmReadByteRegSeq;

  twiPtr->TASKS_STARTTX   = 1;
  twiPtr->TXD             = reg;

  return(retv);
}

TwiStatus nRF52840Twi::readByteRegSeq(int adr, int reg, TwiByteSeqPtr refByteSeq)
{
  byteSeqPtr      = refByteSeq;
  comIdx          = 0;
  twiPtr->ADDRESS = adr;

  byteSeqPtr->twiStatus   = TwStRdReq;
  trfMode                 = ttmReadByteRegSeq;

  twiPtr->INTENCLR        = curIntEn;

  twiPtr->TASKS_STARTTX   = 1;
  twiPtr->TXD             = reg;

  while(byteSeqPtr->twiStatus != TwStFin)
  {
    irqHandler();

    if(byteSeqPtr->twiStatus & TwStError)
      break;
  }

  twiPtr->INTENSET        = curIntEn;

  return(byteSeqPtr->twiStatus);
}

// ----------------------------------------------------------------------------
// Ereignisbearbeitung und Interrupts
// ----------------------------------------------------------------------------
//
nRF52840Twi *nRF52840Twi::instPtr0 = NULL;

void nRF52840Twi::irqHandler0()
{
  if(instPtr0 == NULL) return;
  instPtr0->irqCounter++;
  instPtr0->irqHandler();
}

nRF52840Twi *nRF52840Twi::instPtr1 = NULL;

void nRF52840Twi::irqHandler1()
{
  if(instPtr1 == NULL) return;
  instPtr1->irqCounter++;
  instPtr1->irqHandler();
}


  // --------------------------------------------------------------------------
  // Interrupts (Ereignisbehandlung)
  // --------------------------------------------------------------------------
  //
void nRF52840Twi::irqHandler()
{
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

        irqList[irqIdx++] = 8;
        return;
      }

      if(twiPtr->EVENTS_TXDSENT)
      {
        twiPtr->EVENTS_TXDSENT   = 0;
        byteStruPtr->twiStatus   = TwStSent;
        twiPtr->TASKS_STOP       = 1;

        irqList[irqIdx++] = 1;
        return;
      }

      if(twiPtr->EVENTS_STOPPED)
      {
        twiPtr->EVENTS_STOPPED  = 0;
        if(lastError == 0)
          byteStruPtr->twiStatus  = TwStFin;

        irqList[irqIdx++] = 3;
        return;
      }

      break;

    case ttmWriteByteReg:
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
        if(comIdx == 1)
        {
          comIdx = 0;
          twiPtr->TXD = byteStruPtr->value;
        }
        else
          twiPtr->TASKS_STOP    = 1;
        return;
      }

      if(twiPtr->EVENTS_STOPPED)
      {
        twiPtr->EVENTS_STOPPED  = 0;
        if(lastError == 0)
          byteStruPtr->twiStatus  = TwStFin;
        return;
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

        irqList[irqIdx++] = 8;
        return;
      }

      if(twiPtr->EVENTS_TXDSENT)
      {
        twiPtr->EVENTS_TXDSENT  = 0;
        byteStruPtr->twiStatus  = TwStSent;
        twiPtr->TASKS_STARTRX   = 1;
        twiPtr->SHORTS          = 2;

        irqList[irqIdx++] = 1;
        return;
      }

      if(twiPtr->EVENTS_STOPPED)
      {
        twiPtr->EVENTS_STOPPED  = 0;
        if(lastError == 0)
          byteStruPtr->twiStatus  = TwStFin;

        irqList[irqIdx++] = 3;
        twiPtr->SHORTS = 0;
        return;
      }

      if(twiPtr->EVENTS_RXDREADY)
      {
        twiPtr->EVENTS_RXDREADY = 0;
        byteStruPtr->twiStatus  = TwStRecvd;
        byteStruPtr->value      = twiPtr->RXD;

        irqList[irqIdx++] = 2;
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
        twiPtr->EVENTS_TXDSENT  = 0;
        byteSeqPtr->twiStatus   = TwStSent;
        twiPtr->TASKS_STARTRX   = 1;
        return;
      }

      if(twiPtr->EVENTS_STOPPED)
      {
        twiPtr->EVENTS_STOPPED   = 0;
        if(lastError == 0)
          byteSeqPtr->twiStatus  = TwStFin;
        twiPtr->SHORTS = 0;
        return;
      }

      if(twiPtr->EVENTS_RXDREADY)
      {
        twiPtr->EVENTS_RXDREADY       = 0;
        byteSeqPtr->twiStatus         = TwStRecvd;

        if(comIdx == (byteSeqPtr->len - 2))
          twiPtr->SHORTS              = 2;
        byteSeqPtr->valueRef[comIdx]  = twiPtr->RXD;
        comIdx++;
        return;
      }

      break;

  }
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

void nRF52840Twi::resetIrqList()
{
  irqIdx = 0;
  firstRead = true;

  for(int i = 0; i < 8; i++)
    irqList[i] = 0;
}

void nRF52840Twi::getIrqList(char *dest)
{
  int destIdx = 0;

  for(int i = 0; i < 8; i++)
  {
    if(irqList[i] == 0) break;

    dest[destIdx++] = ' ';
    dest[destIdx++] = irqList[i] + 0x30;
  }

  dest[destIdx] = '\0';
}





