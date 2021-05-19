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
  NrfRadioPtr->TASKS_DISABLE;   // Sender abgeschaltet
}

// ----------------------------------------------------------------------------
// Konfiguration
// ----------------------------------------------------------------------------

// Setzen der Zugriffsadresse
//
void  nRF52840Radio::setAccessAddress(dword addr)
{
  dword prefix = addr >> 24;
  dword base = addr & 0x00FFFFFF;

  cfgData.base0   = NrfRadioPtr->BASE0      = base;
  cfgData.prefix0 = NrfRadioPtr->PREFIX0    = prefix;
  cfgData.txAddr  = NrfRadioPtr->TXADDRESS  = 0;
}

// Telegrammparameter setzen
//
void  nRF52840Radio::setPacketParms(blePduType type)
{
  switch(type)
  {
    case bptAdv:
      cfgData.pCnf0     = NrfRadioPtr->PCNF0        = PCNF0_LFLEN(8) | PCNF0_S0LEN(1) | PCNF0_S1LEN(0);
      cfgData.pCnf1     = NrfRadioPtr->PCNF1        = PCNF1_MAXLEN(40) | PCNF1_BALEN(3) | PCNF1_WHITEEN(1);
      cfgData.modeCnf0  = NrfRadioPtr->MODECNF0     = 1;
      cfgData.crcCnf    = NrfRadioPtr->CRCCNF       = CRCCNF_LEN(3) | CRCCNF_SKIPADDR(1);
      cfgData.crcPoly   = NrfRadioPtr->CRCPOLY      = PolynomCRC;
      cfgData.crcInit   = NrfRadioPtr->CRCINIT      = AdvStartCRC;
      cfgData.packetPtr = NrfRadioPtr->PACKETPTR    = (dword) pduMem;
      cfgData.mode      = NrfRadioPtr->MODE         = 3;
      cfgData.dacnf     = NrfRadioPtr->DACNF        = 0x0FF00;
      cfgData.rxAddrEn  = NrfRadioPtr->RXADDRESSES  = 0x0FF;
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
void nRF52840Radio::advChannel(int idx)
{
  int deltaF = 0;
  int whitePres = 0x40;

  switch(idx)
  {
    case 0:
      deltaF = FrqOffsAdvChn1;
      whitePres = WhiteOffsAdvChn1;
      break;

    case 1:
      deltaF = FrqOffsADvChn2;
      whitePres = WhiteOffsAdvChn2;
      break;

    case 2:
      deltaF = FrqOffsAdvChn3;
      whitePres = WhiteOffsAdvChn3;
      break;
  }
  cfgData.frequency = NrfRadioPtr->FREQUENCY = deltaF;
  cfgData.whiteInit = NrfRadioPtr->DATAWHITEIV = whitePres;
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

// ----------------------------------------------------------------------------
//                      E m p f a n g e n
// ----------------------------------------------------------------------------

// Starten des Datenempfangs
//
int nRF52840Radio::startRec()
{
  int   retv;
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

// Empfangszustand abfragen
//

// ----------------------------------------------------------------------------
//                      D e b u g - H i l f e n
// ----------------------------------------------------------------------------

void nRF52840Radio::hexAsc(char * dest, byte val)
{
  char cv;

  cv = val >> 4;
  if(cv < 10)
    cv += 0x30;
  else
    cv += 0x37;
  dest[0] = cv;

  cv = val & 0x0F;
  if(cv < 10)
    cv += 0x30;
  else
    cv += 0x37;
  dest[1] = cv;

  dest[2] = '\0';
}

void nRF52840Radio::binAsc(char * dest, byte val)
{
  byte mask;

  mask = 0x01;

  for(int i = 0; i < 8; i++)
  {
    if((val & mask) != 0)
      dest[i] = '1';
    else
      dest[i] = '0';
    mask <<= 1;
  }

  dest[8] = '\0';
}

int   nRF52840Radio::cpyStr(char *dest, char *src)
{
  int   i = 0;

  while((dest[i] = src[i]) != '\0') i++;
  return(i);
}

int nRF52840Radio::binSeq(char *dest, dword dwVal)
{
  int   idx = 0;
  byte  bVal;

  bVal = dwVal >> 24;
  binAsc(&dest[idx], bVal);
  idx += 8;
  dest[idx++] = ' ';

  bVal = dwVal >> 16;
  binAsc(&dest[idx], bVal);
  idx += 8;
  dest[idx++] = ' ';

  bVal = dwVal >> 8;
  binAsc(&dest[idx], bVal);
  idx += 8;
  dest[idx++] = ' ';

  bVal = dwVal;
  binAsc(&dest[idx], bVal);
  idx += 8;

  dest[idx] = '\0';
  return(idx);
}

int nRF52840Radio::hexSeq(char *dest, dword dwVal)
{
  int   idx = 0;
  byte  bVal;

  bVal = dwVal >> 24;
  hexAsc(&dest[idx], bVal);
  idx += 2;

  bVal = dwVal >> 16;
  hexAsc(&dest[idx], bVal);
  idx += 2;

  bVal = dwVal >> 8;
  hexAsc(&dest[idx], bVal);
  idx += 2;

  bVal = dwVal;
  hexAsc(&dest[idx], bVal);
  idx += 2;

  dest[idx] = '\0';
  return(idx);
}

// Konfigurationsdaten lesbar aufbereiten
//
int nRF52840Radio::getDataCfg(char *dest, int select)
{
  int   idx;

  idx = cpyStr(dest, (char *) "Adr:");

  switch(select)
  {
    case 0:
      idx += hexSeq(&dest[idx], (dword) &NrfRadioPtr->PCNF0);
      idx += cpyStr(&dest[idx], (char *) " PCNF0 = ");
      idx += binSeq(&dest[idx], cfgData.pCnf0);
      break;

    case 1:
      idx += hexSeq(&dest[idx], (dword) &NrfRadioPtr->PCNF1);
      idx += cpyStr(&dest[idx], (char *) " PCNF1 = ");
      idx += binSeq(&dest[idx], cfgData.pCnf1);
      break;
  }
  return(idx);
}






