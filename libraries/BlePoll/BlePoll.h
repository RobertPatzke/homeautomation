//-----------------------------------------------------------------------------
// Thema:   Social Manufacturing Network / Development Environment
// Datei:   BlePoll.h
// Editor:  Robert Patzke
// URI/URL: www.mfp-portal.de
//-----------------------------------------------------------------------------
// Lizenz:  CC-BY-SA  (wikipedia: Creative Commons)
// Datum:   1. September 2021
//
// Diese Bibliothek (Klassse) enthält diverse Ressourcen zur Kommunikation
// über BLE-Funkmodule auf niedriger Ebene, also dem direkten Telegrammaustausch.
// Darauf aufbauend sollen mehrkanalige Messeinrichtungen mit möglichst
// geringen Latenzzeiten entwickelt werden.
//

#ifndef BlePoll_h
#define BlePoll_h
// ----------------------------------------------------------------------------

#include "stddef.h"
#include "string.h"
#include "arduinoDefs.h"
#include "bleSpec.h"
#include "IntrfRadio.h"

#define MAXSLAVE  100

// Betriebsmodus (Polling und Slave)
//
typedef enum _PlMode
{
  plmIdle,        // Ruhezustand, Gerät nicht im Netz aktiv
  plmTest,        // Low Level Tests
  plmEmpty,       // Leeres Polling (Aufbau Adressliste)
  plmScan,        // Daten aller aktiven Teilnehmer holen (Master)
  plmSoaapM,      // Vollständiger Betrieb SOAAP (Master)
  plmSoaapS,      // Vollständiger Betrieb SOAAP (Slave)
  plmXchg         // Daten übertragen (Slave, beide Richtungen)
} PlMode;

// Grundsätzliche Datenstruktur für die Nutzdaten (ohne 6 Bytes Adresse)
//
typedef struct _PlPduBase   // maximale Länge Beacon = 31 Bytes
{
  byte  counter;      // zyklischer Telegrammmzähler
  byte  type;         // Kennzeichnung der Datenstruktur (AppType)
  byte  plData[29];   // weitere spezifische Nutzdaten
} PlPduBase, *PlPduBasePtr;

// Erweiterte Datenstruktur für die Nutzdaten (ohne 6 Bytes Adresse)
// zur Zeit noch nicht genutzt
//
typedef struct _PlPduExtd   // grundsätzliche maximale Länge = 249 Bytes
{
  byte  counter;      // zyklischer Telegrammmzähler
  byte  type;         // Kennzeichnung der Datenstruktur (AppType)
  byte  plData[247];  // weitere spezifische Nutzdaten
} PlPduExtd, *PlPduExtdPtr;

// Datentypen (appId in plPduBase)
//
typedef enum _PlpType
{
  plptError,        // Fehler erkannt
  plptEmpty,        // Leeres Telegramm (nur adresse)
  plptBasic,        // Grundlegende Bytestruktur
  plptFullMeas,     // Maximale Belegung mit 16-Bit Messwerten (word)
  plptMeas3,        // 3 Messwerte (1 Raumsensor)
  plptMeas6,        // 6 Messwerte (2 Raumsensoren)
  plptMeas9,        // 9 Messwerte (3 Raumsensoren)
  plptMeas12,       // 12 Messwerte (9 + 6 Byte Extradaten)
  plptMeas13,       // 13 Messwerte (9 + 8 Byte Extradaten)
  plptMeasX,        // Variable Anzahl Messwerte
  plptCtrl0,        // Keine Steuerung
  plptCtrl2,        // 2 Byte Steuerung
  plptCtrl27,       // 27 Byte Steuerung
  plptCtrlX,        // Variable Anzahl Steuerbytes
  plptMsg           // (Quittierte) Meldung an Slave
} PlpType, *PlpTypePtr;

// Spezifische Datenstrukturen
//
typedef struct _PlpFullMeas   // Ausnahme für vordefinierte Spezialfälle
{
  byte    counter;    // zyklischer Telegrammmzähler
  byte    type;       // Kennzeichnung der Datenstruktur (AppType)
  word    meas[14];   // Liste von 14 Messwerten
  byte    appId;      // Kennzeichnung für Dateninhalte (PlpType)
  byte    align;      // Wird nicht gesendet, kennzeichnet Alignement
} PlpFullMeas, *PlpFullMeasPtr;

typedef struct _PlpMeas3      // Länge 10 (+ 6 Bytes Adresse)
{
  byte    counter;    // zyklischer Telegrammmzähler
  byte    type;       // Kennzeichnung der Datenstruktur (AppType)
  byte    appId;      // Kennzeichnung für Dateninhalte (PlpType)
  byte    measCnt;    // Zähler für Messwertaktualisierung
  word    meas[3];    // Liste von 3 Messwerten
} PlpMeas3, *PlpMeas3Ptr;

typedef struct _PlpMeas6      // Länge 16 (+ 6 Bytes Adresse)
{
  byte    counter;    // zyklischer Telegrammmzähler
  byte    type;       // Kennzeichnung der Datenstruktur (AppType)
  byte    appId;      // Kennzeichnung für Dateninhalte (PlpType)
  byte    measCnt;    // Zähler für Messwertaktualisierung
  word    meas[6];    // Liste von 6 Messwerten
} PlpMeas6, *PlpMeas6Ptr;

typedef struct _PlpMeas9      // Länge 22 (+ 6 Bytes Adresse)
{
  byte    counter;    // zyklischer Telegrammmzähler
  byte    type;       // Kennzeichnung der Datenstruktur (AppType)
  byte    appId;      // Kennzeichnung für Dateninhalte (PlpType)
  byte    measCnt;    // Zähler für Messwertaktualisierung
  word    meas[9];    // Liste von 9 Messwerten
} PlpMeas9, *PlpMeas9Ptr;

typedef struct _PlpMeas12      // Länge 28 (+ 6 Bytes Adresse)
{
  byte    counter;    // zyklischer Telegrammmzähler
  byte    type;       // Kennzeichnung der Datenstruktur (AppType)
  byte    appId;      // Kennzeichnung für Dateninhalte (PlpType)
  byte    measCnt;    // Zähler für Messwertaktualisierung
  word    meas[12];   // Liste von 12 Messwerten
} PlpMeas12, *PlpMeas12Ptr;

typedef struct _PlpMeas13      // Länge 30 (+ 6 Bytes Adresse)
{
  byte    counter;    // zyklischer Telegrammmzähler
  byte    type;       // Kennzeichnung der Datenstruktur (AppType)
  byte    appId;      // Kennzeichnung für Dateninhalte (PlpType)
  byte    measCnt;    // Zähler für Messwertaktualisierung
  word    meas[13];   // Liste von 13 Messwerten
} PlpMeas13, *PlpMeas13Ptr;

typedef struct _PlpCtrl2        // Länge 6 (+ 6 Bytes Adresse)
{
  byte    counter;    // zyklischer Telegrammmzähler
  byte    type;       // Kennzeichnung der Datenstruktur (AppType)
  byte    appId;      // Kennzeichnung für Dateninhalte (PlpType)
  byte    ctrlCnt;    // Zähler für Kommandoaktualisierung
  byte    ctrl[2];    // Liste von 2 Steuerbytes
} PlpCtrl2, *PlpCtrl2Ptr;

typedef struct _PlpCtrl27        // Länge 31 (+ 6 Bytes Adresse)
{
  byte    counter;    // zyklischer Telegrammmzähler
  byte    type;       // Kennzeichnung der Datenstruktur (AppType)
  byte    appId;      // Kennzeichnung für Dateninhalte (PlpType)
  byte    ctrlCnt;    // Zähler für Kommandoaktualisierung
  byte    ctrl[27];   // Liste von 27 Steuerbytes
} PlpCtrl27, *PlpCtrl27Ptr;

// Identifikator für die Art der Daten
//
typedef enum _MeasId
{
  app               // Gestaltung/Bedeutung der Daten aus Anwendung
} MeasId, *MeasIdPtr;

typedef struct _Slave
{
  dword     timeOut;        // Wartezeit beim Polling in Mikrosekunden
  dword     cntTo;          // Zähler für ausbleibende Antworten
  dword     cntErrCrc;      // Zähler für CRC-Fehler bei der Übertragung
  dword     cntNakEP;       // Zähler für NAK-Antworten beim Empty-Polling
  dword     cntAckDP;       // Zähler für ACK-Antworten beim Data-Polling
  dword     cntLostPdu;     // Zähler für verlorene Telegramme
  dword     cntLostMeas;    // Zähler für verlorene Messwerte
  dword     delayCnt;       // Verzögerung (Polldurchläufe) vor Fehlerzählung
  byte      adr;            // Adresse (Nummer) des Slave (1-255)
  byte      area;           // Einsatzgebiet des Slave (Adresserweiterung)
  byte      chn;            // Aktueller Übertragungskanal (0-39)
  byte      pIdx;           // Index in der aktuellen Pollingliste
  word      prioSet;        // Anfangspriorität (rel. Häufigkeit) beim Polling
  word      minPrio;        // Minimale Priorität (maximale Prioritätszahl)
  PlPduBase result;         // Daten vom Slave
  PlPduBase control;        // Daten zum Slave
  bool      newPdu;         // Kennzeichnung für neues Telegramm
  bool      rspOk;          // Rücksetz-Kennnzeichnung für neues Telegramm
  bool      newMeas;        // Kennzeichnung für neuen Messwert
  byte      oldPduCount;    // Merker für die Telegrammüberwachung
  byte      oldMeasCount;   // Merker für die Messwertüberwachung
  byte      rspCtrlCount;   // Merker für Steuerungsüberwachung
} Slave, *SlavePtr;


typedef struct _PollInfo
{
  dword   aliens;     // Anzahl der netzfremden Aktivitäten
  dword   wrongs;     // Anzahl ungewünschter Netzaktivitäten
} PollInfo, *PollInfoPtr;

typedef struct _PollState
{
  byte    slIdx;      // Index in der Slave-Liste
  byte    status;     // Zustand
  word    prioCnt;    // Prioritätszähler
} PollState, *PollStatePtr;

typedef bool (*cbDataPtr)(PlpType dataType, byte *dest);

#define psSlaveWasPresent 0x01
#define psSlaveIsPresent  0x02

// ----------------------------------------------------------------------------
//                            B l e P o l l
// ----------------------------------------------------------------------------
class BlePoll
{
public:
  // -------------------------------------------------------------------------
  // Öffentliche Datentypen
  // -------------------------------------------------------------------------
  //
  typedef enum _ComType
  {
    ctMASTER,
    ctSLAVE,
    ctHybrid
  } ComType;

  // Identifikator für die Anwendung
  //
  typedef enum _AppType
  {
    atDefault,        // Standard-Default-Anwendung
    atTestSend,       // einfacher Sendetest (Soaap)
    atSOAAP,          // Steuerung optischer und akustischer Ausgaben für Performance-Künstler
    atDevSOAAP,       // Entwicklerbetrieb für SOAAP
    atDHA             // Dezentrale Hausautomatisierung
  } AppType;

  // --------------------------------------------------------------------------
  // Öffentliche Daten
  // --------------------------------------------------------------------------
  //
  bool    DataExchange;

private:
  // -------------------------------------------------------------------------
  // Private Datentypen
  // -------------------------------------------------------------------------
  //

  typedef void (BlePoll::*cbVector)(void);
  typedef dword (*MicsecFuPtr)(void);

  // --------------------------------------------------------------------------
  // Lokale Daten
  // --------------------------------------------------------------------------
  //
  IntrfRadio    *radio;
  bcPdu         pduOut;
  bcPdu         pduIn;
  cbVector      nextState;
  MicsecFuPtr   micSec;
  cbDataPtr     cbData;
  dword         toSet;
  dword         toValue;
  dword         cycleMics;
  dword         cycleCnt;
  dword         wdTimeOut;

  int           chn;
  int           adr;
  int           area;
  bool          master;
  bool          eadr;
  bool          nak;
  bool          crcError;

  PlMode        plMode;
  PlMode        oldPlMode;
  PlpType       plpType;

  Slave         slaveList[MAXSLAVE+1];
  SlavePtr      curSlave;
  int           slaveIdx; // ist z.Zt. identisch mit Slaveadresse adr
  PollState     pollList[MAXSLAVE+1];
  PollStatePtr  curPoll;
  int           pollIdx;
  int           pollNr;
  int           pollMaxNr;

  int           maxAdr;
  dword         cntPolling;
  dword         cntAllRecs;
  dword         cntAllTo;
  bool          pollStop;
  bool          pollStopped;

  dword         cntAllNaks;
  dword         cntAlien;
  dword         cntWrong;
  dword         cntWaitDisabled;

  dword         bleState;
  dword         runCounter;

  TxStatistics  statistic;
  PlpMeas13     valuePdu;
  PlpCtrl27     ctrlPdu;
  bool          newValue;

  // Einstellungen für den Anwendungsbetrieb
  //
  bool          fullCycle;      // Vollständige Anwendung (EP & Data)
  int           epCycleTotal;   // Anzahl der leeren Pollings gesamt
  int           epCycleRun;     // Anzahl der leeren Pollings nach Kontakt
  dword         epTimeOut;      // Time-Out in Mikrosekunden


  // --------------------------------------------------------------------------
  // Lokale Funktionen
  // --------------------------------------------------------------------------
  //
  void setPduAddress();
  void setPduAddress(bcPduPtr pduPtr);
  void setTimeOut(dword value);
  bool timeOut();
  bool getValues(bcPduPtr pduPtr);

  // Zustandsmaschine
  // -----------------------------
  void smInit();
  void smIdle();

  // Leeres Polling Master
  //
  void smStartEP();
  void smReqEadr();
  void smWaitNak();
  void smEndEP();

  // Leeres Polling Slave
  //
  void smWaitEadr();
  void smEvalPoll();

  // Datenübertragung
  //
  void smStartCom();

  // Master: Master -> Slave
  //
  void smReqComE();
  void smWaitAckComE();
  void smEndComE();

  // Master: Slave -> Master
  //
  void smReqComS();
  void smWaitAckComS();
  void smEndComS();

  // Slave: Master <-> Slave
  //
  void smStartComES();
  void smWaitComES();
  void smEvalComES();

  // Test
  //
  void smStartTest();
  void smWaitTest();
  void smLoopTest();


public:
  // --------------------------------------------------------------------------
  // Initialisierungen
  // --------------------------------------------------------------------------
  BlePoll(IntrfRadio *refRadio, dword inCycleMics);
  BlePoll(IntrfRadio *refRadio, MicsecFuPtr inMicroFu);
  void init(IntrfRadio *refRadio, dword inCycleMics, MicsecFuPtr inMicroFu);

  void begin(ComType typeIn, int adrIn, AppType appType, dword watchDog);
  void setCbDataPtr(cbDataPtr cbPtr);

  // --------------------------------------------------------------------------
  // Konfiguration
  // --------------------------------------------------------------------------
  //
  void setPollAddress(int chnIn, int adrIn, int areaIn, bool masterIn, bool eadrIn, bool nakIn);
  void setEmptyPollParams(int cycleTotal, int cycleRun, dword timeOut);
  void setDataPollParams(int slAdr, int prio, int minPrio, dword timeOut);

  // --------------------------------------------------------------------------
  // Steuerung des Telegrammaustausches (Polling)
  // --------------------------------------------------------------------------
  //
  void run();       // Ablaufsteuerung (CPU-Übergabe) dieses Moduls

  void updControl(int adr, byte *ctrlList, int nr);   // neue Steuerungsdaten
  bool ackTrans(int adr);         // Bestätigung Steuerungsdaten übertragen
  bool ackControl(int adr);       // Bestätigung Steuerung ausgeführt


  // Test
  //

  // Leeres Polling
  //
  void stopEP();
  void resumeEP();
  bool stoppedEP();

  // Laufender Betrieb
  //
  void start(PlMode inPlMode);

  // --------------------------------------------------------------------------
  // Zugriff auf Polling-Informationen
  // --------------------------------------------------------------------------
  //
  int   getSlaveList(byte *dest, int maxByte);
  void  resetPollCounters();

  // --------------------------------------------------------------------------
  // Zugriff auf Slavedaten
  // --------------------------------------------------------------------------
  // Der Index wird von 0 an ausgewertet. Allerdings ist [0] in der Slave-Liste
  // auf den Index [1] abzubilden, weil Slave[0] für besondere Aufgaben
  // reserviert und für den Anwender nicht zugänglich ist.
  //
  bool      measAvail(int slIdx);   // Feststellen, ob neue Messwerte da sind
  int       getArea(int slIdx);     // Wert der Area auslesen
  PlpType   getAppId(int slIdx);    // Wert der AppId (BlePoll) auslesen
  int       getMeas(int slIdx, byte *dest);    // Messwerte übergeben



  // --------------------------------------------------------------------------
  // Debugging
  // --------------------------------------------------------------------------
  //
  dword debGetDword(int idx);
  dword getStatistics(TxStatisticsPtr dest);

  SlavePtr      getSlavePtr(int idx);
  PollStatePtr  getPollPtr(int idx);
};


// ----------------------------------------------------------------------------
#endif // BlePoll_h
