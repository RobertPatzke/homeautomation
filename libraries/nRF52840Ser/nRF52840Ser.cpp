//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   nRF52840Ser.cpp
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
//

#include "nRF52840Ser.h"
#include <string.h>

// ----------------------------------------------------------------------------
// Initialisierungen
// ----------------------------------------------------------------------------

nRF52840Ser::nRF52840Ser()
{
  serPtr      = NULL;
  instPtr0    = NULL;
  irqCounter  = 0;
  curIntEn    = 0;
  curIRQ      = 0;
  lastError   = 0;
  cntError    = 0;
}

dword speedArr[18] =
{
    0x0004F000, 0x0009D000, 0x0013B000,
    0x00275000, 0x003B0000, 0x004EA000,
    0x0075F000, 0x00800000, 0x009D5000,
    0x00E50000, 0x00EBF000, 0x013A9000,
    0x01D7E000, 0x03AFB000, 0x04000000,
    0x075F7000, 0x0EBED000, 0x10000000
};

// ----------------------------------------------------------------------------
// Konfiguration
// ----------------------------------------------------------------------------
//
void nRF52840Ser::begin(SerParamsPtr inParPtr, IntrfBuf *bufferIf)
{
  nrfGpioPtr  gpioPtr;
  dword       regVal;

  // Setzen des Peripheriezeigers anhand der Instanz
  // und Initialisieren weiterer Variablen/Zeiger
  //
  serPtr = NrfSerPtr0;
  clrAllEvents();
  instPtr0  = this;
  curIRQ    = 2;

  // Interruptvektor setzen
  //
  __NVIC_SetVector((IRQn_Type) 2, (dword) nRF52840Ser::irqHandler0);
  __NVIC_SetPriority((IRQn_Type) 2, 1);
  __NVIC_EnableIRQ((IRQn_Type) 2);

  // Alternative Peripherie (gleiche ID, also Alles) abschalten
  //
  serPtr->ENABLE = SerDisable;


  // TxD
  // -------------------------------------------------
  //
  // Pins zuweisen und initialisieren
  //
  if(inParPtr->txdPort == 1)
  {
    regVal = 32 + inParPtr->txdPin;
    gpioPtr = NrfGpioPtr1;
  }
  else
  {
    regVal = inParPtr->txdPin;
    gpioPtr = NrfGpioPtr0;
  }


  // Connect (hoechstwertiges Bit) beruecksichtigen
  //
  serPtr->PSEL_TXD = regVal | 0x7FFFFFC0;

  // Zugewiesenen Pin als Ausgang schalten und Treibermodus setzen
  // Laut Datenblatt ist das entsprechende Bit im Konfigurationsregister
  // mit dem DIR-Register physikalisch verbunden
  //
  if(inParPtr->type == stStd)
    regVal = GpioPinCnf_DRIVE(GpioDriveS0S1);
  else if(inParPtr->type == stPow)
    regVal = GpioPinCnf_DRIVE(GpioDriveH0H1);
  else
    regVal = GpioPinCnf_DRIVE(GpioDriveH0D1);

  gpioPtr->PIN_CNF[inParPtr->txdPin] = regVal | GpioPinCnf_DIROUT;

  // RXD
  // -------------------------------------------------
  //
  if(inParPtr->rxdPort == 1)
  {
    regVal = 32 + inParPtr->rxdPin;
    gpioPtr = NrfGpioPtr1;
  }
  else
  {
    regVal = inParPtr->rxdPin;
    gpioPtr = NrfGpioPtr0;
  }

  // Connect (hoechstwertiges Bit) beruecksichtigen
  //
  serPtr->PSEL_RXD = regVal | 0x7FFFFFC0;

  // Zugewiesenen Pin als Eingang schalten und Treibermodus setzen
  // Laut Datenblatt ist das entsprechende Bit im Konfigurationsregister
  // mit dem DIR-Register physikalisch verbunden
  //
  gpioPtr->PIN_CNF[inParPtr->rxdPin] = GpioPinCnf_PULL(GpioPullUp);

  // Bitrate einstellen
  //
  regVal = speedArr[inParPtr->speed];
  serPtr->BAUDRATE = regVal;
  serPtr->SHORTS = 0;

  // Interrupts freischalten
  //
  curIntEn = (SerInt_TXDRDY | SerInt_RXDRDY | SerInt_ERROR);
  serPtr->INTENSET = curIntEn;

  // Und bereit machen
  //
  serPtr->ENABLE = SerEnable;
  txdFin = true;

  bufIf = bufferIf;
  delay(10);
}


// ----------------------------------------------------------------------------
// Steuerfunktionen, gezielte Prozessorzugriffe und Hilfsfunktionen
// ----------------------------------------------------------------------------
//
void nRF52840Ser::clrAllEvents()
{
  serPtr->EVENTS_RXDRDY     = 0;
  serPtr->EVENTS_TXDRDY     = 0;
  serPtr->EVENTS_ERROR      = 0;
}

// Fortsetzen des Interrupt-Sendebetriebs
//
void nRF52840Ser::resuSend()
{
  byte  td;

  if(!txdFin) return;
  if(bufIf == NULL) return;
  if(!bufIf->getByteSnd(&td)) return;

  txdFin = false;
  serPtr->EVENTS_TXDRDY = 0;
  serPtr->TXD = td;
}

// Starten des Sendebetriebs
//
void nRF52840Ser::startSend()
{
  serPtr->TASKS_STARTTX = 1;
}

// Anhalten des Sendebetriebs
//
void nRF52840Ser::stopSend()
{
  serPtr->TASKS_STOPTX = 1;
}

// Starten des Empfangsbetriebs
//
void nRF52840Ser::startRec()
{
  serPtr->TASKS_STARTRX = 1;
}

// Anhalten des Empfangsbetriebs
//
void nRF52840Ser::stopRec()
{
  serPtr->TASKS_STOPRX = 1;
}


// Bedingtes Ausgeben eines Zeichens
//
bool nRF52840Ser::condSend(byte c)
{
  if(!txdFin) return(false);

  txdFin = false;
  serPtr->EVENTS_TXDRDY = 0;
  serPtr->TXD = c;
  return(true);
}

// ----------------------------------------------------------------------------
// Ereignisbearbeitung und Interrupts
// ----------------------------------------------------------------------------
//
nRF52840Ser *nRF52840Ser::instPtr0 = NULL;

void nRF52840Ser::irqHandler0()
{
  if(instPtr0 == NULL) return;
  instPtr0->irqCounter++;
  instPtr0->irqHandler();
}


  // --------------------------------------------------------------------------
  // Interrupts (Ereignisbehandlung)
  // --------------------------------------------------------------------------
  //
void nRF52840Ser::irqHandler()
{
  byte  b;

  if(serPtr->EVENTS_TXDRDY != 0)
  {
    serPtr->EVENTS_TXDRDY = 0;
    if(bufIf == NULL) return;

    if(!bufIf->getByteSnd(&b))
      txdFin = true;
    else
      serPtr->TXD = b;
  }
  else if(serPtr->EVENTS_RXDRDY != 0)
  {
    serPtr->EVENTS_RXDRDY = 0;
    b = serPtr->RXD;
    if(bufIf == NULL) return;
    bufIf->putByteRec(b);
  }
  else if(serPtr->EVENTS_ERROR != 0)
  {
    serPtr->EVENTS_ERROR = 0;
    cntError++;
    lastError = serPtr->ERRORSRC;
    anyError |= lastError;
  }
}

// --------------------------------------------------------------------------
// Datenzugriffe
// --------------------------------------------------------------------------
//
// Letzten Fehler lesen (Bits)
//
int   nRF52840Ser::getLastError()
{
  return(lastError);
}

// Alle vorgekommenen Fehlerbits
//
int   nRF52840Ser::getAnyError()
{
  return(anyError);
}

// Anzahl der Fehler lesen
//
dword nRF52840Ser::getErrCount()
{
  return(cntError);
}



// ----------------------------------------------------------------------------
//                      D e b u g - H i l f e n
// ----------------------------------------------------------------------------
//
dword nRF52840Ser::getIrqCount()
{
  return(irqCounter);
}

void nRF52840Ser::resetIrqList()
{
  irqIdx = 0;
  firstRead = true;

  for(int i = 0; i < 8; i++)
    irqList[i] = 0;
}

void nRF52840Ser::getIrqList(char *dest)
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





