//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   nRF52840Radio.h
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
//

#ifndef NRF52840RADIO_H
#define NRF52840RADIO_H

#include "arduinoDefs.h"
#include "bleSpec.h"
#include "Beacon.h"

// ----------------------------------------------------------------------------

  typedef struct _NRF_RADIO_Type
  {
    volatile  dword  TASKS_TXEN;
    volatile  dword  TASKS_RXEN;
    volatile  dword  TASKS_START;
    volatile  dword  TASKS_STOP;
    volatile  dword  TASKS_DISABLE;
    volatile  dword  TASKS_RSSISTART;
    volatile  dword  TASKS_RSSISTOP;
    volatile  dword  TASKS_BCSTART;
    volatile  dword  TASKS_BCSTOP;
    volatile  dword  TASKS_EDSTART;
    volatile  dword  TASKS_EDSTOP;
    volatile  dword  TASKS_CCASTART;
    volatile  dword  TASKS_CCASTOP;
    volatile  dword  RESERVED0[51];
    volatile  dword  EVENTS_READY;
    volatile  dword  EVENTS_ADDRESS;
    volatile  dword  EVENTS_PAYLOAD;
    volatile  dword  EVENTS_END;
    volatile  dword  EVENTS_DISABLED;
    volatile  dword  EVENTS_DEVMATCH;
    volatile  dword  EVENTS_DEVMISS;
    volatile  dword  EVENTS_RSSIEND;
    volatile  dword  RESERVED1[2];
    volatile  dword  EVENTS_BCMATCH;
    volatile  dword  RESERVED2;
    volatile  dword  EVENTS_CRCOK;
    volatile  dword  EVENTS_CRCERROR;
    volatile  dword  EVENTS_FRAMESTART;
    volatile  dword  EVENTS_EDEND;
    volatile  dword  EVENTS_EDSTOPPED;
    volatile  dword  EVENTS_CCAIDLE;
    volatile  dword  EVENTS_CCABUSY;
    volatile  dword  EVENTS_CCASTOPPED;
    volatile  dword  EVENTS_RATEBOOST;
    volatile  dword  EVENTS_TXREADY;
    volatile  dword  EVENTS_RXREADY;
    volatile  dword  EVENTS_MHRMATCH;
    volatile  dword  RESERVED3[3];
    volatile  dword  EVENTS_PHYEND;
    volatile  dword  RESERVED4[36];
    volatile  dword  SHORTS;
    volatile  dword  RESERVED5[64];
    volatile  dword  INTENSET;
    volatile  dword  INTENCLR;
    volatile  dword  RESERVED6[61];
    volatile  dword  CRCSTATUS;
    volatile  dword  RESERVED7;
    volatile  dword  RXMATCH;
    volatile  dword  RXCRC;
    volatile  dword  DAI;
    volatile  dword  PDUSTAT;
    volatile  dword  RESERVED8[59];
    volatile  dword  PACKETPTR;
    volatile  dword  FREQUENCY;
    volatile  dword  TXPOWER;
    volatile  dword  MODE;
    volatile  dword  PCNF0;
    volatile  dword  PCNF1;
    volatile  dword  BASE0;
    volatile  dword  BASE1;
    volatile  dword  PREFIX0;
    volatile  dword  PREFIX1;
    volatile  dword  TXADDRESS;
    volatile  dword  RXADDRESSES;
    volatile  dword  CRCCNF;
    volatile  dword  CRCPOLY;
    volatile  dword  CRCINIT;
    volatile  dword  RESERVED9;
    volatile  dword  TIFS;
    volatile  dword  RSSISAMPLE;
    volatile  dword  RESERVED10;
    volatile  dword  STATE;
    volatile  dword  DATAWHITEIV;
    volatile  dword  RESERVED11[2];
    volatile  dword  BCC;
    volatile  dword  RESERVED12[39];
    volatile  dword  DAB[8];
    volatile  dword  DAP[8];
    volatile  dword  DACNF;
    volatile  dword  MHRMATCHCONF;
    volatile  dword  MHRMATCHMAS;
    volatile  dword  RESERVED13;
    volatile  dword  MODECNF0;
    volatile  dword  RESERVED14[3];
    volatile  dword  SFD;
    volatile  dword  EDCNT;
    volatile  dword  EDSAMPLE;
    volatile  dword  CCACTRL;
    volatile  dword  RESERVED15[611];
    volatile  dword  POWER;
  } *nrfRadioPtr;


#define NrfRadioBase    0x40001000UL
#define NrfRadioPtr     ((nrfRadioPtr) NrfRadioBase)

// ----------------------------------------------------------------------------

class nRF52840Radio
{
private:
  // --------------------------------------------------------------------------
  // Lokale Daten
  // --------------------------------------------------------------------------
  //
  volatile  byte  pduMem[256];

  // --------------------------------------------------------------------------
  // Lokale Funktionen
  // --------------------------------------------------------------------------
  //

public:
  // --------------------------------------------------------------------------
  // Initialisierungen der Basis-Klasse
  // --------------------------------------------------------------------------

  nRF52840Radio();

  // --------------------------------------------------------------------------
  // Steuerfunktionen
  // --------------------------------------------------------------------------
  //
  void advChannel(int idx);       // Schalten des Bewerbungskanals (advertizing)
  void send(bcPduPtr inPduPtr);   // Senden eines Telegramms

};

#endif // NRF52840RADIO_H

