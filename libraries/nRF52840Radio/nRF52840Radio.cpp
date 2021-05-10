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
  NrfRadioPtr->BASE0 = base;
  NrfRadioPtr->PREFIX0 = prefix;
  NrfRadioPtr->TXADDRESS = 0;
}

// Telegrammparameter setzen
//
void  nRF52840Radio::setPacketParms(blePduType type)
{
  switch(type)
  {
    case bptAdv:
      NrfRadioPtr->PCNF0      = PCNF0_LFLEN(8) | PCNF0_S0LEN(1) | PCNF0_S1LEN(8);
      NrfRadioPtr->PCNF1      = PCNF1_MAXLEN(40) | PCNF1_BALEN(3);
      NrfRadioPtr->MODECNF0   = 1;
      NrfRadioPtr->CRCCNF     = 0x03;
      NrfRadioPtr->PACKETPTR  = (dword) pduMem;
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

  switch(idx)
  {
    case 0:
      deltaF = FrqOffsAdvChn1;
      break;

    case 1:
      deltaF = FrqOffsADvChn2;
      break;

    case 2:
      deltaF = FrqOffsAdvChn3;
      break;
  }
  NrfRadioPtr->FREQUENCY = deltaF;
}

// einstellen der Sendeleistung
//
void  nRF52840Radio::setPower(int DBm)
{
  NrfRadioPtr->TXPOWER = DBm;
}
// Senden eines Telegramms
// Es wird davon ausgeganen, das der Radio-Zustand = DISABLED ist
//
int nRF52840Radio::sendSync(bcPduPtr inPduPtr)
{
  int   retv = 0;
  memcpy((void *)pduMem, (void *)inPduPtr, sizeof(bcPdu));  // Daten kopieren
  NrfRadioPtr->TASKS_TXEN = 1;                  // Starten des Anlaufes
  while(NrfRadioPtr->EVENTS_READY != 1) retv++; // Warten bis angelaufen
  NrfRadioPtr->TASKS_START = 1;                 // Starten des Sendevorgangs
  while(NrfRadioPtr->EVENTS_END != 1) retv++;   // Warten bis gesendet
  NrfRadioPtr->TASKS_DISABLE = 1;               // Sender abschalten
  return(retv);
}
