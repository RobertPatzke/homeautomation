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
#include "IntrfRadio.h"

#define nRF52840RadioDEB

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
  volatile  dword  RESERVED3[2];
  volatile  dword  EVENTS_SYNC;
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



#define NrfRadioBase    0x40001000
#define NrfRadioPtr     ((nrfRadioPtr) NrfRadioBase)

#ifndef NrfPowerBase
#define NrfPowerBase    0x40000000
#define nrfPowerDCDCEN  ((dword *) 0x40000578)
#endif

#ifndef NrfClockBase
#define NrfClockBase    0x40000000
#define nrfClockTASKS_HFCLKSTART  ((dword *) 0x40000000)
#endif

// Direktverbindungen (shortcuts) zwischen events und Tasks
//
#define NrfScREADY_START    0x00000001
#define NrfScEND_DISABLE    0x00000002
#define NrfScDISABLED_TXEN  0x00000004
#define NrfScDISABLED_RXEN  0x00000008
#define NrfScTXREADY_START  0x00040000
#define NrfScRXREADY_START  0x00080000

// Interrupts
//
#define NrfIntREADY         0x00000001
#define NrfIntADDRESS       0x00000002
#define NrfIntPAYLOAD       0x00000004
#define NrfIntEND           0x00000008
#define NrfIntDISABLED      0x00000010
#define NrfIntRSSIEND       0x00000080
#define NrfIntTXREADY       0x00200000
#define NrfIntRXREADY       0x00400000

// Zustände
//
#define NrfStDISABLED       0
#define NrfStRXRU           1
#define NrfStRXIDLE         2
#define NrfStRX             3
#define NrfStRXDISABLE      4
#define NrfStTXRU           9
#define NrfStTXIDLE         10
#define NrfStTX             11
#define NrfStTXDISABLE      12

// Festlegungen für die Paketkonfigurationsregister
//

#define PCNF0_LFLEN(x)    x
// Anzahl der Bits im Längenfeld (0-15)

#define PCNF0_S0LEN(x)    (x << 8)
// Länge des Header0 (S0) in Bytes (0 oder 1)

#define PCNF0_S1LEN(x)    (x << 16)
// Länge des S1-Feldes in Bit (0 bis 15)

#define PCNF1_MAXLEN(x)   x
// Maximale Telegrammlänge (0 bis 255)

#define PCNF1_BALEN(x)    (x << 16)
// Basislänge der Zugriffsadresse (Access Address, 2-4)

#define PCNF1_WHITEEN(x)  (x << 25)
// Whitening (Bitmischung) Ein/Aus (1/0)

// Festlegungen für die CRC-Generierung
//

#define CRCCNF_LEN(x)     x
// Anzahl der Bytes für CRC (0-3)

#define CRCCNF_SKIPADDR(x)  (x << 8)
// Zugriffsadresse (Access Address) nicht im CRC (1), im CRC (0)


typedef struct _nrf52840Cfg
{
  dword   pCnf0;
  dword   pCnf1;
  dword   whiteInit;
  dword   modeCnf0;
  dword   crcPoly;
  dword   crcInit;
  dword   crcCnf;
  dword   packetPtr;
  dword   frequency;
  dword   txPower;
  dword   mode;
  dword   dacnf;
  dword   rxAddrEn;
  dword   base0;
  dword   prefix0;
  dword   txAddr;
  dword   rxAddr;

}nrf52840Cfg, *nrf52840CfgPtr;

// ----------------------------------------------------------------------------

class nRF52840Radio : IntrfRadio
{
private:
  // --------------------------------------------------------------------------
  // Lokale Daten
  // --------------------------------------------------------------------------
  //
  byte        pduMem[256];
  byte        pduSentE[256];
  byte        pduSentS[256];

  bcPduPtr    pmPtr;
  bcPduPtr    pePtr;
  bcPduPtr    psPtr;

  nrf52840Cfg cfgData;

  bool        recMode;
  bool        eadM;
  bool        nakM;
  bool        comFin;
  bool        comError;
  bool        newValues;

  dword       irqCounter;
  TxMode      trfMode;

  TxStatistics    statList[NrOfTxModes];
  TxStatisticsPtr statisticPtr;

public:
  // --------------------------------------------------------------------------
  // Initialisierungen der Basis-Klasse
  // --------------------------------------------------------------------------

  nRF52840Radio();

  // --------------------------------------------------------------------------
  // Konfigurationen
  // --------------------------------------------------------------------------
  //
  void  begin();
  void  setAccessAddress(dword addr); // Setzen der Zugriffsadresse
  void  setPacketParms(blePduType type);

  // --------------------------------------------------------------------------
  // Steuerfunktionen
  // --------------------------------------------------------------------------
  //
  void  setChannel(int nr);           // Schalten physikalischer Kanal
  int   sendSync(bcPduPtr inPduPtr, TxStatePtr refState);

  void  send(bcPduPtr inPduPtr, TxMode txMode);
  void  send(bcPduPtr inPduPtrE, bcPduPtr inPduPtrS, TxMode txMode, bool newValues);
  int   getRecData(bcPduPtr data, TxMode txMode, int max);  // Empfangene Daten lesen

  void  disable(TxMode txMode);
  bool  disabled(TxMode txMode);      // Abfrage, ob ausgeschaltet
  void  cont(TxMode txMode);
  bool  fin(TxMode txMode, bool *err);
                                      // Senden eines Telegramms (und warten)
  int   startRec();                   // Datenempfang starten
  int   contRec();                    // Datenempfang fortsetzen
  int   endRec();                     // Datenempfang beenden
  int   checkRec();                   // Zustand Datenempfang feststellen
  int   getRecData(bcPduPtr data, int max);  // Empfangene Daten lesen

  void  setPower(int DBm);            // Leistung des Senders in DBm

  void  readCheckCfg();               // Konfigurationsdaten auslesen

  static  nRF52840Radio *instPtr0;
  static  void irqHandler0();

  void    irqHandler();

  // --------------------------------------------------------------------------
  // Datenzugriffe
  // --------------------------------------------------------------------------
  //
  int   getStatistics(TxStatisticsPtr dest);
  int   getState();

  // ----------------------------------------------------------------------------
  //                      D e b u g - H i l f e n
  // ----------------------------------------------------------------------------
  //
  int   getPduMem(byte *dest, int start, int end);
  int   getPduSent(byte *dest, int start, int end);


};


#endif // NRF52840RADIO_H

