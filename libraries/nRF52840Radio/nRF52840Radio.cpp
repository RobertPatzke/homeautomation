//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   nRF52840Radio.h
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
//

#include "nRF52840Radio.h"
#include <string.h>

// ----------------------------------------------------------------------------
// Initialisierungen
// ----------------------------------------------------------------------------

nRF52840Radio::nRF52840Radio()
{
  NrfRadioPtr->TASKS_DISABLE;   // Sender/Empfänger abgeschaltet
#ifdef nrfPowerDCDCEN
  *nrfPowerDCDCEN = 1;
#endif
#ifdef nrfClockTASKS_HFCLKSTART
  *nrfClockTASKS_HFCLKSTART = 1;
#endif
  NrfRadioPtr->POWER = 0;
  NrfRadioPtr->POWER = 1;
}

// ----------------------------------------------------------------------------
// Konfiguration
// ----------------------------------------------------------------------------

// Setzen der Zugriffsadresse
//
void  nRF52840Radio::setAccessAddress(dword addr)
{
  dword prefix = addr >> 24;
  dword base = addr << 8;

  cfgData.base0   = NrfRadioPtr->BASE0        = base;
  cfgData.prefix0 = NrfRadioPtr->PREFIX0      = prefix;
  cfgData.txAddr  = NrfRadioPtr->TXADDRESS    = 0;
  cfgData.rxAddr  = NrfRadioPtr->RXADDRESSES  = 0x01;
}

// Telegrammparameter setzen
//
void  nRF52840Radio::setPacketParms(blePduType type)
{
  switch(type)
  {
    case bptAdv:
      cfgData.pCnf0     = NrfRadioPtr->PCNF0        = PCNF0_LFLEN(8) | PCNF0_S0LEN(1) | PCNF0_S1LEN(0);
      cfgData.pCnf1     = NrfRadioPtr->PCNF1        = PCNF1_MAXLEN(42) | PCNF1_BALEN(3) | PCNF1_WHITEEN(1);
      cfgData.modeCnf0  = NrfRadioPtr->MODECNF0     = 1;
      cfgData.crcCnf    = NrfRadioPtr->CRCCNF       = CRCCNF_LEN(3) | CRCCNF_SKIPADDR(1);
      cfgData.crcPoly   = NrfRadioPtr->CRCPOLY      = PolynomCRC;
      cfgData.crcInit   = NrfRadioPtr->CRCINIT      = AdvStartCRC;
      cfgData.packetPtr = NrfRadioPtr->PACKETPTR    = (dword) pduMem;
      cfgData.mode      = NrfRadioPtr->MODE         = 3;
      cfgData.dacnf     = NrfRadioPtr->DACNF        = 0x0FF00;
      break;

    case bptAux:
      break;
  }
}


// ----------------------------------------------------------------------------
// Steuerfunktionen und gezielte Prozessorzugriffe
// ----------------------------------------------------------------------------

// Schalten des Bewerbungskanals
//
void nRF52840Radio::setChannel(int nr)
{
  cfgData.frequency = NrfRadioPtr->FREQUENCY = channelList[nr].freq;
  cfgData.whiteInit = NrfRadioPtr->DATAWHITEIV = channelList[nr].idx;
}

// ----------------------------------------------------------------------------
//                      S e n d e n
// ----------------------------------------------------------------------------

// Einstellen der Sendeleistung
//
void  nRF52840Radio::setPower(int DBm)
{
  cfgData.txPower = NrfRadioPtr->TXPOWER = DBm;
}
// Senden eines Telegramms
// Es wird davon ausgeganen, das der Radio-Zustand = DISABLED ist
//
int nRF52840Radio::sendSync(bcPduPtr inPduPtr, TxStatePtr refState)
{
  int   retv = 0;
  NrfRadioPtr->INTENCLR = 0xFFFFFFFF;
  NrfRadioPtr->EVENTS_READY = 0;
  NrfRadioPtr->EVENTS_END = 0;
  memcpy((void *)pduMem, (void *)inPduPtr, sizeof(bcPdu));  // Daten kopieren
  if(refState != NULL)
    refState->prgLoopPrep = retv = 3 + sizeof(bcPdu);
  NrfRadioPtr->TASKS_TXEN = 1;                  // Starten des Anlaufes
  while(NrfRadioPtr->EVENTS_READY != 1) retv++; // Warten bis angelaufen
  if(refState != NULL)
    refState->evtLoopRampUp = retv - 3;
  NrfRadioPtr->TASKS_START = 1;                 // Starten des Sendevorgangs
  while(NrfRadioPtr->EVENTS_END != 1) retv++;   // Warten bis gesendet
  NrfRadioPtr->TASKS_DISABLE = 1;               // Sender abschalten
  if(refState != NULL)
  {
    refState->evtLoopTrans = retv - refState->evtLoopRampUp;
    refState->txBufferPtr = pduMem;
  }
  return(retv);
}

void  nRF52840Radio::send(bcPduPtr inPduPtr, TxStatePtr refState)
{

}

// ----------------------------------------------------------------------------
//                      E m p f a n g e n
// ----------------------------------------------------------------------------

// Starten des Datenempfangs
//
int nRF52840Radio::startRec()
{
  int   retv;
  NrfRadioPtr->INTENCLR = 0xFFFFFFFF;
  NrfRadioPtr->EVENTS_READY = 0;
  NrfRadioPtr->EVENTS_END = 0;
  NrfRadioPtr->EVENTS_ADDRESS = 0;
  NrfRadioPtr->EVENTS_PAYLOAD = 0;
  NrfRadioPtr->EVENTS_CRCOK = 0;
  NrfRadioPtr->EVENTS_CRCERROR = 0;
  NrfRadioPtr->TASKS_RXEN = 1;                  // Anlauf Empfänger starten
  retv = 8;
  while(NrfRadioPtr->EVENTS_READY != 1) retv++; // Warten bis angelaufen
  NrfRadioPtr->TASKS_START = 1;                 // Starten des Empfangs
  return(retv + 1);
}

// Fortsetzen des Datenempfangs
//
int nRF52840Radio::contRec()
{
  NrfRadioPtr->EVENTS_END = 0;
  NrfRadioPtr->EVENTS_ADDRESS = 0;
  NrfRadioPtr->EVENTS_PAYLOAD = 0;
  NrfRadioPtr->EVENTS_CRCOK = 0;
  NrfRadioPtr->EVENTS_CRCERROR = 0;
  NrfRadioPtr->TASKS_START = 1;                 // Starten des Empfangs
  return(6);
}

// Beenden des Datenempfangs
//
int nRF52840Radio::endRec()
{
  int   retv;
  NrfRadioPtr->EVENTS_DISABLED = 0;
  NrfRadioPtr->EVENTS_END = 0;
  NrfRadioPtr->EVENTS_ADDRESS = 0;
  NrfRadioPtr->EVENTS_PAYLOAD = 0;
  NrfRadioPtr->EVENTS_CRCOK = 0;
  NrfRadioPtr->EVENTS_CRCERROR = 0;
  NrfRadioPtr->TASKS_DISABLE = 1;               // Anlauf Empfänger beenden
  retv = 7;
  while(NrfRadioPtr->EVENTS_DISABLED != 1) retv++; // Warten bis abgelaufen
  return(retv);
}

// Empfangszustand abfragen
//
int nRF52840Radio::checkRec()
{
  int retv = 0;

  if(NrfRadioPtr->EVENTS_ADDRESS != 0)
    retv |= RECSTAT_ADDRESS;

  if(NrfRadioPtr->EVENTS_PAYLOAD != 0)
    retv |= RECSTAT_PAYLOAD;

  if(NrfRadioPtr->EVENTS_END != 0)
    retv |= RECSTAT_END;

  if(NrfRadioPtr->CRCSTATUS != 0)
    retv |= RECSTAT_CRCOK;

  return(retv);
}

int   nRF52840Radio::getRecData(bcPduPtr data, int max)
{
  int retv;
  byte *bPtr = (byte *) data;

  data->head = pduMem[0];
  retv = data->len  = pduMem[1];

  for(int i = 2; i < (retv + 2); i++)
  {
    if(i == max) break;
    bPtr[i] = pduMem[i];
  }

  return(retv);
}


// ----------------------------------------------------------------------------
//                      D e b u g - H i l f e n
// ----------------------------------------------------------------------------
//
int   nRF52840Radio::getPduMem(byte *dest, int start, int end)
{
  int i,j;

  j = 0;

  for(i = start; i < end; i++)
  {
    dest[j++] = pduMem[i];
  }
  return(j);
}







